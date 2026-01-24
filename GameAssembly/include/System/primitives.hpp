#pragma once

#include <cstdint>
#include <compare>

namespace System
{

using Void = void;
using Boolean = bool;
using Char = char;
using Byte = uint8_t;
using SByte = int8_t;
using Int16 = int16_t;
using UInt16 = uint16_t;
using Int32 = int32_t;
using UInt32 = uint32_t;
using Int64 = int64_t;
using UInt64 = uint64_t;

using Single = float;
using Double = double;

struct IntPtr { int64_t val; };
struct UIntPtr { uint64_t val; };

} // namespace System
