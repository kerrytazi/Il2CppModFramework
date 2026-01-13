cmake_minimum_required(VERSION 3.13)
project("GameAssembly_${UC_UNITY_VERSION}")

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/config_common.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/version_to_number.cmake")

config_common(GameAssembly_${UC_UNITY_VERSION} STATIC)
target_include_directories(GameAssembly_${UC_UNITY_VERSION} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")

add_autogen(GameAssembly_${UC_UNITY_VERSION} "${CMAKE_CURRENT_LIST_DIR}/..")

config_mini(GameAssembly_${UC_UNITY_VERSION}_optimized STATIC "${CMAKE_CURRENT_LIST_DIR}/optimized/update_patch.cpp" "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(GameAssembly_${UC_UNITY_VERSION} GameAssembly_${UC_UNITY_VERSION}_optimized)

get_target_property(GameAssembly_${UC_UNITY_VERSION}_optimized_options GameAssembly_${UC_UNITY_VERSION}_optimized COMPILE_OPTIONS)
string(REPLACE "/Ob0;/Od;/RTC1" "/O2;/Ob1" GameAssembly_${UC_UNITY_VERSION}_optimized_options_new "${GameAssembly_${UC_UNITY_VERSION}_optimized_options}")
set_target_properties(GameAssembly_${UC_UNITY_VERSION}_optimized PROPERTIES COMPILE_OPTIONS "${GameAssembly_${UC_UNITY_VERSION}_optimized_options_new}")

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
