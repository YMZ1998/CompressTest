cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(CompressTest)

find_package(ZLIB REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(Snappy CONFIG REQUIRED)

add_executable(compress_test main.cpp)

target_link_libraries(compress_test PRIVATE ZLIB::ZLIB zstd::libzstd Snappy::snappy)
