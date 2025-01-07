#include "ConnectToSqlDb.h"

#include <orm/schema.hpp>

#include "FileServerMysqlDb.h"

void InitSql(const libconfig::Config* config, std::shared_ptr<Orm::DatabaseManager>& dbManager,
             QVariantHash& sqlConfig) {
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
    sqlConfig = {
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
    };

    dbManager = Orm::DB::create({
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

    if (!Orm::Schema::hasTable("file_operation_record")) {
        Orm::Schema::create("file_operation_record", [](Orm::SchemaNs::Blueprint& table) {
            table.id();
            table.softDeletes();
            table.timestamps();
            table.integer("s3_upload_id").index().defaultValue(0);
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
        Orm::Schema::table("file_operation_record", [](Orm::SchemaNs::Blueprint& table) {
            table.softDeletes().change();
            table.integer("s3_upload_id").defaultValue(0).change();
            table.string("file_name").change();
            table.string("file_sample_md5").change();
            table.tinyInteger("file_operation_type").change();
            table.tinyInteger("file_operation_status").change();
            table.tinyInteger("upload_file_server_type").defaultValue(0).change();
            table.tinyInteger("download_file_server_type").defaultValue(0).change();
            table.tinyInteger("is_permanent").defaultValue(0).change();
            table.tinyInteger("upload_method").defaultValue(0).change();
            table.tinyInteger("download_method").defaultValue(0).change();
        });
    }

    if (!Orm::Schema::hasTable("file_shared")) {
        Orm::Schema::create("file_shared", [](Orm::SchemaNs::Blueprint& table) {
            table.softDeletes();
            table.id();
            table.integer("s3_uploadordownload_id").index();
            table.bigInteger("file_operation_id").index();
            table.integer("shard_size").index();
            table.string("shard_md5").index();
            table.string("shard_s3_etag").index();
            table.integer("shard_index").index();
        });
    } else {
        Orm::Schema::table("file_shared", [](Orm::SchemaNs::Blueprint& table) {
            table.softDeletes().change();
            table.integer("s3_uploadordownload_id").change();
            table.bigInteger("file_operation_id").change();
            table.integer("shard_size").change();
            table.string("shard_md5").change();
            table.string("shard_s3_etag").change();
            table.integer("shard_index").change();
        });
    }
}