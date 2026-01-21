add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${PROJECT_NAME}::${PROJECT_NAME}-lib
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
)

target_sources(${PROJECT_NAME} PRIVATE
  src/app.cpp
  src/app.hpp
  src/main.cpp
)
