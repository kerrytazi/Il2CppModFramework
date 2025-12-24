cmake_minimum_required(VERSION 3.13)
project(dll_injector)

add_executable(dll_injector src/main.cpp)

target_compile_features(dll_injector PRIVATE cxx_std_20)
