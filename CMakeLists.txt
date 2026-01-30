cmake_minimum_required(VERSION 3.13)
project("unity_clients")

include("cmake/config_flags.cmake")
include("cmake/add_imgui.cmake")
include("cmake/add_autogen.cmake")

option(UC_ENABLE_IMGUI "Enable ImGui" ON)
message(STATUS "UC_ENABLE_IMGUI ${UC_ENABLE_IMGUI}")

set(UC_LOADER "DLL_INJECTOR" CACHE STRING "Loader type")
message(STATUS "UC_LOADER ${UC_LOADER}")

set(UC_STACK_TRACER "DBGHELP" CACHE STRING "Stack Tracer implementation")
message(STATUS "UC_STACK_TRACER ${UC_STACK_TRACER}")

#print_all_flags()
reset_all_flags()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tools/autogen" "tools/autogen" EXCLUDE_FROM_ALL)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/common" "common" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/debuggers/StackTracer" "debuggers/StackTracer" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/static_lambda" "libs/static_lambda" EXCLUDE_FROM_ALL)
set_common_flags(static_lambda)

set(SIMDUTF_TESTS OFF CACHE BOOL "" FORCE)
set(SIMDUTF_TOOLS OFF CACHE BOOL "" FORCE)
set(SIMDUTF_ICONV OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/simdutf" "libs/simdutf" EXCLUDE_FROM_ALL)
set_common_flags(simdutf)

# no sources
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/json" "libs/json" EXCLUDE_FROM_ALL)
#target_link_libraries(${target_name} nlohmann_json)
#set_common_flags(nlohmann_json)

if(UC_ENABLE_IMGUI)
	add_imgui(${CMAKE_CURRENT_LIST_DIR})
endif()

file(GLOB CLIENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/clients/*")
foreach(CLIENT_DIR ${CLIENT_DIRS})
	if(IS_DIRECTORY "${CLIENT_DIR}" AND EXISTS "${CLIENT_DIR}/CMakeLists.txt")
		add_subdirectory("${CLIENT_DIR}")
	endif()
endforeach()
