# cmake_minimum_required(VERSION 3.0) #最低版本号

# project(DowmLoad) #工程名称

# aux_source_directory (. DIR_SRCS)　#构建源文件列表

# add_executable(Download ${DIR_SRCS})　#生成可执行文件

# target_link_libraries(Download ssl crypto pthread) #为target添加需要链接的共享库

cmake_minimum_required (VERSION 3.0)

project (DownLoad)

aux_source_directory(. DIR_SRCS)

add_executable(Download ${DIR_SRCS})

target_link_libraries(Download ssl crypto pthread)