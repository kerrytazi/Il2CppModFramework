include("${CMAKE_CURRENT_LIST_DIR}/config_common.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/version_to_number.cmake")

function(config_client target_name)
	message(STATUS "config_client: ${target_name}")

	config_common(${target_name} SHARED)
	target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

	target_link_libraries(${target_name} common)
	target_link_libraries(${target_name} StackTracer)
	target_link_libraries(${target_name} static_lambda)
	target_link_libraries(${target_name} simdutf)
#	target_link_libraries(${target_name} nlohmann_json)

	if(UC_ENABLE_IMGUI)
		target_link_libraries(${target_name} imgui)
	endif()

	if(DEFINED UC_UNITY_VERSION)
		version_to_number("${UC_UNITY_VERSION}" UC_UNITY_VERSION_NUM)
		message(STATUS "UC_UNITY_VERSION:     ${UC_UNITY_VERSION}")
		message(STATUS "UC_UNITY_VERSION_NUM: ${UC_UNITY_VERSION_NUM}")

		target_compile_definitions(
			${target_name} PRIVATE
			UC_UNITY_VERSION=${UC_UNITY_VERSION}
			UC_UNITY_VERSION_NUM=${UC_UNITY_VERSION_NUM}
		)

		if (TARGET GameAssembly_${UC_UNITY_VERSION})
			message(STATUS "Reusing GameAssembly_${UC_UNITY_VERSION}")
		else()
			message(STATUS "Adding GameAssembly_${UC_UNITY_VERSION}")
			add_subdirectory("${CMAKE_SOURCE_DIR}/GameAssembly" "${CMAKE_BINARY_DIR}/GameAssembly/${UC_UNITY_VERSION}" EXCLUDE_FROM_ALL)
		endif()

		target_link_libraries(${target_name} GameAssembly_${UC_UNITY_VERSION})

		unset(UC_UNITY_VERSION)
		unset(UC_UNITY_VERSION_NUM)
	else()
		message(FATAL_ERROR "UC_UNITY_VERSION is not defined for client: ${target_name}")
	endif()
endfunction()
