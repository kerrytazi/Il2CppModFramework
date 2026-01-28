#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "common/StringUtils.hpp"
#include "common/Log.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Component.hpp"
#include "System/String.hpp"
#include "System/Threading/ThreadPool.hpp"
#include "System/Threading/WaitCallback.hpp"
#include "il2cpp/il2cpp.hpp"
#include "il2cpp/Assembly.hpp"
#include "il2cpp/Image.hpp"
#include "il2cpp/Class.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/gc_ref.hpp"
#include "System/Collections/Generic/Dictionary.hpp"

#include "common/ExceptionGuarded.hpp"

#include "StackTracer/StackTracer.hpp"

class TestModule : public Module
{
public:

	virtual void Load() override
	{
		Log::Info("TestModule loaded");
	}

private:

	void TestDict()
	{
		System::Collections::Generic::Dictionary<System::Int32, System::Object*>* dict = il2cpp::Class::Find("System.Text", "Encoding")->DynamicStaticField<System::Collections::Generic::Dictionary<System::Int32, System::Object*>*>("encodings");

		for (const auto& kv : *dict)
			Log::Debug("dict[", kv.key, "] = ", kv.value->GetClass()->GetName());
	}

	void TestDynamicField()
	{
		auto str = "Hello world"_il2;
		auto len = str->DynamicField<System::Int32>("_stringLength");
		Log::Debug("_stringLength result: ", len);
		Log::Debug("Empty result: ", str->GetClass()->DynamicStaticField<System::String*>("Empty"));
		Log::Debug("PROBABILISTICMAP_SIZE result: ", str->GetClass()->DynamicStaticLiteralField<System::Int32>("PROBABILISTICMAP_SIZE"));
	}

	void TestInvokeMethod()
	{
		auto str = "Hello world"_il2;
		auto result = str->DynamicInvoke<System::String*>("ToString");
		Log::Debug("ToString result: '", result->AsU16StringView(), "'");
	}

	void TestDumpClass(const std::string& pref, const il2cpp::Class* klass)
	{
		Log::Debug("namespace '", klass->GetNamespace(), "' class '", klass->GetName(), "' 0x", klass);
		if (auto declaring = klass->GetDeclaringClass())
			TestDumpClass(pref + "    declaring: ", declaring);
	}
	void TestDumpClasses()
	{
		for (auto image : il2cpp::Assembly::GetImagesView())
		{
			Log::Debug("----------------");
			Log::Debug("image ", image->GetName());

			for (auto klass : image->GetClassesView())
				TestDumpClass("", klass);
		}
	}

	void TestThreadPool()
	{
		Log::Debug("TestModule MainThreadID: ", GetCurrentThreadId());
		System::Threading::ThreadPool::QueueUserWorkItem(System::Threading::WaitCallback::New([]() {
			Log::Debug("TestModule WorkerThreadID: ", GetCurrentThreadId());
		}));
	}

	il2cpp::gc_ref<System::String> mystr_;
	void TestCreateGCRef()
	{
		mystr_ = il2cpp::gc_ref(u"hello world"_il2);
		auto str2 = mystr_;

		il2cpp::gc_ref<System::Object> test = il2cpp::gc_ref(System::String::New("hello world"));
	}
	void TestUpdateGCRef()
	{
		if (!mystr_.is_alive())
		{
			Log::Debug("TestUpdateGCRef mystr_ died");
			mystr_ = {};
		}
	}

	void TestLog()
	{
		Log::Debug(Logger::Type::System, "Hello ", 3, " world ", cs::Default(false), " ", true);
		Log::Info(u"Hello ", 3, u" world ", cs::Default(false), " ", true);
		Log::Warn((const void*)"Hello", " ", (void*)"world");
		Log::Error(Log::Pad(123), " ", Log::Hex(123), " ", Log::Pad(Log::Hex(123)));
		Log::Error(su::u8((void*)"hello"));

		Log::Info(cs::Default("color Default"), "  ", cs::White("color White"));
		Log::Info(cs::White("color White"), "  ", cs::Black("color Black"));
		Log::Info(cs::Black("color Black"), "  ", cs::Gray("color Gray"));
		Log::Info(cs::Gray("color Gray"), "  ", cs::Red("color Red"));
		Log::Info(cs::Red("color Red"), "  ", cs::Green("color Green"));
		Log::Info(cs::Green("color Green"), "  ", cs::Blue("color Blue"));
		Log::Info(cs::Blue("color Blue"), "  ", cs::Yellow("color Yellow"));
		Log::Info(cs::Yellow("color Yellow"), "  ", cs::Orange("color Orange"));
		Log::Info(cs::Orange("color Orange"), "  ", cs::Pink("color Pink"));
		Log::Info(cs::Pink("color Pink"), "  ", cs::Cyan("color Cyan"));
		Log::Info(cs::Cyan("color Cyan"), "  ", cs::Purple("color Purple"));
		Log::Info(cs::Purple("color Purple"), "  ", cs::Default("color Default"));

		// UTF-8 Test
		Log::Info("\x55\x54\x46\x2d\x38\x20\x54\x65\x73\x74\x3a\x20\x43\x61\x66\xc3\xa9\x20\x45\x73\x70\x61\xc3\xb1\x61");

		// Emojis
		Log::Info("\x45\x6d\x6f\x6a\x69\x73\x3a\x20\xf0\x9f\x98\x80\x20\xf0\x9f\x9a\x80\x20\xf0\x9f\x8e\x89\x20\xf0\x9f\x8c\x8d");

		// Cyrillic
		Log::Info("\x43\x79\x72\x69\x6c\x6c\x69\x63\x3a\x20\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82\x20\xd0\xbc\xd0\xb8\xd1\x80");

		// Japanese
		Log::Info("\x4a\x61\x70\x61\x6e\x65\x73\x65\x3a\x20\xe3\x81\x93\xe3\x82\x93\xe3\x81\xab\xe3\x81\xa1\xe3\x81\xaf\xe4\xb8\x96\xe7\x95\x8c");

		// Chinese
		Log::Info("\x43\x68\x69\x6e\x65\x73\x65\x3a\x20\xe4\xbd\xa0\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c");
	}

	void TestStackTracer()
	{
		auto traces_str = StackTracer::GetStackTrace();
		auto traces = su::SplitView<char>(traces_str, "\n");

		for (const auto& trace : traces)
			Log::Debug(trace);
	}

	void TestException()
	{
		ExceptionGuarded("TestException segfault", [](){
			*(int*)nullptr = 123;
			return 0;
		});

		ExceptionGuarded("TestException segfault", [](){
			((void(*)())nullptr)();
			return 0;
		});

		ExceptionGuarded("TestException game exception", [](){
			auto method = il2cpp::Class::Find("UnityEngine", "GameObject")->FindMethod("get_scene", "UnityEngine.SceneManagement.Scene", {});
			auto method_ptr = method->GetMethodPointer<int(void*)>();
			// Should throw null reference exception
			// Since this method requires `this`
			return method_ptr(nullptr);
		});

		ExceptionGuarded("TestException std exception", [](){
			throw std::runtime_error("Test exception");
			return 0;
		});
	}
};

static RegisterModuleStatic<TestModule> registered;
