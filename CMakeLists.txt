cmake_minimum_required(VERSION 3.22)

project(forma-normal LANGUAGES CXX)

add_executable(${PROJECT_NAME} normal.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)