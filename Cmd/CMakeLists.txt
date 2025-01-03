file(GLOB_RECURSE CmdFile "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(fileserver ${CmdFile})
target_link_libraries(fileserver PRIVATE Utils)

find_package(libconfig CONFIG REQUIRED)
target_link_libraries(fileserver PRIVATE libconfig::config libconfig::config++)

find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(fileserver PRIVATE CLI11::CLI11)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(fileserver PRIVATE spdlog::spdlog)