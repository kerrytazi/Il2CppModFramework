#include "pch.hpp"

#include "common/MemUtils.hpp"

#include <string>
#include <array>

static constexpr std::array<uint8_t, 256> NIBBLE_TABLE = []() {
	std::array<uint8_t, 256> result{};

	for (auto& c : result)
		c = uint8_t(-1);

	for (char i = '0'; i <= '9'; ++i)
		result[i] = i - '0';

	for (char i = 'a'; i <= 'f'; ++i)
		result[i] = i - 'a' + 10;

	for (char i = 'A'; i <= 'F'; ++i)
		result[i] = i - 'A' + 10;

	result['?'] = uint8_t(-2);

	return result;
}();

mu::PatternSearcher::PatternSearcher(std::string_view pattern)
{
	pattern_.reserve(64);
	check_.reserve(64);

	auto ptr = pattern.data();
	auto end = pattern.data() + pattern.size();

	bool comment = false;

	for (; ptr != end; ++ptr)
	{
		if (*ptr == ' ' || *ptr == '\n' || *ptr == '\r' || *ptr == '\t')
		{
			if (*ptr == '\n')
				comment = false;

			continue;
		}

		if (comment)
			continue;

		if (*ptr == ';')
		{
			comment = true;
			continue;
		}

		auto nibble1 = NIBBLE_TABLE[(uint8_t)*ptr];

		if (nibble1 == uint8_t(-1))
			throw std::runtime_error("Expected nibble. pos: " + std::to_string(ptr - pattern.data()));

		++ptr;

		if (ptr == end)
			throw std::runtime_error("Expected nibble. pos: EOL");

		auto nibble2 = NIBBLE_TABLE[(uint8_t)*ptr];

		if (nibble2 == uint8_t(-1))
			throw std::runtime_error("Expected nibble. pos: " + std::to_string(ptr - pattern.data()));

		uint8_t check1 = nibble1;
		uint8_t check2 = nibble2;

		if (nibble1 == uint8_t(-2))
		{
			nibble1 = 0;
			check1 = 0;
		}

		if (nibble2 == uint8_t(-2))
		{
			nibble2 = 0;
			check2 = 0;
		}

		pattern_.push_back((nibble1 << 4) | nibble2);
		check_.push_back((check1 << 4) | check2);
	}
}

bool mu::PatternSearcher::IsEqual(const void* data, size_t size /*= size_t(-1)*/) const
{
	if (size < pattern_.size())
		return false;

	const uint8_t* udata = (const uint8_t*)data;

	for (size_t i = 0; i < pattern_.size(); ++i)
	{
		if ((udata[i] & pattern_[i]) != check_[i])
			return false;
	}

	return true;
}

const void* mu::PatternSearcher::Find(const void* data, size_t size /*= size_t(-1)*/) const
{
	const uint8_t* udata = (const uint8_t*)data;

	while (size >= pattern_.size())
	{
		if (IsEqual(udata, size))
			return (const void*)udata;

		udata += 1;
		size -= 1;
	}

	return nullptr;
}
