// protobuf
#include <FileServer.h>
#include <Proto/FileServer/Service.pb.h>
#include <Utils/IncludeRequire/GlobalRequire.h>

#include <memory>

#include "ConnectToS3Client.h"
#include "ConnectToSqlDb.h"
#include "FileServerMysqlDb.h"
#include "Utils/AwsSdkOption/S3Client.h"

namespace FileServer {

    struct FileServerMetadata {
        std::shared_ptr<S3Utils::S3Client> S3Client;
    };

    class FileServerService : public FileServer::FileService {
      public:
        FileServerService() {}
        virtual ~FileServerService() {}
        virtual void CreateFile(::google::protobuf::RpcController* controller, const ::FileServer::CreateFileRequest* request, ::FileServer::CreateFileResponse* response,
                                ::google::protobuf::Closure* done) override {
            brpc::ClosureGuard doneGuard(done);
            std::cout << "hello";
            response->set_name(request->name());
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