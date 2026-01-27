#pragma once

namespace il2cpp
{

class Class;
class Type;

struct TmpMethodInfo;
typedef void(*TmpMethodPointer)();
typedef void (*TmpInvokerMethod)(TmpMethodPointer, const TmpMethodInfo*, void*, void**, void*);
struct TmpMethodInfo
{
	TmpMethodPointer methodPointer;
	TmpMethodPointer virtualMethodPointer;
	TmpInvokerMethod invoker_method;
	const char* name;
	il2cpp::Class *klass;
	const il2cpp::Type *return_type;
	const il2cpp::Type** parameters;
	union
	{
		const struct Il2CppRGCTXData* rgctx_data;
		const void* methodMetadataHandle;
	};
	union
	{
		const void* genericMethod;
		const void* genericContainerHandle;
	};
	uint32_t token;
	uint16_t flags;
	uint16_t iflags;
	uint16_t slot;
	uint8_t parameters_count;
	uint8_t bitflags;
};

} // namespace il2cpp
