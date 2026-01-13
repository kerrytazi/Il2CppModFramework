cmake_minimum_required(VERSION 3.13)
project("common")

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/config_common.cmake")

config_common(common STATIC)

if(NOT UC_ENABLE_IMGUI)
	set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/src/ImGuiHook.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

if(UC_LOADER STREQUAL "DLL_INJECTOR")
	target_compile_definitions(common PRIVATE UC_DLL_INJECTOR=1)
	add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../loaders/dll_injector" "../loaders/dll_injector")
elseif(UC_LOADER STREQUAL "MANUAL_MAPPER")
	target_compile_definitions(common PRIVATE UC_MANUAL_MAPPER=1)
else()
	message(FATAL_ERROR "No loader selected.\nUse on of:\n\tcmake -D UC_LOADER=DLL_INJECTOR\n\tcmake -D UC_LOADER=MANUAL_MAPPER")
endif()
