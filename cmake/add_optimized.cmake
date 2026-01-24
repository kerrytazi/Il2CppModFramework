include("${CMAKE_CURRENT_LIST_DIR}/../cmake/config_mini.cmake")

function(add_optimized target_name library_type target_sources)
	config_mini(${target_name} ${library_type} "${target_sources}")

	get_target_property(${target_name}_options ${target_name} COMPILE_OPTIONS)
	string(REPLACE "/Ob0;/Od;/RTC1" "/O2;/Ob1" ${target_name}_options_new "${${target_name}_options}")
	set_target_properties(${target_name} PROPERTIES COMPILE_OPTIONS "${${target_name}_options_new}")
endfunction()
