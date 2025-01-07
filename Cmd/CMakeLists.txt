
include(FindProtobuf)

# moc
set(CMAKE_AUTOMOC ON)

# target
file(GLOB_RECURSE CmdFile "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(fileservercmd ${CmdFile})

target_link_libraries(fileservercmd PUBLIC fileserver)

