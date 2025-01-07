// #include <aws/core/Aws.h>
// extern void InitS3Client();

#ifndef S3_UTILS_HPP
#define S3_UTILS_HPP

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

// #include <opencv4/opencv2/opencv.hpp>
#include <memory>
#include <string>
#include <vector>

namespace S3Utils {
    void InitAwsAPI(bool shutdown = false);

    class S3Client {
        std::shared_ptr<Aws::S3::S3Client> Client;

      public:
        S3Client(const std::string &endpoint, const std::string &accessKey, const std::string &secretKey);

        ~S3Client() {}

        void CreateBucket(const std::string &bucketName);

        // bool upload(const cv::Mat &img, const std::string &bucketName, const std::string &objectKey, const std::vector<int> &vecCompression_params = {});

        bool UploadFile(const std::string &contents, const std::string &bucketName, const std::string &objectKey);

        std::string download(const std::string &bucketName, const std::string &objectKey);

        // cv::Mat downloadImg(const std::string &bucketName, const std::string &objectKey);
    };
}  // namespace S3Utils

#endif  // S3_UTILS_HPP
