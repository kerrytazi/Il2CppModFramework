#ifdef UC_ENABLE_IMGUI
#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "common/Log.hpp"

#include "imgui_internal.h"

extern void ImGui::ShowDemoWindow(bool*);

// If you have troubles setting up imgui/docking
// Usually you want to disable/remove this module before publishing.
class ImGuiDumperModule : public Module
{
public:

	virtual void OnPostImGuiMenu() override
	{
		if (ImGui::IsKeyDown(ImGuiKey_LeftAlt) && ImGui::IsKeyPressed(ImGuiKey_O, false))
			Dump(ImGui::GetCurrentContext(), 0);
	}

private:

	void Dump(const ImGuiViewport* viewport, int tab)
	{
		auto tabs = std::string(tab, '\t');
	}

	void Dump(const ImGuiWindow* window, int tab)
	{
		auto tabs = std::string(tab, '\t');
		if (window->DockNode)
			Log::Debug(tabs, "DockNode ", cs::White(window->DockNode->ID));
		else
			Log::Debug(tabs, "DockNode ", cs::Gray("(null)"));
	}

	void Dump(const ImGuiDockNode* node, int tab)
	{
		auto tabs = std::string(tab, '\t');

		if (node->HostWindow)
			Log::Debug(tabs, "HostWindow ", cs::Green(node->HostWindow->Name));
		else
			Log::Debug(tabs, "HostWindow ", cs::Gray("(null)"));

		if (node->VisibleWindow)
			Log::Debug(tabs, "VisibleWindow ", cs::Green(node->VisibleWindow->Name));
		else
			Log::Debug(tabs, "VisibleWindow ", cs::Gray("(null)"));

		if (!node->Windows.empty())
		{
			Log::Debug(tabs, "Windows:");
			for (auto window : node->Windows)
				Log::Debug(std::string(tab + 1, '\t'), "Window ", cs::Green(window->Name));
		}
		else
		{
			Log::Debug(tabs, "Windows: ", cs::Gray("empty"));
		}

		if (node->CentralNode)
		{
			Log::Debug(tabs, "CentralNode ", cs::Yellow(node->CentralNode->ID));
			if (node != node->CentralNode)
				Dump(node->CentralNode, tab + 1);
		}
		else
		{
			Log::Debug(tabs, "CentralNode ", cs::Gray("(null)"));
		}

		for (size_t i = 0; i < 2; ++i)
		{
			auto child = node->ChildNodes[i];

			if (child)
			{
				Log::Debug(tabs, "ChildNodes[", cs::Cyan(i), "] ", cs::Yellow(child->ID));
				Dump(child, tab + 1);
			}
			else
			{
				Log::Debug(tabs, "ChildNodes[", cs::Cyan(i), "] ", cs::Gray("(null)"));
			}
		}
	}

	void Dump(const ImGuiContext* ctx, int tab)
	{
		auto tabs = std::string(tab, '\t');
		Log::Debug(tabs, "ImGui dump begin -------");

		Log::Debug(tabs, "Viewports:");
		for (auto viewport : ctx->Viewports)
		{
			Log::Debug(std::string(tab + 1, '\t'), "Viewport ", viewport->ID);
			Dump(viewport, tab + 2);
		}

		Log::Debug(tabs, "Windows:");
		for (auto window : ctx->Windows)
		{
			Log::Debug(std::string(tab + 1, '\t'), "Window ", cs::Green(window->Name));
			Dump(window, tab + 2);
		}

		Log::Debug(tabs, "Dock Nodes:");
		for (auto node_pair : ctx->DockContext.Nodes.Data)
		{
			auto node = (const ImGuiDockNode*)node_pair.val_p;

			// ctx->DockContext.Nodes contains ALL nodes.
			// Since we draw it as a tree
			// we don't need nodes that are placed inside other nodes.
			if (node && node->ID > 1000)
			{
				Log::Debug(std::string(tab + 1, '\t'), "Node ", cs::Yellow(node->ID));
				Dump(node, tab + 2);
			}
		}

		Log::Debug(tabs, "ImGui dump end   -------");
	}

	bool do_dump_ = false;
};

static RegisterModuleStatic<ImGuiDumperModule, 100> registered;

#endif // UC_ENABLE_IMGUI
