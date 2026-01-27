#pragma once

#include <vector>
#include <string>

enum class ParsedClassType
{
	Unknown,
	Class,
	Struct,
	Enum,
};

struct ParsedMethod
{
	std::string name;
	std::string ret_type;
	std::vector<std::string> parameters;
	bool is_static = false;
	bool is_new = false;
	bool is_icall = false;
	bool is_virtual = false;
};

struct ParsedClass
{
	std::string name;
	ParsedClassType type = ParsedClassType::Unknown;
	std::vector<ParsedMethod> methods;
	bool is_gen_finder = false;
};

struct ParsedNamespace
{
	std::string name;
	std::vector<ParsedClass> classes;
	std::vector<ParsedNamespace> namespaces;
};

struct ParsedResult
{
	std::vector<ParsedNamespace> namespaces;
	bool any_autogen = false;
	bool any_new = false;
	bool any_icall = false;
	bool any_method = false;
	bool any_virtual = false;
};
