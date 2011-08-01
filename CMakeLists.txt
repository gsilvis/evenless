#required for all CMakeList files.
cmake_minimum_required(VERSION 2.6)

#project name
project(evenless)

set(CMAKE_C_FLAGS -Wall)
set(CMAKE_MAJOR_VERSION 0)
set(CMAKE_MINOR_VERSION 0)
set(CMAKE_PATCH_VERSION 3)


add_executable(evenless evenless.c)

target_link_libraries(evenless notmuch)

# gcc -o evenless evenless.c -lnotmuch
