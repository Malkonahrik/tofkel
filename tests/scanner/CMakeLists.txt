add_executable(test_directory test_directory_data.c)

target_link_libraries(test_directory PRIVATE scanner)

add_test(NAME directory_data_scan_empty_directory
    COMMAND test_directory scan_empty_directory)

add_test(NAME directory_data_scan_files
    COMMAND test_directory scan_files)

add_test(NAME directory_data_scan_subdirectories
    COMMAND test_directory scan_subdirectories)

add_test(NAME directory_data_scan_depth
    COMMAND test_directory scan_depth)

add_test(NAME directory_data_scan_invalid_path
    COMMAND test_directory scan_invalid_path)

add_test(NAME directory_data_scan_trailing_slash
    COMMAND test_directory scan_trailing_slash)
