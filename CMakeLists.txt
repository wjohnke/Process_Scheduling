cmake_minimum_required(VERSION 2.8)
project(assignment3)

# ALL OF THE SPECIFIED NAMES IN THIS CMAKE ARE REQUIRED.
# Set the project name variable to assignment3.
# THIS IS REQUIRED

# Add your CMAKE_C_FLAGS AND CMAKE_CXX_FLAGS here
# Make SURE to use -std=c11 and -std=c++11 where relevant.
# The options specified below are required, but additional options
#  may be used.


# Add our include directory to CMake's search paths.
# THIS IS REQUIRED


# Compile dyn_array so we can use it.


# Compile the analysis executable.


# link the dyn_array library we compiled against our analysis executable.


# Compile the the tester executable.
add_executable(${PROJECT_NAME}_test )


# Link ${PROJECT_NAME}_test with dyn_array and gtest and pthread libraries
target_link_libraries(${PROJECT_NAME}_test gtest pthread dyn_array)
