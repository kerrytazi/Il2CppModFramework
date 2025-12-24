cmake_minimum_required(VERSION 3.13)
project("StackTracer")

include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/config_common.cmake")

config_common(StackTracer STATIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_LIST_DIR}/../..")
