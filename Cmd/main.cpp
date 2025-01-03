#include <Utils/AwsSdkOption/file.h>
#include <libconfig.h>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <libconfig.h++>

// 主函数
int main(int argc, char* argv[]) {
    using namespace std;

    spdlog::set_pattern("%^[%n][%L] %!:%# -> %v%$");
    spdlog::set_level(spdlog::level::trace);
    auto logger = spdlog::stdout_color_mt("console");
    // 只有在 SPDLOG_ACTIVE_LEVEL >= SPDLOG_LEVEL_TRACE 时，SPDLOG_TRACE 才会生效
    SPDLOG_TRACE("这是 TRACE 级别日志，包含函数名和行号");
    SPDLOG_DEBUG("这是 DEBUG 级别日志，同样包含函数名和行号");
    SPDLOG_INFO("这是 INFO 日志，也会受上面 set_pattern 的影响打印出相应格式");

    // // s3_utils::initAwsAPI();
    // ffmpeg_push_stream();

    // // 添加编译的配置文件的选项
    // CLI::App app{"a learning software"};
    // std::string config_file;
    // app.add_option("-c,--config", config_file, "config file");
    // CLI11_PARSE(app, argc, argv);

    // // 添加默认的配置文件
    // if (config_file.empty()) {
    //     config_file = "/fileserver/config/config.cfg";
    // }
    // config_t cfg;
    // config_setting_t* setting;
    // config_init(&cfg);
    // if (config_read_file(&cfg, config_file.c_str()) != CONFIG_TRUE) {
    //     config_destroy(&cfg);
    //     return 1;
    // }
    // auto gateWay = std::make_shared<gateway>(cfg);
    // gateWay->initGateway();
    // config_destroy(&cfg);

    return 0;
}