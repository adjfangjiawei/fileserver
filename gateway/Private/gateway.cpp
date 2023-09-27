
#include <BookServer/book.h>
#include <gateway/gateway.h>
#include <spdlog/spdlog.h>

#include <BookServer/jb/book.hxx>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/regex.hpp>
#include <boost/system.hpp>
#include <boost/url.hpp>
#include <boost/url/parse.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <odb/transaction.hxx>
#include <thread>
using database = std::shared_ptr<odb::mysql::database>;
using transaction = odb::core::transaction;

#if defined(BOOST_ASIO_HAS_CO_AWAIT)

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

using tcp_stream = typename beast::tcp_stream::rebind_executor<net::use_awaitable_t<>::executor_with_default<net::any_io_executor>>::other;

// 根据文件扩展名获得合适的媒体类型.
beast::string_view mime_type(beast::string_view path) {
    using beast::iequals;
    auto const ext = [&path] {
        auto const pos = path.rfind(".");
        if (pos == beast::string_view::npos) {
            return beast::string_view{};
        }
        return path.substr(pos);
    }();
    if (iequals(ext, ".htm")) return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php")) return "text/html";
    if (iequals(ext, ".css")) return "text/css";
    if (iequals(ext, ".txt")) return "text/plain";
    if (iequals(ext, ".js")) return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml")) return "application/xml";
    if (iequals(ext, ".swf")) return "application/x-shockwave-flash";
    if (iequals(ext, ".flv")) return "video/x-flv";
    if (iequals(ext, ".png")) return "image/png";
    if (iequals(ext, ".jpe")) return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg")) return "image/jpeg";
    if (iequals(ext, ".gif")) return "image/gif";
    if (iequals(ext, ".bmp")) return "image/bmp";
    if (iequals(ext, ".ico")) return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif")) return "image/tiff";
    if (iequals(ext, ".svg")) return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// 将一个Http地址拼接到本地的文件系统路径上，这样返回的路径就可以在不同的平台自适应.
std::string path_cat(beast::string_view base, beast::string_view path) {
    if (base.empty()) {
        return std::string(path);
    }
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if (result.back() == path_separator) {
        result.resize(result.size() - 1);
    }
    result.append(path.data(), path.size());
    for (auto& c : result) {
        if (c == '/') {
            c = path_separator;
        }
    }
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator) {
        result.resize(result.size() - 1);
    }
    result.append(path.data(), path.size());
#endif
    return result;
}

typedef std::uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;

constexpr hash_t basis = 0xCBF29CE484222325ull;

hash_t hash_(char const* str)  //功能和hash_compile_time功能相同，处理阶段不同，一个是运行时没，一个是编译时

{
    hash_t ret{basis};

    while (*str) {
        ret ^= *str;

        ret *= prime;

        str++;
    }

    return ret;
}

//下面定义的预处理函数，使得和上面的hash_值得到的结果一样，因为字串需要调用hash_函数将字串转化成hash序列，hash_compile_time函数在预编译时也能达到相同值，所以两个函数是一一对应的

constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)

{
    return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

//这里是为了定义一个用户字面量运算符_hash
constexpr unsigned long long operator"" _hash(char const* p, size_t)

{
    return hash_compile_time(p);
}

// 对一个给定的请求给予反馈
template <class Body, class Allcator>
http::message_generator handle_request(beast::string_view doc_root, http::request<Body, http::basic_fields<Allcator>>&& req) {
    // 返回一个错误请求的回复
    auto const bad_request = [&req](beast::string_view why) {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // 返回一个没有找到，记录不存在的恢复
    auto const not_found = [&req](beast::string_view target) {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // 返回一个内部服务错误的恢复
    auto const server_error = [&req](beast::string_view what) {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    // 确保是正确的http请求方法
    // if (req.method() != http::verb::get && req.method() != http::verb::head) {
    //     return bad_request("Unknown HTTP-method");
    // }

    std::string url_target = req.target();
    spdlog::info("{}", req.body());
    nlohmann::json request = nlohmann::json::parse(req.body());
    nlohmann::json response;
    database db(new odb::mysql::database("root", "123456", "odb_test", "198.1.17.252", 30306));
    switch (hash_(url_target.c_str())) {
#include "bookserver.def"
        default:
            break;
    }
    // 需要请求的路径是绝对路径并且不包含 "..".
    if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != beast::string_view::npos) {
        return bad_request("Illegal request-target");
    }

    // boost::system::result<boost::url_view> url = boost::urls::parse_origin_form(req.target());
    // 构建请求的文件的路径
    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/') {
        path.append("index.html");
    }

    // 打开文件
    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // 处理文件不存在的情况
    if (ec == beast::errc::no_such_file_or_directory) {
        return not_found(req.target());
    }

    // 处理一个未知的错误
    if (ec) {
        return server_error(ec.message());
    }

    // 把文件尺寸缓存下来，因为之后move了需要尺寸
    auto const size = body.size();

    // 对于HEAD请求的恢复
    if (req.method() == http::verb::head) {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    // 对于GET请求的回复
    http::response<http::file_body> res{std::piecewise_construct, std::make_tuple(std::move(body)), std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
}

// 处理一个http服务器的连接
net::awaitable<void> do_session(tcp_stream stream, std::shared_ptr<std::string const> doc_root) {
    // 这个缓冲区有避免交替读
    beast::flat_buffer buffer;

    // 这个lambda被用来发送信息
    try {
        for (;;) {
            // 设置超时时间
            stream.expires_after(std::chrono::seconds(30));

            // 读一个请求
            http::request<http::string_body> req;
            co_await http::async_read(stream, buffer, req);

            // 处理请求
            http::message_generator msg = handle_request(*doc_root, std::move(req));

            // 保活
            bool keep_alive = msg.keep_alive();

            // 发送回复
            co_await beast::async_write(stream, std::move(msg), net::use_awaitable);

            if (!keep_alive) {
                break;
            }
        }
    } catch (boost::system::system_error const& se) {
        if (se.code() != http::error::end_of_stream) {
            throw;
        }
    }

    // 关闭TCP
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_send, ec);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
// 接收一个进入的连接并且启动会话
net::awaitable<void> do_listen(tcp::endpoint endpoint, std::shared_ptr<std::string const> doc_root) {
    // 打开一个接收器
    auto acceptor = net::use_awaitable.as_default_on(tcp::acceptor(co_await net::this_coro::executor));
    acceptor.open(endpoint.protocol());

    // 设置允许地址重用
    acceptor.set_option(net::socket_base::reuse_address(true));

    // 绑定到服务地址
    acceptor.bind(endpoint);

    // 开始监听连接
    acceptor.listen(net::socket_base::max_listen_connections);

    for (;;) {
        boost::asio::co_spawn(acceptor.get_executor(), do_session(tcp_stream(co_await acceptor.async_accept()), doc_root), [](std::exception_ptr e) {
            if (e) {
                try {
                    std::rethrow_exception(e);
                } catch (std::exception const& e) {
                    { std::cerr << "Error insession: " << e.what() << std::endl; }
                }
            }
        });
    }
}

void gateway::initGateway() {
    // 设置监听地址
    auto const address = net::ip::make_address("0.0.0.0");

    // 设置监听端口
    auto const port = static_cast<unsigned short>(std::atoi("81"));

    // 设置根目录
    auto const doc_root = std::make_shared<std::string>("/mnt/e");

    // 创建io_context
    net::io_context ioc{1};

    // 生成一个监听
    boost::asio::co_spawn(ioc, do_listen(tcp::endpoint{address, port}, doc_root), [](std::exception_ptr e) {
        if (e) {
            try {
                std::rethrow_exception(e);
            } catch (std::exception const& e) {
                std::cerr << "Error in listen: " << e.what() << std::endl;
            }
        }
    });

    // 运行IO服务
    std::vector<std::thread> v;
    for (auto i = 0; i < 0; i++) {
        v.emplace_back([&ioc] { ioc.run(); });
    }
    ioc.run();
}
#endif