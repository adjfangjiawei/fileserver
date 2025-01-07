cmake -S ./Source/ThirdParty/BookflavorInterface -B Build -G Ninja -DCMAKE_TOOLCHAIN_FILE="/fjw/vcpkg/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Debug
cmake --build ./Build -j 16