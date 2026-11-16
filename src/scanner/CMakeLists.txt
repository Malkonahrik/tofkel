add_library(scanner data.c directory_data.c)

target_include_directories(scanner PUBLIC ${PROJECT_SOURCE_DIR}/include)
