#pragma once

#include "common/autogen.hpp"

namespace UnityEngine
{

// [Flags]
enum class __autogen HideFlags
{
	None = 0,
	HideInHierarchy = 1,
	HideInInspector = 2,
	DontSaveInEditor = 4,
	NotEditable = 8,
	DontSaveInBuild = 16,
	DontUnloadUnusedAsset = 32,
	DontSave = 52,
	HideAndDontSave = 61,
};

} // namespace UnityEngine
