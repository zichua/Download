cmake_minimum_required (VERSION 3.20.0)

project (DOWN_01)

aux_source_directory (. DIR_SRCS)

add_executable (DOWN_01 ${DIR_SRCS})

target_link_libraries (DOWN_01 sll crypto pthread)