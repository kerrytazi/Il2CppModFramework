include("${CMAKE_CURRENT_LIST_DIR}/config_mini.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/config_flags.cmake")
cmake_policy(SET CMP0079 NEW)

function(add_autogen project_target)
	message(STATUS "Adding autogen for ${project_target}")

	get_target_property(target_sources ${project_target} SOURCES)

	set(header_files "")
	foreach(source ${target_sources})
		if(source MATCHES "\\.(h|hpp|hh)$")
			get_filename_component(filename ${source} NAME)
			if(NOT filename MATCHES "^(stdafx|pch|precompiled|prefix)\\.(h|hpp|hh)$")
				if(NOT source MATCHES ".*pch\\.(h|hpp|hh)$")
					list(APPEND header_files ${source})
				endif()
			endif()
		endif()
	endforeach()

	get_target_property(project_source_dir ${project_target} SOURCE_DIR)
	get_target_property(project_binary_dir ${project_target} BINARY_DIR)

	set(generated_files "")
	foreach(header ${header_files})
		file(RELATIVE_PATH relative_path ${project_source_dir} ${header})
		string(REGEX REPLACE "\\.(h|hpp|hh)$" ".cpp" cpp_name ${relative_path})
		set(cpp_file "${project_binary_dir}/autogen/${cpp_name}")
		list(APPEND generated_files ${cpp_file})

		get_filename_component(output_dir ${cpp_file} DIRECTORY)
		file(MAKE_DIRECTORY ${output_dir})

		add_custom_command(
			OUTPUT ${cpp_file}
			COMMAND autogen ${header} ${cpp_file}
			DEPENDS autogen ${header}
			COMMENT "autogen ${cpp_file} from ${header}"
			VERBATIM
		)
	endforeach()

	config_mini(${project_target}_autogen STATIC "${generated_files}" ${CMAKE_SOURCE_DIR})

	target_link_libraries(${project_target} ${project_target}_autogen)
endfunction()
