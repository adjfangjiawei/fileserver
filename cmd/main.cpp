#include <BookServer/book.h>
#include <gateway/gateway.h>
#include <libconfig.h>
#include <spdlog/spdlog.h>

#include <BookServer/jb/book.hxx>
#include <CLI/CLI.hpp>
#include <libconfig.h++>
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <odb/transaction.hxx>

using database = std::shared_ptr<odb::mysql::database>;
using transaction = odb::core::transaction;
// 主函数
int main(int argc, char *argv[]) {
    using namespace std;
    JbBook book;
    database db;
    CreateBook(db, &book);
    // 添加编译的配置文件的选项
    CLI::App app{"a learning software"};
    std::string config_file;
    app.add_option("-c,--config", config_file, "config file");
    CLI11_PARSE(app, argc, argv);

    // 添加默认的配置文件
    if (config_file.empty()) {
        config_file = "/fileserver/config/config.cfg";
    }
    config_t cfg;
    config_setting_t *setting;
    config_init(&cfg);
    if (config_read_file(&cfg, config_file.c_str()) != CONFIG_TRUE) {
        spdlog::error("config file read error");
        config_destroy(&cfg);
        return 1;
    }

    auto gateWay = std::make_shared<gateway>(cfg);
    gateWay->initGateway();
    config_destroy(&cfg);

    return 0;
}