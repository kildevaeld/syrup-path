cmake_minimum_required(VERSION 3.0)
project(syrup-buffer)

option(SYRUP_BUILD_TEST "build tests" off)

include_directories(include)

add_subdirectory(src)

if (SYRUP_BUILD_TEST)
    include(CTest)
    add_subdirectory(vendor/unity)
    add_subdirectory(test)
endif(SYRUP_BUILD_TEST)