#pragma once

namespace ImGuiHook
{

bool Load();
bool Unload();
void NewFrame();

void SetRenderActive(bool active);
bool IsClientMenuActive();
bool IsFullyInitialized();

}
