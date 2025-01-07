file(GLOB_RECURSE FileServerFile "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(fileserver ${FileServerFile})

target_include_directories(fileserver PUBLIC Public)

# Util
target_link_libraries(fileserver PUBLIC Utils)

# libconfig
find_package(libconfig CONFIG REQUIRED)
target_link_libraries(fileserver PUBLIC libconfig::config libconfig::config++)

# CLI11
find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(fileserver PUBLIC CLI11::CLI11)

# spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(fileserver PUBLIC spdlog::spdlog)

# tinyorm
find_package(TinyOrm 0.38.1 CONFIG REQUIRED)
target_link_libraries(fileserver PUBLIC TinyOrm::TinyOrm)

# protobuf
include(FindProtobuf)
find_package(utf8_range REQUIRED)
target_link_libraries(fileserver PUBLIC ProtoObject)
target_link_libraries(fileserver PUBLIC protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
target_link_libraries(fileserver PUBLIC utf8_range::utf8_range utf8_range::utf8_validity)

# brpc
find_package(unofficial-brpc CONFIG REQUIRED)
if(${VCPKG_TARGET_TRIPLET} STREQUAL "x64-linux")
    target_link_libraries(fileserver PUBLIC unofficial::brpc::brpc-static)
elseif(${VCPKG_TARGET_TRIPLET} STREQUAL "x64-linux-dynamic")
    target_link_libraries(fileserver PUBLIC unofficial::brpc::brpc-shared)
endif()