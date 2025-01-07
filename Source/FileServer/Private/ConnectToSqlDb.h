#pragma once
#include <Utils/IncludeRequire/GlobalRequire.h>
void InitSql(const libconfig::Config* config, std::shared_ptr<Orm::DatabaseManager>& dbManager,
             QVariantHash& sqlConfig);