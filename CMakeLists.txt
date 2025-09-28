cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

set(build_version 0.1.3)
project(depzip VERSION ${build_version})

option("DEPZIP_BUILD_TESTS" "Build depzip tests" ${PROJECT_IS_TOP_LEVEL})

add_subdirectory(ext)
add_subdirectory(lib)
add_subdirectory(cli)

if(DEPZIP_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
  add_test(NAME depzip-cli-test
    COMMAND depzip -w=../workspace -v=verbose
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext"
  )
endif()
