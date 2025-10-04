cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

include(version.cmake)
set(package_name depzip)
set(build_version ${depzip_version})

project(depzip VERSION ${build_version})

option(DEPZIP_INSTALL "Setup depzip install" ${PROJECT_IS_TOP_LEVEL})
option(DEPZIP_BUILD_TESTS "Build depzip tests" ${PROJECT_IS_TOP_LEVEL})

configure_file(Doxyfile.in Doxyfile @ONLY)

add_subdirectory(ext)
add_subdirectory(lib)
add_subdirectory(cli)

if(DEPZIP_INSTALL)
  message(STATUS "[${package_name}] setting up install")
  include(GNUInstallDirs)

  set(install_dst "${CMAKE_INSTALL_LIBDIR}/cmake/${package_name}")

  install(
    FILES ${package_name}-config.cmake
    DESTINATION "${install_dst}"
  )

  install(
    TARGETS ${package_name}-lib ${package_name}
    EXPORT ${package_name}-targets
    FILE_SET HEADERS
  )

  install(
    EXPORT ${package_name}-targets
    DESTINATION "${install_dst}"
    NAMESPACE ${package_name}::
    FILE ${package_name}-targets.cmake
  )
endif()

if(DEPZIP_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
  add_test(NAME depzip-cli-test
    COMMAND depzip -w=../workspace -v=verbose
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext"
  )
endif()
