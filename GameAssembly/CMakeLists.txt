cmake_minimum_required(VERSION 3.13)
project("GameAssembly_${UC_UNITY_VERSION}")

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/config_common.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/version_to_number.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/add_optimized.cmake")

config_common(GameAssembly_${UC_UNITY_VERSION} STATIC)
target_include_directories(GameAssembly_${UC_UNITY_VERSION} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")

add_autogen(GameAssembly_${UC_UNITY_VERSION})

add_optimized(GameAssembly_${UC_UNITY_VERSION}_optimized STATIC "${CMAKE_CURRENT_LIST_DIR}/optimized/update_patch.cpp")
target_link_libraries(GameAssembly_${UC_UNITY_VERSION} GameAssembly_${UC_UNITY_VERSION}_optimized)

target_compile_definitions(
	GameAssembly_${UC_UNITY_VERSION}
	PRIVATE UC_UNITY_VERSION=${UC_UNITY_VERSION}
	PRIVATE UC_UNITY_VERSION_NUM=${UC_UNITY_VERSION_NUM}
)

target_compile_definitions(
	GameAssembly_${UC_UNITY_VERSION}_optimized
	PRIVATE UC_UNITY_VERSION=${UC_UNITY_VERSION}
	PRIVATE UC_UNITY_VERSION_NUM=${UC_UNITY_VERSION_NUM}
)

target_compile_definitions(
	GameAssembly_${UC_UNITY_VERSION}_autogen
	PRIVATE UC_UNITY_VERSION=${UC_UNITY_VERSION}
	PRIVATE UC_UNITY_VERSION_NUM=${UC_UNITY_VERSION_NUM}
)
