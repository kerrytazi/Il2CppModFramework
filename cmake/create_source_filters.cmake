function(create_source_filters target_name)
    # Get target sources
    get_target_property(source_files ${target_name} SOURCES)
    
    foreach(source_file ${source_files})
        # Get the full path and make it relative to project root
        get_filename_component(abs_path ${source_file} ABSOLUTE)
        file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${abs_path})
        
        # Remove filename to get directory path
        get_filename_component(dir_path ${rel_path} DIRECTORY)
        
        if(dir_path)
            # Use the actual directory structure as filter
            string(REPLACE "/" "\\" filter_name "${dir_path}")
            source_group("${filter_name}" FILES ${source_file})
        endif()
    endforeach()
endfunction()