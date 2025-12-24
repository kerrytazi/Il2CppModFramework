#pragma once

#include "common/NoImplement.hpp"

#include <ranges>

namespace il2cpp
{

class Class;
class Assembly;

class Image : NoImplement
{
public:

	Assembly* GetAssembly() const { return assembly; }

	size_t GetClassCount() const { return size_t(typeCount); }
	const Class* GetClass(size_t index) const;

	std::string_view GetName() const { return name; } // null-terminated

	template <typename T = void>
	auto GetClassesView() const
	{
		const auto get_class_by_index = [this](size_t index) {
			return GetClass(index);
		};

		return
			std::views::iota(size_t(0), GetClassCount()) |
			std::views::transform(get_class_by_index);
	}

private:

	const char* name;
	const char* nameNoExt;
	Assembly* assembly;

	uint32_t typeCount;
	uint32_t exportedTypeCount;
	uint32_t customAttributeCount;

	const struct ___Il2CppMetadataImageHandle* metadataHandle;

	struct Il2CppNameToTypeHandleHashTable* nameToClassHashTable;

	const struct Il2CppCodeGenModule* codeGenModule;

	uint32_t token;
	uint8_t dynamic;
};

} // namespace il2cpp
