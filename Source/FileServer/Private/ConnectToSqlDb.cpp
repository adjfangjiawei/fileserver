#include "ConnectToSqlDb.h"

#include <orm/schema.hpp>

#include "FileServerMysqlDb.h"
void InitSql(const libconfig::Config* config) {
    std::string mysqlHost;
    if (!config->lookupValue("mysql.host", mysqlHost)) {
        SPDLOG_CRITICAL("mysql.host not found in config file");
    }
    std::string mysqlPort;
    if (!config->lookupValue("mysql.port", mysqlPort)) {
        SPDLOG_CRITICAL("mysql.port not found in config file");
    }
    std::string mysqlUser;
    if (!config->lookupValue("mysql.username", mysqlUser)) {
        SPDLOG_CRITICAL("mysql.user not found in config file");
    }
    std::string mysqlPassword;
    if (!config->lookupValue("mysql.password", mysqlPassword)) {
        SPDLOG_CRITICAL("mysql.password not found in config file");
    }
    std::string mysqlDatabase;
    if (!config->lookupValue("mysql.database", mysqlDatabase)) {
        SPDLOG_CRITICAL("mysql.database not found in config file");
    }

    auto manager = Orm::DB::create({
        {"driver", "QMYSQL"},
        {"host", mysqlHost.c_str()},
        {"port", mysqlPort.c_str()},
        {"username", mysqlUser.c_str()},
        {"password", mysqlPassword.c_str()},
        {"database", mysqlDatabase.c_str()},
        {"charset", qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
        {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_0900_ai_ci")},
        {"timezone", "+00:00"},
        {"qt_timezone", QVariant::fromValue(QTimeZone::UTC)},
        {"prefix", ""},
        {"prefix_indexes", false},
        {"strict", true},
        {"engine", "InnoDB"},
        {"options", QVariantHash()},
    });

    if (!Orm::Schema::hasTable("user")) {
        Orm::Schema::create("user", [](Orm::SchemaNs::Blueprint& table) {
            table.id();
            table.string("name");
            table.string("email");
            table.timestamps();
        });
    }

    if (!Orm::Schema::hasTable("flights")) {
        Orm::Schema::create("flights", [](Orm::SchemaNs::Blueprint& table) {
            table.id();
            table.softDeletes();
            table.timestamps();
        });
    }

    if (!Orm::Schema::hasTable("flightsaa")) {
        Orm::Schema::create("flightsaa", [](Orm::SchemaNs::Blueprint& table) {
            table.id();
            table.softDeletes();
            table.timestamps();
        });
    }

    if (!Orm::Schema::hasTable("file_operation_record")) {
        Orm::Schema::create("file_operation_record", [](Orm::SchemaNs::Blueprint& table) {
            table.id();
            table.softDeletes();
            table.timestamps();
            table.integer("s3_upload_id").index();
            table.string("file_name");
            table.string("file_path");
            table.string("file_sample_md5");
            table.tinyInteger("file_operation_type");
            table.tinyInteger("file_operation_status");
            table.tinyInteger("upload_file_server_type");
            table.tinyInteger("download_file_server_type");
            table.tinyInteger("is_permanent");
            table.tinyInteger("upload_method");
            table.tinyInteger("download_method");
        });
    } else {
        Orm::Schema::table("file_operation_record", [](Orm::SchemaNs::Blueprint& table) { auto columns = table.getColumns(); });
    }
}