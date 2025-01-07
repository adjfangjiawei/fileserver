#include "ConnectToS3Client.h"

#include <Utils/IncludeRequire/GlobalRequire.h>

#include "Utils/AwsSdkOption/S3Client.h"

void InitS3Client(const libconfig::Config* config, std::shared_ptr<S3Utils::S3Client>& s3Client) {
    std::string url;
    if (!config->lookupValue("minio.url", url)) {
        spdlog::critical("s3.url not found in config file");
    }
    std::string accessKey;
    if (!config->lookupValue("minio.accessKey", accessKey)) {
        spdlog::critical("s3.accessKey not found in config file");
    }
    std::string secretKey;
    if (!config->lookupValue("minio.secretKey", secretKey)) {
        spdlog::critical("s3.secretKey not found in config file");
    }

    s3Client = std::make_shared<S3Utils::S3Client>(url, accessKey, secretKey);
}