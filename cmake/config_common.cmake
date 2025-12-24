include("${CMAKE_CURRENT_LIST_DIR}/config_mini.cmake")

function(config_common target_name library_type target_dir root_dir)
	file(GLOB_RECURSE target_sources
		"${target_dir}/include/*.hpp"
		"${target_dir}/src/*.hpp"
		"${target_dir}/src/*.cpp"
	)

	config_mini(${target_name} ${library_type} "${target_sources}" ${root_dir})
endfunction()
