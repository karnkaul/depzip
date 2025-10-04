add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${PROJECT_NAME}::${PROJECT_NAME}-lib
  klib::klib
)

target_sources(${PROJECT_NAME} PRIVATE
  src/main.cpp
)
