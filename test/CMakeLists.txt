# cmake的最小版本号，对应的还有max， 设置项目的cmake最低要求版本
cmake_minimum_required(VERSION 3.12) 

# 项目名称：倒没什么影响，但是cmake会默认设置一些变量，所以
project(gUnittest)

set(CMAKE_PREFIX_PATH /linux)

find_package(GTest REQUIRED)

include_directories(/linux/include)
include_directories(
/fileserver/BookServer/Public
/fileserver/gateway/Public 
/fileserver/Neo4j/Public 
/fileserver/ffmpeg/Public 
/fileserver/aws-sdk/Public
/fileserver/util/Public
/fileserver/User/Public
)
# 添加一个测试：名称和可执行文件
add_executable(mytests /fileserver/gateway/Private/client.cpp mytest.cpp) 


# 链接库文件，必须在上一句的后面，会有依赖关系的 
target_link_libraries(mytests GTest::GTest GTest::Main) 
find_package(CURL)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Boost COMPONENTS system url REQUIRED)
find_package(OpenSSL REQUIRED)
# abseil
find_package(absl REQUIRED)

find_library(LibConfig config /linux/lib)

target_link_libraries(mytests  CURL::libcurl odb odb-mysql)
target_link_libraries(mytests nlohmann_json::nlohmann_json)
target_link_libraries(mytests ${Boost_LIBRARIES})
target_link_libraries(mytests ${LibConfig})
target_link_libraries(mytests OpenSSL::SSL)
target_link_libraries(mytests OpenSSL::Crypto)
target_link_libraries(mytests absl::base)