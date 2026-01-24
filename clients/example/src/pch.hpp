#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <string>
#include <string_view>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <fstream>
#include <filesystem>
#include <charconv>
#include <bit>

#include "nlohmann/json.hpp"
#include "simdutf.h"

#include "common/MyWindows.hpp"

#ifdef UC_ENABLE_IMGUI
#include "imgui.h"
#endif // UC_ENABLE_IMGUI

#include "static_lambda/detour_lambda.hpp"
