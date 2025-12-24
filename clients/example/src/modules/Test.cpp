#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "common/StringUtils.hpp"
#include "common/Log.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Component.hpp"
#include "System/Exception.hpp"
#include "System/String.hpp"
#include "il2cpp/il2cpp.hpp"
#include "il2cpp/Class.hpp"

#include "StackTracer/StackTracer.hpp"

class TestModule : public Module
{
public:

	virtual void Load() override
	{
		Log::Info("TestModule loaded");
	}

private:

	void TestStackTracer()
	{
		auto traces_str = StackTracer::GetStackTrace();
		auto traces = su::SplitView<char>(traces_str, "\n");

		for (const auto& trace : traces)
			Log::Debug(trace);
	}

	void TestException()
	{
		using func_t = int(*)(void*);
		func_t func = (func_t)(il2cpp::GetGameAssemblyBase() + 0x6F17C20); // FIXME

		try
		{
			func(nullptr);
		}
		catch (const std::exception& ex)
		{
			Log::Debug("Exception: " + std::string(ex.what()));
		}
		catch (Il2CppExceptionWrapper& ex)
		{
			Log::Debug("Exception: " + ex.GetException()->GetMessage()->AsString());
			Log::Debug("Exception: " + ex.GetException()->GetSource()->AsString());
			Log::Debug("Exception: " + ex.GetException()->GetStackTrace()->AsString());
			Log::Debug("Exception: " + std::to_string((intptr_t)ex.GetException()->GetInnerException()));
		}
		catch (...)
		{
			Log::Debug("Exception ?");
		}
	}
};

static RegisterModuleStatic<TestModule> registered;
