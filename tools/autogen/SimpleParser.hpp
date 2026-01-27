#pragma once

#include "ParsedResult.hpp"

#include <string_view>
#include <iostream>
#include <array>
#include <optional>

class SimpleParser
{
public:

	SimpleParser(uint64_t unity_version_num, const char* ptr, size_t size);

	void Parse();

	ParsedResult Result() &&
	{
		return std::move(result_);
	}

private:

	std::string TryReadTypeName();

	std::string_view TryReadName();

	std::string_view TryReadNamespaceName();

	[[nodiscard]]
	bool TryReadToken(std::string_view token);

	[[nodiscard]]
	bool SkipWhitespaces(bool at_least_once = false);

	bool SkipComments();

	struct BracketInfo
	{
		char open = 0;
		uint16_t namespace_index = -1;
		uint16_t class_index = -1;
	};

	[[nodiscard]]
	std::optional<BracketInfo*> TryReadOpenBracket(char expected_bracket = '\0');

	[[nodiscard]]
	std::optional<BracketInfo> TryReadCloseBracket();

	[[nodiscard]]
	bool IsLastBracket(char bracket_open) const;

	[[nodiscard]]
	int SizeLeft() const
	{
		return (int)(end_ - ptr_);
	}

	std::vector<BracketInfo> brackets_;

	const char* begin_ = nullptr;
	const char* ptr_ = nullptr;
	const char* end_ = nullptr;
	uint64_t unity_version_num_ = 0;

	ParsedResult result_;
};

std::optional<ParsedResult> SimpleParse(uint64_t unity_version_num, const char* ptr, size_t size);
