#include <algorithm>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void fail(beast::error_code ec, char const* what) { std::cerr << what << ": " << ec.message() << "\n"; }

class webSocketSession : public boost::asio::coroutine, public std::enable_shared_from_this<webSocketSession> {
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;

  public:
    explicit webSocketSession(tcp::socket socket) : ws_(std::move(socket)) {}
    void run() { net::dispatch(ws_.get_executor(), beast::bind_front_handler(&webSocketSession::loop, shared_from_this(), beast::error_code{}, 0)); }
#include <boost/asio/yield.hpp>
    void loop(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        reenter(*this) {
            ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

            ws_.set_option(
                websocket::stream_base::decorator([](websocket::response_type& res) { res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-stackless"); }));
        }

        yield ws_.async_accept(std::bind(&webSocketSession::loop, shared_from_this(), std::placeholders::_1, 0));

        if (ec) return fail(ec, "accept");

        for (;;) {
        }
    }
};