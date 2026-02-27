#include "pch.hpp"

#include "common/Log.hpp"
#include "common/MemUtils.hpp"
#include "common/MyWindows.hpp"
#include "common/win32_utils.hpp"

extern const std::string& GetExeDir();

static std::unordered_map<std::string, std::vector<std::string>> g_custom_il2cpp_functions_renames;
static std::unordered_map<std::string, std::vector<mu::PatternSearcher>> g_custom_il2cpp_functions_patterns;
static bool g_checked_custom_il2cpp_functions = false;
static bool g_have_custom_il2cpp_functions = false;

static std::vector<char> ReadWholeFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file)
		throw std::runtime_error("Cannot open file: " + filename);

	auto size = file.tellg();
	std::vector<char> buffer(size);

	file.seekg(0);
	file.read(buffer.data(), size);

	return buffer;
}

static bool TryInitCustomIl2CppFunctions()
{
	if (g_checked_custom_il2cpp_functions)
		return g_have_custom_il2cpp_functions;

	g_checked_custom_il2cpp_functions = true;

	auto il2cpp_functions_file = GetExeDir() + "/" + "custom_il2cpp_functions.txt";

	if (std::filesystem::exists(il2cpp_functions_file))
	{
		g_have_custom_il2cpp_functions = true;

		auto data = ReadWholeFile(il2cpp_functions_file);

		const char* ptr = data.data();
		const char* end = data.data() + data.size();

		const char* name_begin = nullptr;
		const char* name_end = nullptr;

		const char* value_begin = nullptr;
		const char* value_end = nullptr;

		bool byte_pattern = false;

		auto add_name_value = [&]() {
			assert(name_begin);
			assert(name_end);
			assert(value_begin);
			assert(value_end);

			std::string name(name_begin, name_end);
			std::string value(value_begin, value_end);

			if (byte_pattern)
				g_custom_il2cpp_functions_patterns[name].push_back(mu::PatternSearcher::FromPattern(value));
			else
				g_custom_il2cpp_functions_renames[name].push_back(value);

			name_begin = nullptr;
			name_end = nullptr;
			value_begin = nullptr;
			value_end = nullptr;

			byte_pattern = false;
		};

		for (; ptr != end; ++ptr)
		{
			if (*ptr == ' ' || *ptr == '\n' || *ptr == '\r' || *ptr == '\t')
			{
				if (name_begin && !name_end)
					name_end = ptr;

				if (!byte_pattern && value_begin && !value_end)
				{
					value_end = ptr;
					add_name_value();
				}

				continue;
			}

			if (!name_end)
			{
				if (!name_begin)
					name_begin = ptr;

				continue;
			}

			if (*ptr == '#')
			{
				if (ptr + 1 != end && ptr[1] == '#')
				{
					++ptr;
					continue;
				}

				if (value_begin)
				{
					value_end = ptr;
					add_name_value();
					continue;
				}

				byte_pattern = true;
				continue;
			}

			if (!value_end)
			{
				if (!value_begin)
					value_begin = ptr;

				continue;
			}

			throw std::runtime_error("custom_il2cpp_functions.txt unexpected symbol at pos: " + su::u8(ptr - data.data()));
		}

		if (!value_end && name_begin)
		{
			value_end = ptr;
			add_name_value();
		}
	}

	return g_have_custom_il2cpp_functions;
}

extern "C" const void* GetIl2CppFunction(const void* GameAssemblyLibrary, const char* name) noexcept(false)
{
	auto result = GetProcAddress((HINSTANCE)GameAssemblyLibrary, name);

	if (!result && TryInitCustomIl2CppFunctions())
	{
		while (!result)
		{
			if (auto it = g_custom_il2cpp_functions_renames.find(name); it != g_custom_il2cpp_functions_renames.end())
			{
				bool recheck = false;

				for (const auto& rename : it->second)
				{
					result = GetProcAddress((HINSTANCE)GameAssemblyLibrary, rename.c_str());

					if (!result)
					{
						name = rename.c_str();
						recheck = true;
						continue;
					}
				}

				if (recheck)
					continue;
			}

			if (auto it = g_custom_il2cpp_functions_patterns.find(name); it != g_custom_il2cpp_functions_patterns.end())
			{
				for (const auto& pattern : it->second)
				{
					for (const auto& proc : win32_utils::ProcsView(GameAssemblyLibrary))
					{
						if (pattern.IsEqual(proc.func))
							return proc.func;
					}
				}

				Log::Warn("custom_il2cpp_functions.txt found pattern '", name, "' but can't find matching bytes");
				return nullptr;
			}

			Log::Warn("custom_il2cpp_functions.txt missing '", name, "'");
			return nullptr;
		}
	}

	return result;
}
