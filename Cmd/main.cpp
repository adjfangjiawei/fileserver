
#include <FileServer.h>
#include <Utils/AwsSdkOption/S3Client.h>
#include <Utils/Log/SpdlogWrap.h>
#include <libconfig.h>

#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <libconfig.h++>

// 主函数
int main(int argc, char* argv[]) {
    InitSpdlog();
    S3Utils::InitAwsAPI();
    QCoreApplication qCoreApp(argc, argv);

    // 添加编译的配置文件的选项
    CLI::App cliApp{"a learning software"};
    std::string config_file;
    cliApp.add_option("-c,--config", config_file, "config file");
    CLI11_PARSE(cliApp, argc, argv);

    // 添加默认的配置文件
    if (config_file.empty()) {
        config_file = "Config/Config.cfg";
    }

    // 加载配置文件
    libconfig::Config cfg;
    try {
        cfg.readFile(config_file);
    } catch (const libconfig::ParseException& pex) {
        spdlog::error("Error reading config file: {}", pex.what());
        return 1;
    }

    FileServer::InitPkg(&cfg);
    S3Utils::InitAwsAPI(true);
    return 0;
}
