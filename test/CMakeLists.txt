# cmake的最小版本号，对应的还有max， 设置项目的cmake最低要求版本
cmake_minimum_required(VERSION 3.12) 

# 项目名称：倒没什么影响，但是cmake会默认设置一些变量，所以
project(gUnittest)

set(CMAKE_PREFIX_PATH /linux)

find_package(GTest REQUIRED)

include_directories(/linux/include)
# 添加一个测试：名称和可执行文件
add_executable(mytests mytest.cpp) 


# 链接库文件，必须在上一句的后面，会有依赖关系的 
target_link_libraries(mytests PRIVATE GTest::GTest GTest::Main) 