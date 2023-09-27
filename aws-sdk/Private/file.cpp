
#include "../Public/file.h"

#include <aws/core/utils/memory/stl/AWSStreamFwd.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <fstream>
#include <iostream>
#include <memory>
void InitS3Client(Aws::Utils::Logging::LogLevel logLevel) {
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = logLevel;
    // options.httpOptions.httpClientFactory_create_fn = []() {
    //     return Aws::MakeShared<Mycu>
    // };
    options.loggingOptions.crt_logger_create_fn = []() { return Aws::MakeShared<Aws::Utils::Logging::DefaultCRTLogSystem>("CRTLogSystem", Aws::Utils::Logging::LogLevel::Warn); };
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.proxyHost = "localhost";
    clientConfig.proxyPort = 1234;
    clientConfig.enableTcpKeepAlive = true;

    Aws::S3::S3Client{clientConfig};
}

// A mutex is a synchronization primitive that can be used to protect shared
// data from being simultaneously accessed by multiple threads.
std::mutex upload_mutex;
// A condition_variable is synchronization primitive that can be used to bloack
// a thread, or to block multiple threads at the same time.
// The thread is blocaked until another thread both modifies a shared
// variable(the condition) and notifies the condition_variable.
std::condition_variable upload_variable;

void PutObjectAsyncFinished(const Aws::S3::S3Client *s3Client, const Aws::S3::Model::PutObjectRequest &request, const Aws::S3::Model::PutObjectOutcome &outcome,
                            const std::shared_ptr<const Aws::Client::AsyncCallerContext> &context) {
    if (outcome.IsSuccess()) {
        std::cout << "Success: PutObjectAsyncFinished: Finished uploading " << context->GetUUID() << std::endl;
    } else {
        std::cerr << "Error: PutObjectAsyncFinished: " << outcome.GetError().GetMessage() << std::endl;
    }

    // Unblock the thread that is waiting for this function to complete.
    upload_variable.notify_one();
}

bool PutObjectAsync(const Aws::S3::S3Client &s3Client, const Aws::String &bucketName, const Aws::String &fileName) {
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(bucketName);
    request.SetKey(fileName);
    const std::shared_ptr<Aws::IOStream> input_data = Aws::MakeShared<Aws::FStream>("SampleAllocationTag", fileName.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!*input_data) {
        std::cout << "Failed to open file " << fileName << std::endl;
        return false;
    }
    request.SetBody(input_data);

    // Create and  configure the context for the asynchronous put object
    // request.
    std::shared_ptr<Aws::Client::AsyncCallerContext> context = Aws::MakeShared<Aws::Client::AsyncCallerContext>("PutObjectAllocationTag");
    context->SetUUID(fileName);

    // Make the asynchronous put object call. Queue the request into a
    // thread executor and call the  PutObjectAsyncFinished function when
    // the operation has finished.
    s3Client.PutObjectAsync(request, PutObjectAsyncFinished, context);
    return true;
}

void S3Main() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        // TODO(user): Change bucket_name to the name of a bucket in your
        // account.
        const Aws::String bucket_name = "my-bucket";
        // TODO(user)::Create a file called "my-file.txt" in the local folder
        // where your executables are built to.
        const Aws::String object_name = "my-file.txt";

        // A unique_lock is a general-purpose mutex ownership wrapper allowing
        // deferred locking,time-constrained attempts at locking,recursive
        // locking,transfer of lock ownership, and use with condition variables.
        std::unique_lock<std::mutex> lock(upload_mutex);

        // Create and configure the Amazon S3 client.
        // This client must be declared here, as this client must exist
        // until the push object operation finishes.
        Aws::Client::ClientConfiguration config;
        config.region = "us-east-8";
        Aws::S3::S3Client s3_client(config);
        PutObjectAsync(s3_client, bucket_name, object_name);
        std::cout << "main: Waiting for file upload attempt..." << std::endl << std::endl;

        // While the put object operation attempt is in progress,
        // you can perform other tasks.
        // This example simply blocks until the put object operation attemp
        // finishes.
        upload_variable.wait(lock);
        std::cout << std::endl << "main: File upload attempt finished." << std::endl;
    }
    Aws::ShutdownAPI(options);
}

#include <aws/monitoring/CloudWatchClient.h>
#include <aws/monitoring/model/PutMetricAlarmRequest.h>
void CreateAlarm() {
    Aws::CloudWatch::CloudWatchClient cw;
    Aws::CloudWatch::Model::PutMetricAlarmRequest request;
    request.SetAlarmName("CPUAlarm");
    request.SetComparisonOperator(Aws::CloudWatch::Model::ComparisonOperator::GreaterThanThreshold);
    request.SetEvaluationPeriods(1);
    request.SetMetricName("CPUUtilization");
    request.SetNamespace("AWS/EC2");
    request.SetPeriod(60);
    request.SetStatistic(Aws::CloudWatch::Model::Statistic::Average);
    request.SetThreshold(70.0);
    request.SetActionsEnabled(false);
    request.SetAlarmDescription("Alarm when server CPU exceeds 70%");
    request.SetUnit(Aws::CloudWatch::Model::StandardUnit::Seconds);

    Aws::CloudWatch::Model::Dimension dimension;
    dimension.SetName("InstanceId");
    dimension.SetValue("1");
    request.AddDimensions(dimension);
    auto outCome = cw.PutMetricAlarm(request);
    if (!outCome.IsSuccess()) {
        std::cout << "Failed to create CloudWatch alarm:" << outCome.GetError().GetMessage() << std::endl;
    } else {
        std::cout << "Successfully created CloudWatch alarm" << std::endl;
    }
}

#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/CreateTableRequest.h>
bool createTable(const Aws::String &tableName, const Aws::String &primaryKey, const Aws::Client::ClientConfiguration &clientConfig) {
    Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);
    std::cout << "Creating table " << tableName << "with a simple primary key:\"" << primaryKey << "\"." << std::endl;
    Aws::DynamoDB::Model::CreateTableRequest request;
    Aws::DynamoDB::Model::AttributeDefinition hashKey;
    hashKey.SetAttributeName(primaryKey);
    hashKey.SetAttributeType(Aws::DynamoDB::Model::ScalarAttributeType::S);
    request.AddAttributeDefinitions(hashKey);
    Aws::DynamoDB::Model::KeySchemaElement keySchemaElement;
    keySchemaElement.WithAttributeName(primaryKey).WithKeyType(Aws::DynamoDB::Model::KeyType::HASH);
    request.AddKeySchema(keySchemaElement);
    Aws::DynamoDB::Model::ProvisionedThroughput provisionedThroughput;
}