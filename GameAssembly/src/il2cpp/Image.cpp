#include "pch.hpp"

#include "il2cpp/Image.hpp"

#include "il2cpp/Class.hpp"
#include "il2cpp/Assembly.hpp"

#include "il2cpp_data.hpp"

const il2cpp::Class* il2cpp::Image::GetClass(size_t index) const
{
	assert(g_il2cpp_data.GameAssembly);
	assert(g_il2cpp_data.il2cpp_image_get_class);

	return g_il2cpp_data.il2cpp_image_get_class(this, index);
}
