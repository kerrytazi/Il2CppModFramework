#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

namespace mu
{

class PatternSearcher
{
public:

	PatternSearcher(std::string_view pattern);

	bool IsEqual(const void* data, size_t size = size_t(-1)) const;
	const void* Find(const void* data, size_t size = size_t(-1)) const;

private:

	std::vector<uint8_t> pattern_;
	std::vector<uint8_t> check_;
};

} // namespace mu
