include(CMakeFindDependencyMacro)

find_dependency(djson)
find_dependency(klib)

include("${CMAKE_CURRENT_LIST_DIR}/depzip-targets.cmake")
