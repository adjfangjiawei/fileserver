#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#if defined(BOOST_ASIO_HAS_CO_AWAIT)

#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// 进行HTTP GET并且打印返回的消息
net::awaitable<void> do_cession(std::string host, std::string port, std::string target, int version) {
    auto resolver = net::use_awaitable.as_default_on(tcp::resolver(co_await net::this_coro::executor));
    auto stream = net::use_awaitable.as_default_on(beast::tcp_stream(co_await net::this_coro::executor));

    // 查看域名
    auto const results = co_await resolver.async_resolve(host, port);

    // 设置超时时间
    stream.expires_after(std::chrono::seconds(30));

    // 连接这个地址
    co_await stream.async_connect(results);

    // 设置一个HTTP GET的请求消息
    http::request<http::string_body> req{http::verb::get, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // 设置超时时间
    stream.expires_after(std::chrono::seconds(30));

    // 发送HTTP请求到远程主机
    co_await http::async_write(stream, req);

    // 这个缓冲区用于读取，并且必须持久存在
    beast::flat_buffer b;

    // 声明一个容器来接收返回体
    http::response<http::dynamic_body> res;

    // 接受HTTP的返回
    co_await http::async_read(stream, b, res);

    // 把返回体写到标准输出
    std::cout << res << std::endl;

    // 优雅地关闭套接字
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    if (ec && ec != beast::errc::not_connected) {
        throw beast::system_error{ec, "shutdown"};
    }
}

void clientMain() {}
#endif