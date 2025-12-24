#pragma once

#include "common/NoImplement.hpp"

#include <span>
#include <ranges>

namespace il2cpp
{

class Image;

class Assembly : NoImplement
{
public:

	static std::span<const Assembly*> GetAssemblies();
	const Image* GetImage() const { return image; }

	std::string_view GetName() const { return aname.name; } // null-terminated

	template <typename T = void>
	static auto GetImagesView()
	{
		const auto get_image_from_assembly = [](const Assembly* assembly) {
			return assembly->GetImage();
		};

		return
			GetAssemblies() |
			std::views::transform(get_image_from_assembly);
	}

private:

	struct Il2CppAssemblyName
	{
		static const size_t PUBLIC_KEY_BYTE_LENGTH = 8;

		const char* name;
		const char* culture;
		const uint8_t* public_key;
		uint32_t hash_alg;
		int32_t hash_len;
		uint32_t flags;
		int32_t major;
		int32_t minor;
		int32_t build;
		int32_t revision;
		uint8_t public_key_token[PUBLIC_KEY_BYTE_LENGTH];
	};

	Image* image;
	uint32_t token;
	int32_t referencedAssemblyStart;
	int32_t referencedAssemblyCount;
	Il2CppAssemblyName aname;
};

} // namespace il2cpp
