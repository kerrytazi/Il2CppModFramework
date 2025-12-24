include("${CMAKE_CURRENT_LIST_DIR}/config_mini.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/config_flags.cmake")

function(add_imgui root_dir)
	set(IMGUI_SOURCES
		"${root_dir}/libs/imgui/imconfig.h"
		"${root_dir}/libs/imgui/imgui.cpp"
		"${root_dir}/libs/imgui/imgui.h"
		"${root_dir}/libs/imgui/imgui_demo.cpp"
		"${root_dir}/libs/imgui/imgui_draw.cpp"
		"${root_dir}/libs/imgui/imgui_internal.h"
		"${root_dir}/libs/imgui/imgui_tables.cpp"
		"${root_dir}/libs/imgui/imgui_widgets.cpp"
		"${root_dir}/libs/imgui/imstb_rectpack.h"
		"${root_dir}/libs/imgui/imstb_textedit.h"
		"${root_dir}/libs/imgui/imstb_truetype.h"

		"${root_dir}/libs/imgui/backends/imgui_impl_dx11.cpp"
		"${root_dir}/libs/imgui/backends/imgui_impl_dx11.h"
		"${root_dir}/libs/imgui/backends/imgui_impl_win32.cpp"
		"${root_dir}/libs/imgui/backends/imgui_impl_win32.h"
	)

	config_mini(imgui STATIC "${IMGUI_SOURCES}" ${root_dir})
endfunction()
