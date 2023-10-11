#include <gateway/gateway.h>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// // 进行HTTP GET并且打印返回的消息
// net::awaitable<void> do_cession(std::string host, std::string port, std::string target, int version) {
//     auto resolver = net::use_awaitable.as_default_on(tcp::resolver(co_await net::this_coro::executor));
//     auto stream = net::use_awaitable.as_default_on(beast::tcp_stream(co_await net::this_coro::executor));

//     // 查看域名
//     auto const results = co_await resolver.async_resolve(host, port);

//     // 设置超时时间
//     stream.expires_after(std::chrono::seconds(30));

//     // 连接这个地址
//     co_await stream.async_connect(results);

//     // 设置一个HTTP GET的请求消息
//     http::request<http::string_body> req{http::verb::get, target, version};
//     req.set(http::field::host, host);
//     req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

//     // 设置超时时间
//     stream.expires_after(std::chrono::seconds(30));

//     // 发送HTTP请求到远程主机
//     co_await http::async_write(stream, req);

//     // 这个缓冲区用于读取，并且必须持久存在
//     beast::flat_buffer b;

//     // 声明一个容器来接收返回体
//     http::response<http::dynamic_body> res;

//     // 接受HTTP的返回
//     co_await http::async_read(stream, b, res);

//     // 把返回体写到标准输出
//     std::cout << res << std::endl;

//     // 优雅地关闭套接字
//     beast::error_code ec;
//     stream.socket().shutdown(tcp::socket::shutdown_both, ec);
//     if (ec && ec != beast::errc::not_connected) {
//         throw beast::system_error{ec, "shutdown"};
//     }
// }

#include <spdlog/spdlog.h>

#include <BookServer/jb/book.hxx>
#include <iostream>
#include <nlohmann/json.hpp>
void clientMain() {
    try {
        auto const host = "172.23.57.2";
        auto const port = "81";
        auto const target = "/bookserver/createbook";

        // The io_context is required for all I/O
        net::io_context ioc;

        // The object perform our I/O
        tcp::resolver resolver{ioc};
        beast::tcp_stream stream{ioc};

        // Lool up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);

        // Set up an HTTP post message
        http::request<http::string_body> req{http::verb::post, target, 11};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        nlohmann::json book = JbBook{.display_name = "红楼梦"};
        req.body() = book.dump();
        req.content_length(book.dump().size());
        spdlog::info("{}", book.dump());
        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declared a container to hold the response
        http::response<http::dynamic_body> res;

        // Recerive the HTTP response
        http::read(stream, buffer, res);
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}