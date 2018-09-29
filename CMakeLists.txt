cmake_minimum_required(VERSION 2.8)
project(assignment3)

# ALL OF THE SPECIFIED NAMES IN THIS CMAKE ARE REQUIRED.
# Set the project name variable to assignment3.
# THIS IS REQUIRED


# Add your CMAKE_C_FLAGS AND CMAKE_CXX_FLAGS here
# Make SURE to use -std=c11 and -std=c++11 where relevant.
# The options specified below are required, but additional options
#  may be used.

set(CMAKE_C_FLAGS "-std=c11 -Werror  -Wall -Wextra")
set(CMAKE_CXX_FLAGS "-std=c++11 -Werror -Wall -Wextra")


# Add our include directory to CMake's search paths.
# THIS IS REQUIRED

include_directories(include)

# Compile dyn_array so we can use it.

#add_library(dyn_array SHARED src/dyn_array.c)
add_library(dyn_array src/dyn_array.c src/process_scheduling.c)

# Compile the analysis executable.

#add_executable(analysis src/analysis.c src/process_scheduling.c)
add_executable(analysis src/analysis.c)

# link the dyn_array library we compiled against our analysis executable.

target_link_libraries(analysis dyn_array)

# Compile the the tester executable.
add_executable(${PROJECT_NAME}_test ${CMAKE_CURRENT_SOURCE_DIR}/test/tests.cpp src/process_scheduling.c)


# Link ${PROJECT_NAME}_test with dyn_array and gtest and pthread libraries
target_link_libraries(${PROJECT_NAME}_test gtest pthread dyn_array)
add_test (NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

