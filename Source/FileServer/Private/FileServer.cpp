// protobuf
#include <FileServer.h>
#include <Proto/FileServer/Service.pb.h>
#include <Utils/IncludeRequire/GlobalRequire.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <memory>
#include <mutex>
#include <orm/exceptions/runtimeerror.hpp>
#include <orm/exceptions/sqlerror.hpp>
#include <string>
#include <thread>

#include "ConnectToS3Client.h"
#include "ConnectToSqlDb.h"
#include "FileServerMysqlDb.h"
#include "Proto/FileServer/FileOperation.pb.h"
#include "Utils/AwsSdkOption/S3Client.h"
#include "Utils/Sql/TinyOrmHelperFunction.h"

namespace FileServer {

    struct FileServerMetadata {
        std::shared_ptr<S3Utils::S3Client> S3Client = nullptr;
        std::shared_ptr<Orm::DatabaseManager> dbManager = nullptr;
        QVariantHash sqlConfig;
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

            for (auto fileOperation : request->file_operations()) {
                std::string connectionName = GetConnection(metadata.dbManager, metadata.sqlConfig);
                FileOperationRecord dbFileOperationRecord = FileOperationRecord::instance(connectionName.c_str());
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

                try {
                    dbFileOperationRecord.save();
                } catch (Orm::Exceptions::InvalidArgumentError& e) {
                    spdlog::error("Failed to save file operation record: {}", e.what());

                } catch (Orm::Exceptions::SqlError& e) {
                    spdlog::error("Failed to save file operation record: {}", e.what());
                }
            }
        }

        virtual void CreateFileShards(::google::protobuf::RpcController* controller,
                                      const ::FileServer::CreateFileShardsRequest* request,
                                      ::FileServer::CreateFileShardsResponse* response,
                                      ::google::protobuf::Closure* done) override {
            brpc::ClosureGuard doneGuard(done);
            spdlog::info("CreateFileOperations called");

            for (auto fileShard : request->file_shards()) {
                if (fileShard.shard_md5() == "") {
                    spdlog::error("Shard md5 is empty");
                    return;
                }
                if (fileShard.shard_size() == 0) {
                    spdlog::error("Shard size is zero");
                    return;
                }
                if (fileShard.file_operation_id() == 0) {
                    spdlog::error("File operation id is zero");
                    return;
                }
                if (fileShard.s3_uploadordownload_id() == 0) {
                    spdlog::error("S3 upload id is zero");
                    return;
                }
            }
        }

        FileServerMetadata metadata;
    };

    void InitRpcServer(const libconfig::Config* config, std::shared_ptr<Orm::DatabaseManager>& dbManager,
                       QVariantHash& sqlConfig) {
        brpc::Server server;
        FileServerService service;
        service.metadata.dbManager = dbManager;
        service.metadata.sqlConfig = sqlConfig;
        InitS3Client(config, service.metadata.S3Client);
        server.AddService(&service, brpc::SERVER_DOESNT_OWN_SERVICE);
        brpc::ServerOptions options;
        options.idle_timeout_sec = 1000;
        server.Start(8021, &options);
        server.RunUntilAskedToQuit();
    }

    void InitPkg(const libconfig::Config* config) {
        std::shared_ptr<Orm::DatabaseManager> dbManager;
        QVariantHash sqlConfig;
        InitSql(config, dbManager, sqlConfig);
        InitRpcServer(config, dbManager, sqlConfig);
    }
}  // namespace FileServer