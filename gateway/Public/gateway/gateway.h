#ifndef GATEWAY_H
#define GATEWAY_H
enum class HttpMethod : unsigned char { GET, POST, PUT, DELETE, HEAD, OPTIONS, TRACE, CONNECT, PATCH, UNKNOWN };
#include <libconfig.h>
class gateway {
  public:
    gateway(config_t config) : config(config){};
    void initGateway();

  private:
    config_t config;
};
extern void clientMain();
#endif