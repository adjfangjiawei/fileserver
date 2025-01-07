#pragma once
#include <Utils/IncludeRequire/GlobalRequire.h>
inline std::string GetConnection(std::shared_ptr<Orm::DatabaseManager> dbManager, QVariantHash sqlConfig) {
    std::mutex mu;
    std::unique_lock lock(mu);
    auto connectionName = std::to_string(gettid()) + "test"s;
    try {
        dbManager->originalConfig(connectionName.c_str());
    } catch (Orm::Exceptions::InvalidArgumentError& e) {
        dbManager->addConnection(sqlConfig, connectionName.c_str());
    }

    lock.unlock();
    return connectionName;
}