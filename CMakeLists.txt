cmake_minimum_required(VERSION 3.12.0)
project(proxysever)
include(cmake/CPM.cmake)
set(CMAKE_PREFIX_PATH /linux)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
add_compile_options(-Wall -Wextra -Wunused-parameter)
set_target_properties(${TARGET} PROPERTIES
COMPILE_FLAGS "-pthread"
LINK_FLAGS "-pthread")
# CPMAddPackage("gh:google/libphonenumber@8.13.2")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20   -fsanitize=address -fpermissive -w")
include_directories(
/fileserver/BookServer/Public
/fileserver/gateway/Public 
/fileserver/Neo4j/Public 
/fileserver/ffmpeg/Public 
/fileserver/aws-sdk/Public
/fileserver/util/Public
/fileserver/User/Public
)
aux_source_directory(util/Private util)
aux_source_directory(BookServer/Private bookserver)
aux_source_directory(. mainsource)
add_executable(main  
${bookserver}  
cmd/main.cpp 
gateway/Private/gateway.cpp
${util}
)
target_sources(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/BookServer/Public/BookServer/db/book-odb.cxx)


find_package(CURL)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Boost COMPONENTS system url REQUIRED)
find_package(OpenSSL REQUIRED)
# abseil
# find_package(absl REQUIRED)

find_library(LibConfig config /linux/lib)

# target_include_directories(
# main
# PRIVATE
# /fileserver/BookServer/Public
# /fileserver/gateway/Public 
# /fileserver/Neo4j/Public 
# /fileserver/ffmpeg/Public 
# /fileserver/aws-sdk/Public
# )
target_link_libraries(main  CURL::libcurl odb odb-mysql)
target_link_libraries(main nlohmann_json::nlohmann_json)
target_link_libraries(main ${Boost_LIBRARIES})
target_link_libraries(main ${LibConfig})
target_link_libraries(main OpenSSL::SSL)
target_link_libraries(main OpenSSL::Crypto)
# target_link_libraries(main absl::base)