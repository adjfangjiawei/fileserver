// protobuf
#include <FileServer.h>
#include <Proto/FileServer/Service.pb.h>
#include <Utils/IncludeRequire/GlobalRequire.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "ConnectToS3Client.h"
#include "ConnectToSqlDb.h"
#include "FileServerMysqlDb.h"
#include "Proto/FileServer/FileOperation.pb.h"
#include "Utils/AwsSdkOption/S3Client.h"

namespace FileServer {

    struct FileServerMetadata {
        std::shared_ptr<S3Utils::S3Client> S3Client;
    };

    class FileServerService : public FileServer::FileService {
      public:
        FileServerService() {}
        virtual ~FileServerService() {}
        virtual void CreateFileOperations(::google::protobuf::RpcController* controller,
                                          const ::FileServer::CreateFileOperationsRequest* request,
                                          ::FileServer::CreateFileOperationsResponse* response,
                                          ::google::protobuf::Closure* done) override {
            brpc::ClosureGuard doneGuard(done);
            spdlog::info("CreateFileOperations called");
            FileOperationRecord dbFileOperationRecord;

            for (auto fileOperation : request->file_operations()) {
                if (fileOperation.file_operation_type() ==
                    ::FileServer::FileOperation::FileOperationType::
                        FileOperation_FileOperationType_FILE_OPERATION_TYPE_UNSPECIFIED) {
                    spdlog::error("FileOperationType is unspecified");
                    return;
                }
                if (fileOperation.file_name() == "") {
                    spdlog::error("File name is empty");
                    return;
                }
                if (fileOperation.file_sample_md5() == "") {
                    spdlog::error("File sample md5 is empty");
                    return;
                }
                if (fileOperation.file_operation_type() ==
                    ::FileServer::FileOperation::FileOperationType::
                        FileOperation_FileOperationType_FILE_OPERATION_TYPE_DOWNLOAD_FILE) {
                    if (!fileOperation.has_download_file_policy()) {
                        spdlog::error("download file do not has a download file policy");
                        return;
                    }
                    if (fileOperation.download_file_policy().file_server_type() ==
                            ::FileServer::FileOperation_UploadFilePolicy::FILE_SERVER_TYPE_MINIO &&
                        fileOperation.minio_key() == "") {
                        spdlog::error("minio key is empty");
                        return;
                    }
                } else if (fileOperation.file_operation_type() ==
                           ::FileServer::FileOperation::FileOperationType::
                               FileOperation_FileOperationType_FILE_OPERATION_TYPE_UPLOAD_FILE) {
                    if (!fileOperation.has_upload_file_policy()) {
                        spdlog::error("upload file do not has a upload file policy");
                        return;
                    }
                    if (fileOperation.upload_file_policy().file_server_type() ==
                            ::FileServer::FileOperation_UploadFilePolicy::FILE_SERVER_TYPE_MINIO &&
                        fileOperation.minio_key() == "") {
                        spdlog::error("minio key is empty");
                        return;
                    }
                }
                fileOperation.set_operation_status(
                    ::FileServer::FileOperation::FileOperationStatus::
                        FileOperation_FileOperationStatus_FILE_OPERATION_STATUS_IN_PROGRESS);
                dbFileOperationRecord.setAttribute("file_operation_type", fileOperation.file_operation_type());
                dbFileOperationRecord.setAttribute("file_name", fileOperation.file_name().c_str());
                dbFileOperationRecord.setAttribute("file_sample_md5", fileOperation.file_sample_md5().c_str());
                dbFileOperationRecord.setAttribute("file_operation_status", fileOperation.operation_status());
                if (fileOperation.has_upload_file_policy()) {
                    dbFileOperationRecord.setAttribute("upload_file_server_type",
                                                       fileOperation.upload_file_policy().file_server_type());
                    dbFileOperationRecord.setAttribute("is_permanent",
                                                       fileOperation.upload_file_policy().is_permanent());
                    auto newUploadPolicy = fileOperation.upload_file_policy();
                    if (newUploadPolicy.upload_method() ==
                        ::FileServer::FileOperation_UploadFilePolicy::UPLOAD_METHOD_UNSPECIFIED) {
                        newUploadPolicy.set_upload_method(
                            ::FileServer::FileOperation_UploadFilePolicy::UPLOAD_METHOD_FULL);
                    }
                    dbFileOperationRecord.setAttribute("upload_method", newUploadPolicy.upload_method());
                } else if (fileOperation.has_download_file_policy()) {
                    dbFileOperationRecord.setAttribute("download_file_server_type",
                                                       fileOperation.download_file_policy().file_server_type());
                    dbFileOperationRecord.setAttribute("is_permanent",
                                                       fileOperation.download_file_policy().is_permanent());

                    auto newDownloadPolicy = fileOperation.download_file_policy();
                    if (newDownloadPolicy.download_method() ==
                        ::FileServer::FileOperation_DownloadFilePolicy::DOWNLOAD_METHOD_UNSPECIFIED) {
                        newDownloadPolicy.set_download_method(
                            ::FileServer::FileOperation_DownloadFilePolicy::DOWNLOAD_METHOD_FULL);
                    }
                    dbFileOperationRecord.setAttribute("download_method", newDownloadPolicy.download_method());
                }
                dbFileOperationRecord.save();
            }
        }

        FileServerMetadata metadata;
    };

    void InitRpcServer(const libconfig::Config* config) {
        brpc::Server server;
        FileServerService service;
        InitS3Client(config, service.metadata.S3Client);
        server.AddService(&service, brpc::SERVER_DOESNT_OWN_SERVICE);
        brpc::ServerOptions options;
        options.idle_timeout_sec = 1000;
        server.Start(8021, &options);
        server.RunUntilAskedToQuit();
    }

    void InitPkg(const libconfig::Config* config) {
        InitSql(config);
        InitRpcServer(config);
    }
}  // namespace FileServer