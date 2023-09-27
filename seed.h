#ifndef SEED_H
#define SEED_H

#include <iostream>
#include <list>
#include <memory>
#include <string>

#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/tcp.hpp"
class seed {
  private:
    struct endpoint {
        std::string host_addr;
        uint32_t host_addr_binary;
        uint32_t host_port;
        endpoint(std::string addr, uint32_t addr_binary, uint32_t port)
            : host_addr(addr), host_addr_binary(addr_binary), host_port(port) {}
    };
    std::list<endpoint> endpoints;
    // 异步对象
    boost::asio::io_service io_service;
    void start_listen_loop();
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;

    // boost::public : void start();
};

#endif