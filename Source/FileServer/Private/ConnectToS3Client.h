
#include <libconfig.h++>

#include "Utils/AwsSdkOption/S3Client.h"
void InitS3Client(const libconfig::Config *config, std::shared_ptr<S3Utils::S3Client> &s3Client);