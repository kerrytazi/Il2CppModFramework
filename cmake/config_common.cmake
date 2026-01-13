include("${CMAKE_CURRENT_LIST_DIR}/config_mini.cmake")

function(config_common target_name library_type)
	file(GLOB_RECURSE target_sources
		"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	)

	config_mini(${target_name} ${library_type} "${target_sources}")
endfunction()
