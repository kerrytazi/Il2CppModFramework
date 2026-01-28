#pragma once

#include "il2cpp/Array.hpp"
#include "il2cpp/ClassFinder.hpp"
#include "System/primitives.hpp"

#include <type_traits>

namespace System::Collections::Generic
{

template <typename TKey, typename TValue>
class Dictionary : public System::Object
{
public:

	struct KVEntry
	{
		System::Int32 __hashCode;
		System::Int32 __next;
		alignas(8) TKey key;
		alignas(8) TValue value;
	};

	size_t Length() const { return (size_t)_count; }

	const il2cpp::Array<KVEntry>* Entries() const { return _entries; }
	il2cpp::Array<KVEntry>* Entries() { return _entries; }

	KVEntry& at(size_t index)
	{
		assert(index < Length());
		return Entries()->at(index);
	}

	const KVEntry& at(size_t index) const
	{
		assert(index < Length());
		return Entries()->at(index);
	}

	KVEntry& operator[](size_t index) { return at(index); }
	const KVEntry& operator[](size_t index) const { return at(index); }

	KVEntry* begin() { return Entries()->begin(); }
	KVEntry* end() { return Entries()->begin() + Length(); }

	const KVEntry* begin() const { return Entries()->begin(); }
	const KVEntry* end() const { return Entries()->begin() + Length(); }
	const KVEntry* cbegin() const { return Entries()->begin(); }
	const KVEntry* cend() const { return Entries()->begin() + Length(); }

private:

	il2cpp::Array<System::Int32>* _buckets;
	il2cpp::Array<KVEntry>* _entries;
	System::Int32 _count;
	System::Int32 _freeList;
	System::Int32 _freeCount;
	System::Int32 _version;
	struct System_Collections_Generic_IEqualityComparer_TKey__o* _comparer;
	struct System_Collections_Generic_Dictionary_KeyCollection_TKey__TValue__o* _keys;
	struct System_Collections_Generic_Dictionary_ValueCollection_TKey__TValue__o* _values;
	System::Object* _syncRoot;
};

} // namespace System::Collections::Generic
