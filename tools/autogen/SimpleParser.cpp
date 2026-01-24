#include "SimpleParser.hpp"

constexpr static std::array<bool, 256> WHITESPACE_TABLE = []() {
	std::array<bool, 256> result{};
	result[(int)' '] = true;
	result[(int)'\t'] = true;
	result[(int)'\n'] = true;
	result[(int)'\r'] = true;
	return result;
}();

constexpr static std::array<char, 256> BRACKET_OPEN_TABLE = []() {
	std::array<char, 256> result{};
	result[(int)'('] = ')';
	result[(int)'{'] = '}';
	result[(int)'['] = ']';
	return result;
}();

constexpr static std::array<char, 256> BRACKET_CLOSE_TABLE = []() {
	std::array<char, 256> result{};
	result[(int)')'] = '(';
	result[(int)'}'] = '{';
	result[(int)']'] = '[';
	return result;
}();

constexpr static std::array<bool, 256> VALID_NAME_TABLE = []() {
	std::array<bool, 256> result{};
	for (char c = 'a'; c <= 'z'; ++c) result[(int)c] = true;
	for (char c = 'A'; c <= 'Z'; ++c) result[(int)c] = true;
	for (char c = '0'; c <= '9'; ++c) result[(int)c] = true;
	result[(int)'_'] = true;
	result[(int)'$'] = true;
	return result;
}();

constexpr static std::array<bool, 256> VALID_NAMESPACE_TABLE = []() {
	std::array<bool, 256> result = VALID_NAME_TABLE;
	result[(int)':'] = true;
	return result;
}();

SimpleParser::SimpleParser(const char* ptr, size_t size)
	: begin_{ ptr }
	, ptr_{ ptr }
	, end_{ ptr + size }
{
}

void SimpleParser::Parse()
{
	char want_open_bracket = 0;
	bool want_whitespaces = false;
	bool want_name_namespace = false;
	bool want_name_class = false;
	bool want_autogen = false;
	bool want_autogen_icall = false;
	bool want_autogen_virtual = false;
	bool want_method_ret_type = false;
	bool want_method_name = false;
	bool want_method_arguments = false;
	bool want_method_argument_end = false;
	bool want_class_decl_end = false;
	uint16_t cur_namespace_index = (uint16_t)-1;
	uint16_t cur_class_index = (uint16_t)-1;
	uint16_t prev_namespace_index = (uint16_t)-1;
	uint16_t prev_class_index = (uint16_t)-1;
	uint16_t cur_method_index = (uint16_t)-1;
	std::string_view method_ret_type;
	std::string_view method_name;

	while (ptr_ != end_)
	{
		if (SkipComments())
		{
			want_whitespaces = false;
			continue;
		}

		if (SkipWhitespaces(want_whitespaces))
		{
			want_whitespaces = false;
			continue;
		}

		if (cur_namespace_index != (uint16_t)-1)
		{
			if (auto pres = TryReadOpenBracket(want_open_bracket))
			{
				(*pres)->namespace_index = prev_namespace_index;
				(*pres)->class_index = prev_class_index;
				prev_namespace_index = cur_namespace_index;
				prev_class_index = cur_class_index;
				want_open_bracket = 0;
				want_class_decl_end = false;
				continue;
			}

			if (want_method_arguments && *ptr_ == ')')
			{
				want_method_arguments = false;
				want_method_argument_end = false;
				cur_method_index = (uint16_t)-1;
			}

			if (want_method_argument_end)
			{
				if (TryReadToken(","))
				{
					want_method_argument_end = false;
					continue;
				}

				auto param_name = TryReadName();
				(void)param_name;
				continue;
			}

			if (auto res = TryReadCloseBracket())
			{
				prev_namespace_index = cur_namespace_index = res->namespace_index;
				prev_class_index = cur_class_index = res->class_index;
				continue;
			}

			if (TryReadToken("__autogen"))
			{
				want_whitespaces = true;
				want_autogen = true;
				result_.any_autogen = true;
				result_.any_method = true;
				continue;
			}

			if (TryReadToken("__autogen_icall"))
			{
				if (want_name_class)
					throw std::runtime_error("Invalid __autogen_icall. Can't be declared for class");

				want_whitespaces = true;
				want_autogen_icall = true;
				result_.any_autogen = true;
				result_.any_icall = true;
				continue;
			}

			if (TryReadToken("__autogen_virtual"))
			{
				if (want_name_class)
					throw std::runtime_error("Invalid __autogen_virtual. Can't be declared for class");

				want_whitespaces = true;
				want_autogen_virtual = true;
				result_.any_autogen = true;
				result_.any_method = true;
				result_.any_virtual = true;
				continue;
			}

			if (want_name_class)
			{
				auto& klass = result_.namespaces[cur_namespace_index].classes[cur_class_index];
				klass.name = TryReadName();

				if (want_autogen)
				{
					klass.is_gen_finder = true;
					want_autogen = false;
				}

				want_name_class = false;
				continue;
			}

			if (want_method_ret_type)
			{
				if (TryReadToken("inline"))
				{
					want_whitespaces = true;
					continue;
				}
				if (TryReadToken("constexpr"))
				{
					want_whitespaces = true;
					continue;
				}
				if (TryReadToken("[[nodiscard]]"))
				{
					continue;
				}
				if (TryReadToken("[[noreturn]]"))
				{
					continue;
				}

				if (TryReadToken("static"))
				{
					want_whitespaces = true;
					auto& method = result_.namespaces[cur_namespace_index].classes[cur_class_index].methods[cur_method_index];
					method.is_static = true;
					continue;
				}
			}

			if (want_method_ret_type)
			{
				auto& method = result_.namespaces[cur_namespace_index].classes[cur_class_index].methods[cur_method_index];
				method.ret_type = TryReadTypeName();
				want_whitespaces = true;
				want_method_name = true;
				want_method_ret_type = false;
				continue;
			}

			if (want_method_name)
			{
				auto& method = result_.namespaces[cur_namespace_index].classes[cur_class_index].methods[cur_method_index];
				method.name = TryReadName();
				want_open_bracket = '(';
				want_method_arguments = true;
				want_method_name = false;
				continue;
			}

			if (want_method_arguments)
			{
				auto& method = result_.namespaces[cur_namespace_index].classes[cur_class_index].methods[cur_method_index];
				auto& param = method.parameters.emplace_back();
				param = TryReadTypeName();
				want_method_argument_end = true;
				continue;
			}

			if (want_autogen)
			{
				if (cur_class_index == (uint16_t)-1)
					throw std::runtime_error("Invalid __autogen. Outside of class");

				cur_method_index = (uint16_t)result_.namespaces[cur_namespace_index].classes[cur_class_index].methods.size();
				auto& method = result_.namespaces[cur_namespace_index].classes[cur_class_index].methods.emplace_back();
				want_method_ret_type = true;
				want_autogen = false;
				continue;
			}

			if (want_autogen_icall)
			{
				if (cur_class_index == (uint16_t)-1)
					throw std::runtime_error("Invalid __autogen_icall. Outside of class");

				cur_method_index = (uint16_t)result_.namespaces[cur_namespace_index].classes[cur_class_index].methods.size();
				auto& method = result_.namespaces[cur_namespace_index].classes[cur_class_index].methods.emplace_back();
				method.is_icall = true;
				want_method_ret_type = true;
				want_autogen_icall = false;
				continue;
			}

			if (want_autogen_virtual)
			{
				if (cur_class_index == (uint16_t)-1)
					throw std::runtime_error("Invalid __autogen_virtual. Outside of class");

				cur_method_index = (uint16_t)result_.namespaces[cur_namespace_index].classes[cur_class_index].methods.size();
				auto& method = result_.namespaces[cur_namespace_index].classes[cur_class_index].methods.emplace_back();
				method.is_virtual = true;
				want_method_ret_type = true;
				want_autogen_virtual = false;
				continue;
			}

			if (want_name_namespace)
			{
				result_.namespaces[cur_namespace_index].name = TryReadNamespaceName();
				want_name_namespace = false;
				want_open_bracket = '{';
				continue;
			}

			if (cur_method_index == (uint16_t)-1)
			{
				// TODO: separate?
				if (TryReadToken("enum class"))
				{
					want_whitespaces = true;
					want_name_class = true;
					want_class_decl_end = true;
					cur_class_index = (uint16_t)result_.namespaces[cur_namespace_index].classes.size();
					auto& klass = result_.namespaces[cur_namespace_index].classes.emplace_back();
					klass.type = ParsedClassType::Enum;
					continue;
				}

				if (TryReadToken("class"))
				{
					want_whitespaces = true;
					want_name_class = true;
					want_class_decl_end = true;
					cur_class_index = (uint16_t)result_.namespaces[cur_namespace_index].classes.size();
					auto& klass = result_.namespaces[cur_namespace_index].classes.emplace_back();
					klass.type = ParsedClassType::Class;
					continue;
				}

				if (TryReadToken("struct"))
				{
					want_whitespaces = true;
					want_name_class = true;
					want_class_decl_end = true;
					cur_class_index = (uint16_t)result_.namespaces[cur_namespace_index].classes.size();
					auto& klass = result_.namespaces[cur_namespace_index].classes.emplace_back();
					klass.type = ParsedClassType::Struct;
					continue;
				}
			}

			if (want_class_decl_end && TryReadToken(";"))
			{
				// Remove classes without definition
				result_.namespaces[cur_namespace_index].classes.pop_back();
				cur_class_index = prev_class_index;
				want_class_decl_end = false;
				continue;
			}
		}

		if (cur_class_index == (uint16_t)-1)
		{
			if (TryReadToken("namespace"))
			{
				want_whitespaces = true;
				want_name_namespace = true;
				cur_namespace_index = (uint16_t)result_.namespaces.size();
				result_.namespaces.emplace_back();
				continue;
			}
		}

		++ptr_;
	}

	for (auto it = result_.namespaces.begin(); it != result_.namespaces.end();)
	{
		if (it->classes.empty())
			it = result_.namespaces.erase(it);
		else
			++it;
	}
}

std::string SimpleParser::TryReadTypeName()
{
	std::string result;
	int in_template = 0;

	while (ptr_ != end_)
	{
		if (VALID_NAMESPACE_TABLE[(size_t)*ptr_])
		{
			result += *ptr_++;
			continue;
		}

		if (*ptr_ == '<')
		{
			++in_template;
			result += *ptr_++;
			continue;
		}

		if (*ptr_ == '*')
		{
			result += *ptr_++;
			continue;
		}

		if (in_template)
		{
			if (*ptr_ == '>')
			{
				--in_template;
				result += *ptr_++;
				continue;
			}

			if (*ptr_ == '(' || *ptr_ == ')' || *ptr_ == ',' || *ptr_ == ' ')
			{
				result += *ptr_++;
				continue;
			}
		}

		break;
	}

	if (result.empty())
		throw std::runtime_error("Expected type name");

	return result;
}

std::string_view SimpleParser::TryReadName()
{
	auto prev = ptr_;
	while (ptr_ != end_ && VALID_NAME_TABLE[(size_t)*ptr_])
		++ptr_;

	if (prev == ptr_)
		throw std::runtime_error("Expected name");

	return std::string_view(prev, ptr_);
}

std::string_view SimpleParser::TryReadNamespaceName()
{
	auto prev = ptr_;
	while (ptr_ != end_ && VALID_NAMESPACE_TABLE[(size_t)*ptr_])
		++ptr_;

	if (prev == ptr_)
		throw std::runtime_error("Expected name");

	return std::string_view(prev, ptr_);
}

bool SimpleParser::TryReadToken(std::string_view token)
{
	if (token.empty())
		throw std::runtime_error("Invalid token");

	if (SizeLeft() < (int)token.size())
		return false;

	if (std::string_view(ptr_, token.size()) != token)
		return false;

	if (VALID_NAME_TABLE[(size_t)token.back()] && SizeLeft() > (int)token.size())
	{
		// Simple lookahead for one symbol.
		// To disallow searching for "class" in "className".
		if (VALID_NAME_TABLE[(size_t)ptr_[token.size()]])
			return false;
	}

	if (VALID_NAME_TABLE[(size_t)token[0]] && ptr_ != begin_)
	{
		// Simple lookbehind for one symbol.
		// To disallow searching for "class" in "generic_class".
		if (VALID_NAME_TABLE[(size_t)ptr_[-1]])
			return false;
	}

	ptr_ += token.size();
	return true;
}

bool SimpleParser::SkipWhitespaces(bool at_least_once /*= false*/)
{
	auto prev = ptr_;

	while (ptr_ != end_ && WHITESPACE_TABLE[(size_t)*ptr_])
		++ptr_;

	if (at_least_once && prev == ptr_)
		throw std::runtime_error("Expected whitespace");

	return prev != ptr_;
}

bool SimpleParser::SkipComments()
{
	if (SizeLeft() >= 2 && ptr_[0] == '/' && ptr_[1] == '/')
	{
		ptr_ += 2;

		while (ptr_ != end_ && *ptr_ != '\n')
			++ptr_;

		if (ptr_ != end_ && *ptr_ == '\n')
			++ptr_;

		return true;
	}

	if (SizeLeft() >= 2 && ptr_[0] == '/' && ptr_[1] == '*')
	{
		ptr_ += 2;

		while (SizeLeft() >= 2 && (ptr_[0] != '*' || ptr_[1] != '/'))
			++ptr_;

		if (SizeLeft() >= 2 && ptr_[0] == '*' && ptr_[1] == '/')
			ptr_ += 2;
		else
			throw std::runtime_error("Missing multiline close");

		return true;
	}

	return false;
}

std::optional<SimpleParser::SimpleParser::BracketInfo*> SimpleParser::TryReadOpenBracket(char expected_bracket /*= '\0'*/)
{
	if (expected_bracket && (ptr_ >= end_ || expected_bracket != *ptr_))
		throw std::runtime_error("Expected bracket");

	if (ptr_ >= end_)
		return std::nullopt;

	auto bracket_close = BRACKET_OPEN_TABLE[(size_t)*ptr_];

	if (!bracket_close)
		return std::nullopt;

	auto bracket_info = &brackets_.emplace_back(*ptr_);
	++ptr_;
	return bracket_info;
}

std::optional<SimpleParser::SimpleParser::BracketInfo> SimpleParser::TryReadCloseBracket()
{
	if (ptr_ >= end_)
		return std::nullopt;

	auto bracket_open = BRACKET_CLOSE_TABLE[(size_t)*ptr_];

	if (!bracket_open)
		return std::nullopt;

	if (!IsLastBracket(bracket_open))
		throw std::runtime_error("Invalid bracket close");

	auto bracket_info = std::move(brackets_.back());
	brackets_.pop_back();
	++ptr_;
	return bracket_info;
}

bool SimpleParser::IsLastBracket(char bracket_open) const
{
	if (brackets_.empty())
		return false;

	return brackets_.back().open == bracket_open;
}

std::optional<ParsedResult> SimpleParse(const char* ptr, size_t size)
{
	try
	{
		SimpleParser parser(ptr, size);
		parser.Parse();
		return std::move(parser).Result();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what();
		return std::nullopt;
	}
}
