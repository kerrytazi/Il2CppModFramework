cmake_minimum_required(VERSION 3.13)
project("GameAssembly")

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/config_common.cmake")

config_common(GameAssembly STATIC "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_LIST_DIR}/..")
target_include_directories(GameAssembly PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")

config_mini(GameAssembly_optimized STATIC "${CMAKE_CURRENT_LIST_DIR}/optimized/update_patch.cpp" "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(GameAssembly GameAssembly_optimized)

get_target_property(GameAssembly_optimized_options GameAssembly_optimized COMPILE_OPTIONS)
string(REPLACE "/Ob0;/Od;/RTC1" "/O2;/Ob1" GameAssembly_optimized_options_new "${GameAssembly_optimized_options}")
set_target_properties(GameAssembly_optimized PROPERTIES COMPILE_OPTIONS "${GameAssembly_optimized_options_new}")
