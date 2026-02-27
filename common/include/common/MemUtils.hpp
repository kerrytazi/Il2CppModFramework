#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

namespace mu
{

class PatternSearcher
{
public:

	bool IsEqual(const void* data, size_t size = size_t(-1)) const;
	const void* Find(const void* data, size_t size = size_t(-1)) const;
	const void* FindUp(const void* data, size_t size = size_t(-1)) const;

	static PatternSearcher FromString(std::string_view str);
	static PatternSearcher FromPattern(std::string_view pattern);

private:

	PatternSearcher() = default;

	std::vector<uint8_t> pattern_;
	std::vector<uint8_t> check_;
};

} // namespace mu
