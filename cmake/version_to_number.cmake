function(version_to_number VERSION_STRING OUTPUT_VAR)
	# Parse with regex: decimal.decimal.hex
	string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9a-fA-F]+)$" MATCH_RESULT "${VERSION_STRING}")

	if(NOT MATCH_RESULT)
		message(WARNING "Invalid version format: ${VERSION_STRING}")
		set(${OUTPUT_VAR} 0 PARENT_SCOPE)
		return()
	endif()

	set(MAJOR "${CMAKE_MATCH_1}")
	set(MINOR "${CMAKE_MATCH_2}")
	set(HEX_PART "${CMAKE_MATCH_3}")

	math(EXPR HEX_DECIMAL "0x${HEX_PART}" OUTPUT_FORMAT DECIMAL)
	math(EXPR VERSION_NUM "${MAJOR} * 1000000 + ${MINOR} * 100000 + ${HEX_DECIMAL}")
	set(${OUTPUT_VAR} ${VERSION_NUM} PARENT_SCOPE)
endfunction()
