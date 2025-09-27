cmake_minimum_required(VERSION 3.23)

cmake_policy(VERSION 3.23)

set(build_version 0.1.0)
project(depzip VERSION ${build_version})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

add_subdirectory(ext)
add_subdirectory(lib)
add_subdirectory(cli)
