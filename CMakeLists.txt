project(findText)
cmake_minimum_required(VERSION 3.16)
set( LIBRARY_OUTPUT_PATH ../bin)


#set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin)
#set(CMAKE_CXX_FLAGS_RELEASE "-s -Wl,-gc-sections -mwindows")

add_link_options("-Wl,--large-address-aware")
add_library(test MODULE src/main.cc src/tabbar.cc src/util.cc src/hookproc.cc)
target_link_libraries(test -lexshit)
target_link_options(test PRIVATE -nostartfiles)
