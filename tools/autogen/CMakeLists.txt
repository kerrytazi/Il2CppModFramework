cmake_minimum_required(VERSION 3.13)
project("autogen")

include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/config_mini.cmake")

config_mini(autogen EXECUTABLE "main.cpp" "${CMAKE_CURRENT_LIST_DIR}/../..")
