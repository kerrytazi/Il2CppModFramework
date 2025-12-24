include("${CMAKE_CURRENT_LIST_DIR}/config_common.cmake")

function(config_client target_name target_dir root_dir)
	config_common(${target_name} SHARED ${target_dir} ${root_dir})
	target_include_directories(${target_name} PRIVATE "${target_dir}/src")

	target_link_libraries(${target_name} common)
	target_link_libraries(${target_name} GameAssembly)
	target_link_libraries(${target_name} StackTracer)
	target_link_libraries(${target_name} static_lambda)
	target_link_libraries(${target_name} simdutf)
#	target_link_libraries(${target_name} nlohmann_json)

	if(UC_ENABLE_IMGUI)
		target_link_libraries(${target_name} imgui)
	endif()
endfunction()
