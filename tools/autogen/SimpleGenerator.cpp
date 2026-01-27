#include "SimpleGenerator.hpp"
#include "ParsedResult.hpp"

#include <sstream>
#include <set>

static std::string ReplaceAll(std::string str, const std::string_view& from, const std::string_view& to)
{
	if (from.empty())
		return str;

	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	return str;
}

static std::string ReplaceArray(std::string type, bool add_array = true)
{
	constexpr auto arr_start = std::string_view("il2cpp.Array<");
	constexpr auto arr_cs = std::string_view("[]");

	if (type.starts_with(arr_start))
	{
		type.erase(type.begin(), type.begin() + arr_start.size());
		type.erase(type.end() - 1);

		if (add_array)
			type.insert(type.end(), arr_cs.begin(), arr_cs.end());

		type = ReplaceArray(std::move(type));
	}

	return type;
}

static std::string NormalizeType(std::string type, bool add_array = true)
{
	auto replaced_scope = ReplaceAll(std::move(type), "::", ".");
	auto removed_pointers = ReplaceAll(std::move(replaced_scope), "*", "");
	auto removed_spaces = ReplaceAll(std::move(removed_pointers), " ", "");
	auto replaced_array = ReplaceArray(std::move(removed_spaces), add_array);
	auto removed_empty_template = ReplaceAll(std::move(replaced_array), "<>", "");

	return removed_empty_template;
}

static std::string NormalizeMethodName(std::string method)
{
	auto m1 = ReplaceAll(std::move(method), "_ctor", ".ctor");

	return m1;
}

std::string SimpleGenerate(const ParsedResult& parsed)
{
	std::string_view generated_comment = "// Auto generated file - will be overwritten by autogen tool\n";

	if (!parsed.any_autogen)
		return std::string(generated_comment);

	std::stringstream ss;
	ss << generated_comment;

	{
		std::set<std::string> all_types;

		for (const auto& ns : parsed.namespaces)
		{
			for (const auto& cl : ns.classes)
			{
				if (cl.is_gen_finder)
					all_types.insert(ReplaceAll(ns.name, "::", ".") + "." + cl.name);

				auto ignore_class = ns.name + "::" + cl.name + "::";

				for (const auto& m : cl.methods)
				{
					all_types.insert(ReplaceAll(ns.name, "::", ".") + "." + cl.name);

					if (m.ret_type.find(ignore_class) == std::string_view::npos && m.ret_type.find("*") == std::string_view::npos)
						all_types.insert(NormalizeType(m.ret_type, false));

					for (const auto& param : m.parameters)
					{
						if (param.find(ignore_class) == std::string_view::npos && param.find("*") == std::string_view::npos && param.find("&") == std::string_view::npos)
							all_types.insert(ReplaceAll(NormalizeType(param, false), "&", ""));
					}
				}
			}
		}

		all_types.erase("System.Void");
		all_types.erase("System.Boolean");
		all_types.erase("System.Char");
		all_types.erase("System.Byte");
		all_types.erase("System.SByte");
		all_types.erase("System.Int16");
		all_types.erase("System.UInt16");
		all_types.erase("System.Int32");
		all_types.erase("System.UInt32");
		all_types.erase("System.Int64");
		all_types.erase("System.UInt64");
		all_types.erase("System.IntPtr");
		all_types.erase("System.UIntPtr");
		all_types.erase("System.Single");
		all_types.erase("System.Double");

		ss << "#include \"System/primitives.hpp\"\n";
		ss << "#include \"common/CallCached.hpp\"\n";
		ss << "#include \"il2cpp/Class.hpp\"\n";
		ss << "#include \"il2cpp/ClassFinder.hpp\"\n";
		if (parsed.any_icall)
			ss << "#include \"il2cpp/il2cpp.hpp\"\n";
		if (parsed.any_method)
			ss << "#include \"il2cpp/Method.hpp\"\n";
		ss << "\n";
		ss << "#include <cassert>\n\n";

		for (const auto& type : all_types)
			ss << "#include \"" << ReplaceAll(type, ".", "/") << ".hpp\"\n";

		if (!all_types.empty())
			ss << "\n";
	}

	for (const auto& ns : parsed.namespaces)
	{
		for (const auto& cl : ns.classes)
		{
			if (!cl.is_gen_finder)
				continue;

			ss << "const il2cpp::Class* il2cpp::FindClassOnce<" << ns.name << "::" << cl.name << ">::Find()\n";
			ss << "{\n";
			ss << "\tauto klass = CallCached<decltype([]() { ";
			ss << "return il2cpp::Class::Find(\"" << NormalizeType(ns.name) << "\", \"" << cl.name << "\");";
			ss << " })>(); assert(klass);\n";
			ss << "\tklass->_ForceInitFull();\n";
			ss << "\treturn klass;\n";
			ss << "}\n\n";
		}
	}

	for (const auto& ns : parsed.namespaces)
	{
		for (const auto& cl : ns.classes)
		{
			auto ignore_class = ns.name + "::" + cl.name + "::";

			for (const auto& m : cl.methods)
			{
				ss << m.ret_type << " " << ns.name << "::" << cl.name << "::" << m.name << "(";

				for (size_t i = 0; i < m.parameters.size(); ++i)
				{
					ss << m.parameters[i] << " a" << i;

					if (i < m.parameters.size() - 1)
						ss << ", ";
				}

				ss << ")\n";

				ss << "{\n";

				if (m.is_virtual)
					ss << "\tauto vmethod = ";
				else
					ss << "\tauto func = ";

				ss << "CallCached<decltype([]() {\n";

				auto add_method_ptr = [&](std::string_view var_name) {
					ss << "\tauto " << var_name << " = method->template GetMethodPointer<";
					ss << m.ret_type << "(";

					if (!m.is_static)
					{
						ss << "decltype(this)";

						if (!m.parameters.empty())
							ss << ", ";
					}

					for (size_t i = 0; i < m.parameters.size(); ++i)
					{
						ss << m.parameters[i];

						if (i < m.parameters.size() - 1)
							ss << ", ";
					}

					ss << ")";

					ss << ">(); assert(" << var_name << ");\n";
				};

				if (m.is_icall)
				{
					ss << "\t\tusing func_t = ";
					ss << m.ret_type << "(*)(";

					if (!m.is_static)
					{
						ss << "decltype(this)";

						if (!m.parameters.empty())
							ss << ", ";
					}

					for (size_t i = 0; i < m.parameters.size(); ++i)
					{
						ss << m.parameters[i];

						if (i < m.parameters.size() - 1)
							ss << ", ";
					}

					ss << ");\n";

					ss << "\t\treturn (func_t)il2cpp::resolve_icall(";
					ss << "\"" << ns.name << "." << cl.name << "::" << m.name << "\"";
					ss << ");\n";
				}
				else
				{
					ss << "\t\tauto klass = il2cpp::Find<" << ns.name << "::" << cl.name << ">(); assert(klass);\n";
					if (m.is_virtual)
						ss << "\t\tauto vmethod = klass->FindMethod(";
					else
						ss << "\t\tauto method = klass->FindMethod(";
					ss << "\"" << NormalizeMethodName(m.name) << "\", ";
					ss << "\"" << NormalizeType(m.ret_type) << "\", ";

					ss << "{ ";
					for (size_t i = 0; i < m.parameters.size(); ++i)
					{
						ss << "\"" << NormalizeType(m.parameters[i]) << "\"";

						if (i < m.parameters.size() - 1)
							ss << ",";

						ss << " ";
					}
					ss << "}, ";

					ss << "" << (m.is_static ? "true" : "false");

					if (m.is_virtual)
						ss << "); assert(vmethod);\n";
					else
						ss << "); assert(method);\n";

					if (m.is_virtual)
					{
						ss << "\t\treturn vmethod;\n";
					}
					else
					{
						ss << "\t";
						add_method_ptr("method_ptr");
						ss << "\t\treturn method_ptr;\n";
					}
				}

				ss << "\t})>();\n";

				if (m.is_virtual)
				{
					ss << "\tauto method = GetClass()->GetVirtualMethod(vmethod); assert(method);\n";
					add_method_ptr("func");
				}

				ss << "\treturn func(";

				if (!m.is_static)
				{
					ss << "this";

					if (!m.parameters.empty())
						ss << ", ";
				}

				for (size_t i = 0; i < m.parameters.size(); ++i)
				{
					ss << "a" << i;

					if (i < m.parameters.size() - 1)
						ss << ", ";
				}

				ss << ");\n";

				ss << "}\n\n";
			}
		}
	}

	return std::move(ss).str();
}
