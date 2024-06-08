aux_source_directory(. sources)



add_executable(sample ${sources})
target_link_libraries(sample reader)