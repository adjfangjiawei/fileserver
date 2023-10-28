cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_TOOLCHAIN_FILE /mnt/m/fileservervcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake)
# set(CMAKE_FIND_DEBUG_MODE FALSE)
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
/fileserver/thirdparty
/linux/include
# /fileserverinclude
)
aux_source_directory(util/Private util)
aux_source_directory(BookServer/Private bookserver)
aux_source_directory(ffmpeg/Private ffmpegServer)
aux_source_directory(. mainsource)
add_executable(main  
${bookserver}  
cmd/main.cpp 
gateway/Private/gateway.cpp
${util}
aws-sdk/Private/file.cpp
${ffmpegServer}
)



target_sources(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/BookServer/Public/BookServer/db/book-odb.cxx)


find_package(CURL)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Boost COMPONENTS system url REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(fmt CONFIG REQUIRED)
set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-linux/share/opencv4")
find_package(OpenCV REQUIRED)
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
# target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS})
# target_link_libraries(main ${OpenCV_LIBRARIES})
target_link_libraries(main  CURL::libcurl odb odb-mysql)
target_link_libraries(main nlohmann_json::nlohmann_json)
target_link_libraries(main ${Boost_LIBRARIES})
target_link_libraries(main ${LibConfig})
# target_link_libraries(main OpenSSL::SSL)
# target_link_libraries(main OpenSSL::Crypto)
# target_link_libraries(main ${AWSSDK_LINK_LIBRARIES})
target_link_libraries(main fmt::fmt)
target_link_libraries(main ${OpenCV_LIBRARIES})

target_include_directories(main PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(main ${FFMPEG_LIBRARIES})
target_link_directories(main PRIVATE ${FFMPEG_LIBRARY_DIRS})

# target_link_libraries(main absl::base)