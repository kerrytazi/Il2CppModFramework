cmake_minimum_required(VERSION 3.13)
project("common")

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/config_common.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/add_optimized.cmake")

config_common(common STATIC)

if(UC_ENABLE_IMGUI)
	add_optimized(common_optimized STATIC "${CMAKE_CURRENT_LIST_DIR}/optimized/ImGuiHook.cpp")
	target_link_libraries(common common_optimized)
endif()

if(UC_LOADER STREQUAL "DLL_INJECTOR")
	target_compile_definitions(common PRIVATE UC_LOADER_DLL_INJECTOR=1)
	add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../loaders/dll_injector" "../loaders/dll_injector")
elseif(UC_LOADER STREQUAL "MANUAL_MAPPER")
	target_compile_definitions(common PRIVATE UC_LOADER_MANUAL_MAPPER=1)
else()
	message(FATAL_ERROR "No loader selected.\nUse on of:\n\tcmake -D UC_LOADER=DLL_INJECTOR\n\tcmake -D UC_LOADER=MANUAL_MAPPER")
endif()
