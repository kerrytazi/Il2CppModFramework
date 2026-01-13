include("${CMAKE_CURRENT_LIST_DIR}/create_source_filters.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/config_flags.cmake")

function(config_mini target_name library_type target_sources)
	if("${library_type}" STREQUAL "EXECUTABLE")
		add_executable(${target_name} ${target_sources})
	else()
		add_library(${target_name} ${library_type} ${target_sources})
	endif()

	target_compile_features(${target_name} PUBLIC cxx_std_20)
	set_common_flags(${target_name})

	target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/common/include")
	target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/GameAssembly/include")
	target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/debuggers/StackTracer/include")
	target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/libs/static_lambda/include")
	target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/libs/simdutf/include")
	target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/libs/json/include")

	if(UC_ENABLE_IMGUI)
		target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/libs/imgui")
	endif()

	create_source_filters(${target_name})
endfunction()
