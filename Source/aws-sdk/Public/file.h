// #include <aws/core/Aws.h>
// extern void InitS3Client();

#ifndef S3_UTILS_HPP
#define S3_UTILS_HPP

// #include <aws/core/Aws.h>
// #include <aws/s3/S3Client.h>

// #include <opencv4/opencv2/opencv.hpp>
#include <string>

// namespace s3_utils {
//     void initAwsAPI(bool shutdown = false);

//     class MinioClient {
//         std::shared_ptr<Aws::S3::S3Client> client_;

//       public:
//         MinioClient(const std::string &endpoint, const std::string &accessKey, const std::string &secretKey);

//         ~MinioClient() { client_.reset(); }

//         bool upload(const cv::Mat &img, const std::string &bucketName, const std::string &objectKey, const std::vector<int> &vecCompression_params = {});

//         bool upload(const std::string &contents, const std::string &bucketName, const std::string &objectKey);

//         std::string download(const std::string &bucketName, const std::string &objectKey);

//         cv::Mat downloadImg(const std::string &bucketName, const std::string &objectKey);
//     };
// }  // namespace s3_utils

#endif  // S3_UTILS_HPP
