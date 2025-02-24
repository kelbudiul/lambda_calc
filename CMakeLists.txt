cmake_minimum_required(VERSION 3.10)
project(LambdaCalculus VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(lambda_calculus
    main.cpp
    Expression.cpp
    Evaluator.cpp
    Parser.cpp
)

# Include directories
target_include_directories(lambda_calculus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Enable warnings
if(MSVC)
    target_compile_options(lambda_calculus PRIVATE /W4)
else()
    target_compile_options(lambda_calculus PRIVATE -Wall -Wextra -pedantic)
endif()