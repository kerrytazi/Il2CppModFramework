#pragma once

namespace ImGuiHook
{

void LoadD3D11();
void UnloadD3D11();
void NewFrame();

void SetRenderActive(bool active);
bool IsClientMenuActive();
bool IsFullyInitialized();

}
