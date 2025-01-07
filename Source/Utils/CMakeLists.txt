
file(GLOB_RECURSE UtilsSourceFile "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp")
message("UtilsSourceFile: ${UtilsSourceFile}")
add_library(Utils ${UtilsSourceFile})
target_include_directories(Utils PUBLIC Public)

find_package(Boost COMPONENTS system url REQUIRED)
target_link_libraries(Utils PRIVATE Boost::system Boost::url)

# aws-sdk
find_package(AWSSDK CONFIG REQUIRED COMPONENTS core s3)
target_link_libraries(Utils PRIVATE ${AWSSDK_LIBRARIES})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(Utils PRIVATE spdlog::spdlog)

# 连接Qt
# find_package(Qt REQUIRED)
# target_link_libraries(Utils PRIVATE Qt::Core Qt::Network Qt::Widgets)