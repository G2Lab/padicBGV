cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(PIR)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(helib EXACT REQUIRED HINTS /gpfs/commons/groups/gursoy_lab/jblindenbach/padics/mylibs)
add_subdirectory(src)

# add Tests
enable_testing()
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest not found. Installing GTest...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Add the necessary source files for the Encoder class
set(ENCODER_SOURCES
    src/encoder.cpp
)

# Add the necessary header files for the Encoder class
set(ENCODER_HEADERS
    src/encoder.hpp
)

# Add the Encoder source files to the project
add_library(encoder ${ENCODER_SOURCES} ${ENCODER_HEADERS})

# Link the HElib library to the encoder library
target_link_libraries(encoder PRIVATE helib)

# Link the encoder library to the tests executable
add_executable(tests ./src/test.cpp)
target_link_libraries(tests PRIVATE helib GTest::gtest_main encoder)
add_test(NAME tests COMMAND tests)
