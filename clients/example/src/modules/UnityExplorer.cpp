#ifdef UC_ENABLE_IMGUI
#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "common/Log.hpp"
#include "common/ExitScope.hpp"

#include "il2cpp/Class.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/Field.hpp"
#include "il2cpp/Property.hpp"
#include "il2cpp/Event.hpp"

#include "il2cpp/Array.hpp"
#include "il2cpp/gc_ref.hpp"

#include "System/String.hpp"

#include "UnityEngine/Input.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "imgui_internal.h"

static ImGuiDockNode* SplitWindowInNewDock(
	ImGuiWindow* left_window,
	const std::string& right_name)
{
	ImGuiID split_id = ImGui::GetID((std::string(left_window->Name) + "_Dock").c_str());

	ImGuiID left_id = 0;
	ImGuiID right_id = 0;

	ImGui::DockBuilderAddNode(split_id);
	ImGui::DockBuilderSetNodePos(split_id, left_window->Pos);
	auto new_size = left_window->Size;
	if (new_size.x < 700) new_size.x += 400;
	ImGui::DockBuilderSetNodeSize(split_id, new_size);

	ImGui::DockBuilderSplitNode(split_id, ImGuiDir_Left, 0.5f, &left_id, &right_id);
	ImGui::DockBuilderDockWindow(left_window->Name, left_id);
	ImGui::DockBuilderDockWindow(right_name.c_str(), right_id);

	ImGui::DockBuilderFinish(split_id);

	return left_window->DockNode->ParentNode;
}

static bool TrySplitOrMergeWindow(
	ImGuiWindow* left_window,
	const std::string& right_name,
	std::string_view merge_with)
{
	if (!left_window->DockNode)
		return false;
	if (!left_window->DockNode->ParentNode)
		return false;

	auto parent = left_window->DockNode->ParentNode;

	auto sibling_node = parent->ChildNodes[0];
	if (left_window->DockNode == sibling_node)
		sibling_node = parent->ChildNodes[1];

	// If left_window was splitted before but now have no siblings.
	// FIXME: sibling_node may exist but be in invalid state.
	// As a workaround we check VisibleWindow.
	if (sibling_node && (!sibling_node->VisibleWindow || std::string_view(sibling_node->VisibleWindow->Name).find(merge_with) != std::string_view::npos))
	{
		ImGui::DockBuilderDockWindow(right_name.c_str(), sibling_node->ID);
	}
	else
	// If can't split then just add new window to tabs.
	{
		ImGui::DockBuilderDockWindow(right_name.c_str(), left_window->DockNode->ID);
	}

	ImGui::DockBuilderFinish(parent->ID);

	return true;
}

template <typename TReadFunc, typename TWriteFunc>
static void MiniFieldText(TReadFunc&& read_func, TWriteFunc&& write_func)
{
	constexpr bool is_write_null = std::is_null_pointer_v<std::remove_reference_t<TWriteFunc>>;

	ImGuiInputTextFlags flags = 0;
	if (is_write_null)
		flags |= ImGuiInputTextFlags_ReadOnly;
	else
		flags |= ImGuiInputTextFlags_EnterReturnsTrue;

	if constexpr (std::is_same_v<std::invoke_result_t<TReadFunc>, std::u16string_view>)
	{
		char buf[256];
		auto orig_name = read_func();
		auto buf_size = su::u8(buf, sizeof(buf), orig_name);
		buf[buf_size] = '\0';

		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::InputText("##MiniFieldText2", buf, sizeof(buf)), flags)
		{
			char16_t buf16[256];
			auto buf16_size = su::u16(buf16, sizeof(buf16)/sizeof(buf16[0]), (const char*)buf);
			write_func(std::u16string_view(buf16, buf16_size));
		}
	}
	else
	if constexpr (std::is_same_v<std::invoke_result_t<TReadFunc>, std::string_view>)
	{
		if constexpr (is_write_null)
		{
			auto orig_name = read_func();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputText("##MiniFieldText", (char*)orig_name.data(), orig_name.size() + 1, flags);
		}
		else
		{
			char buf[256]{};
			auto orig_name = read_func();
			memcpy(buf, orig_name.data(), orig_name.size());

			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputText("##MiniFieldText", buf, sizeof(buf)), flags)
				write_func(std::string_view(buf));
		}
	}
	else
	{
		static_assert(templates::always_false<TReadFunc>, "Not Implemented");
	}
}

template <typename TReadFunc, typename TWriteFunc>
static void MiniFieldSlider3(float min_max, TReadFunc&& read_func, TWriteFunc&& write_func)
{
	bool changed = false;
	UnityEngine::Vector3 val = read_func();

	char buf[128]{};
	{
		char* it = buf;
		char* end = buf + sizeof(buf);
		it += su::u8(it, end - it, val.x);
		it += su::u8(it, end - it, ", ");
		it += su::u8(it, end - it, val.y);
		it += su::u8(it, end - it, ", ");
		it += su::u8(it, end - it, val.z);
	}

	float num_items = 3;
	float width = ImGui::GetColumnWidth(1);
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float half = width / 2 - spacing;
	float item_width = half / num_items;

	ImGui::SetNextItemWidth(half);
	if (ImGui::InputText("##Value Full", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		[&]() {
			UnityEngine::Vector3 result;
			const char* it = buf;
			const char* end = buf + sizeof(buf);
			while (*it == ' ') ++it;

			if (auto r = std::from_chars(it, end, result.x); r.ec == std::errc{})
				it = r.ptr;
			else
				return;

			while (*it == ' ') ++it;
			if (*it == ',') ++it;
			while (*it == ' ') ++it;

			if (auto r = std::from_chars(it, end, result.y); r.ec == std::errc{})
				it = r.ptr;
			else
				return;

			while (*it == ' ') ++it;
			if (*it == ',') ++it;
			while (*it == ' ') ++it;

			if (auto r = std::from_chars(it, end, result.z); r.ec == std::errc{})
				it = r.ptr;
			else
				return;

			while (*it == ' ') ++it;

			if (*it != '\0')
				return;

			changed = true;
			val = result;
		}();
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(item_width);
	if (ImGui::SliderFloat("##Value x", &val.x, -min_max, min_max))
		changed = true;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(item_width);
	if (ImGui::SliderFloat("##Value y", &val.y, -min_max, min_max))
		changed = true;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(item_width);
	if (ImGui::SliderFloat("##Value z", &val.z, -min_max, min_max))
		changed = true;

	if (changed)
		write_func(val);
}

template <typename TFunc>
static void MiniTableRow2(std::string_view name, TFunc&& func)
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text(name.data());
	ImGui::TableNextColumn();
	func();
}

template <typename TFunc>
static void MiniRow(int id, TFunc&& func)
{
	ImGui::PushID(id);
	ExitScope _([]() { ImGui::PopID(); });
	func();
}

class UnityExplorerModule : public Module
{
public:

	virtual void Update() override
	{
		if (dockspace_active_)
			return;

		if (UnityEngine::Input::GetKeyDown(UnityEngine::KeyCode::U) &&
			UnityEngine::Input::GetKey(UnityEngine::KeyCode::LeftAlt))
		{
			dockspace_active_ = true;

			if (scene_explorers_.empty())
				OpenNewSceneExplorer();
		}
	}

	virtual bool IsForceImGui() override
	{
		return dockspace_active_;
	}

	virtual void OnPostImGuiMenu() override
	{
		if (dockspace_active_ && ImGui::IsKeyDown(ImGuiKey_LeftAlt) && ImGui::IsKeyPressed(ImGuiKey_U, false))
			dockspace_active_ = false;

		if (!dockspace_active_)
			return;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Unity Explorer", nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_MenuBar);
		ImGui::PopStyleVar();

		ImGuiID dockspace_id = ImGui::GetID("Unity Explorer Dockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(), ImGuiDockNodeFlags_PassthruCentralNode);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Unity Explorer"))
			{
				if (ImGui::MenuItem("New Scene Explorer"))
					OpenNewSceneExplorer();

				if (ImGui::MenuItem("Close"))
					dockspace_active_ = false;

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		for (auto& scene_explorer : scene_explorers_)
		{
			if (!scene_explorer->three_pane)
				continue;
			if (!scene_explorer->three_pane->should_recreate)
				continue;

			auto prev_pane = std::move(scene_explorer->three_pane);

			scene_explorer->three_pane = std::make_shared<ThreePaneDock>(scene_explorer->id);

			for (auto& weak_component_explorer : prev_pane->component_explorers)
				weak_component_explorer.lock()->three_pane = scene_explorer->three_pane;

			for (auto& weak_object_explorer : prev_pane->object_explorers)
				weak_object_explorer.lock()->three_pane = scene_explorer->three_pane;

			assert(prev_pane.use_count() == 1);
		}

		bool anything_opened = false;

		auto draw_explorers = [&](auto& explorers) {
			for (auto it = explorers.begin(); it != explorers.end();)
			{
				if (!(*it)->Draw())
					it = explorers.erase(it);
				else
					++it;

				anything_opened = true;
			}
		};

		// Reset each frame.
		selected_component_ = nullptr;
		selected_object_ = nullptr;

		// Order of draw_explorers is important!
		draw_explorers(component_explorers_);
		draw_explorers(object_explorers_);
		draw_explorers(scene_explorers_);

		ImGui::End();

		if (!anything_opened)
			dockspace_active_ = false;
	}

private:

	struct BaseExplorer;
	struct ComponentExplorer;
	struct ObjectExplorer;
	struct SceneExplorer;

	struct ThreePaneDock
	{
		ImGuiID split_id = 0;
		ImGuiID left = 0;
		ImGuiID middle = 0;
		ImGuiID right = 0;

		bool should_recreate = false;

		std::vector<std::weak_ptr<ComponentExplorer>> component_explorers;
		std::vector<std::weak_ptr<ObjectExplorer>> object_explorers;

		// Only for comparison. May be non-null but invalid.
		UnityEngine::GameObject* selected_object = nullptr;
		UnityEngine::Component* selected_component = nullptr;

		explicit ThreePaneDock(int id)
		{
			split_id = ImGui::GetID(("ThreePaneDock_" + su::u8(id)).c_str());

			auto split_pos = ImVec2(100, 100);
			auto split_size = ImVec2(1200, 800);

			if (auto node = ImGui::DockBuilderGetNode(split_id))
			{
				split_pos = node->Pos;
				split_size = node->Size;
				ImGui::DockBuilderRemoveNode(split_id);
			}

			ImGui::DockBuilderAddNode(split_id);
			ImGui::DockBuilderSetNodePos(split_id, split_pos);
			ImGui::DockBuilderSetNodeSize(split_id, split_size);
			ImGuiID tmp_right = 0;
			ImGui::DockBuilderSplitNode(split_id, ImGuiDir_Left, 0.33f, &left, &tmp_right);
			ImGui::DockBuilderSplitNode(tmp_right, ImGuiDir_Left, 0.5f, &middle, &right);
			ImGui::DockBuilderFinish(split_id);
		}

		bool IsInsideSplit(ImGuiDockNode* node) const
		{
			while (node)
			{
				if (node->ID == split_id)
					return true;

				node = node->ParentNode;
			}

			return false;
		}

		template <typename T>
		void _RemoveExplorer(std::vector<std::weak_ptr<T>>& explorers, const T* to_find)
		{
			for (auto it = explorers.begin(); it != explorers.end();)
			{
				auto explorer = it->lock();

				if (explorer.get() == to_find)
					it = explorers.erase(it);
				else
					++it;
			}
		}

		void RemoveComponentExplorer(const ComponentExplorer* component_explorer)
		{
			_RemoveExplorer(component_explorers, component_explorer);
		}

		void RemoveObjectExplorer(const ObjectExplorer* object_explorer)
		{
			_RemoveExplorer(object_explorers, object_explorer);
		}
	};

	struct BaseExplorer
	{
		UnityExplorerModule* unity_explorer = nullptr;
		std::shared_ptr<ThreePaneDock> three_pane;
		std::string name;
		int id = 0;

		ThreePaneDock* last_three_pane = nullptr;

		BaseExplorer(UnityExplorerModule* _unity_explorer, const std::shared_ptr<ThreePaneDock>& _three_pane, std::string _name, int _id)
			: unity_explorer{ _unity_explorer }
			, three_pane{ _three_pane }
			, name{ std::move(_name) }
			, id{ _id }
		{
		}
	};

	struct ComponentExplorer : BaseExplorer
	{
		il2cpp::gc_ref<UnityEngine::Component> comp;
		std::string comp_name;

		ComponentExplorer(UnityExplorerModule* _unity_explorer, const std::shared_ptr<ThreePaneDock>& _three_pane, int _id, UnityEngine::Component* _comp)
			: BaseExplorer(_unity_explorer, _three_pane, std::string(_comp->GetClass()->GetName()) + "##_ComponentExplorer_" + su::u8(_id), _id)
			, comp{ _comp }
			, comp_name{ _comp->GetClass()->GetName() }
		{
		}

		~ComponentExplorer()
		{
			if (three_pane)
				three_pane->RemoveComponentExplorer(this);
		}

		bool Draw()
		{
			assert(id != 0);

			if (three_pane && three_pane.get() != last_three_pane)
			{
				last_three_pane = three_pane.get();
				ImGui::SetNextWindowDockID(three_pane->right);
			}

			bool active = true;
			ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(2000, 1000));
			ImGui::Begin(name.c_str(), &active);

			// If detached
			if (three_pane && !three_pane->IsInsideSplit(ImGui::GetCurrentWindow()->DockNode))
			{
				if (three_pane->selected_component == comp.get_obj())
					three_pane->selected_component = nullptr;

				three_pane->RemoveComponentExplorer(this);

				if (three_pane->component_explorers.empty())
					three_pane->should_recreate = true;

				three_pane = {};
			}

			if (comp)
			{
				if (ImGui::IsWindowFocused())
				{
					unity_explorer->selected_component_ = comp.get_obj();

					if (three_pane)
						three_pane->selected_component = comp.get_obj();
				}

				if (ImGui::BeginTable("Component Info", 2, ImGuiTableFlags_Resizable))
				{
					auto add_row = [](std::string_view name, const void* id = nullptr) {
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::AlignTextToFramePadding();
						if (id) ImGui::PushID(id);
						ImGui::Text(name.data());
						if (id) ImGui::PopID();
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(-FLT_MIN);
					};

					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 200.0f);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 50.0f);

					add_row("Name");
					ImGui::InputText("##Name", (char*)comp_name.c_str(), comp_name.size() + 1, ImGuiInputTextFlags_ReadOnly);

					for (const auto& field : comp->GetClass()->GetFields())
					{
						add_row(field.GetName(), &field);
					}

					for (const auto& prop : comp->GetClass()->GetProperties())
					{
						add_row(prop.GetName(), &prop);
					}

					for (const auto& method : comp->GetClass()->GetMethods())
					{
						add_row(method->GetName(), method);
					}

					for (const auto& event : comp->GetClass()->GetEvents())
					{
						add_row(event.GetName(), &event);
					}

					ImGui::EndTable();
				}
			}
			else
			{
				ImGui::Text("Component Destroyed");
				comp = {};
			}

			ImGui::End();

			return active;
		}
	};

	struct ObjectExplorer : BaseExplorer
	{
		il2cpp::gc_ref<UnityEngine::GameObject> obj;
		std::vector<char> obj_name_cache;
		std::string obj_full_path_cache;

		UnityEngine::Transform* transform = nullptr;

		ObjectExplorer(UnityExplorerModule* _unity_explorer, const std::shared_ptr<ThreePaneDock>& _three_pane, int _id, UnityEngine::GameObject* _obj)
			: BaseExplorer(_unity_explorer, _three_pane, _obj->get_name()->AsString() + "##_ObjectExplorer_" + su::u8(_id), _id)
			, obj{ _obj }
			, transform{ _obj->get_transform() }
		{
		}

		~ObjectExplorer()
		{
			if (three_pane)
				three_pane->RemoveObjectExplorer(this);
		}

		bool Draw()
		{
			assert(id != 0);

			if (three_pane && three_pane.get() != last_three_pane)
			{
				last_three_pane = three_pane.get();
				ImGui::SetNextWindowDockID(three_pane->middle);
			}

			bool active = true;
			ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(2000, 1000));
			ImGui::Begin(name.c_str(), &active);

			// If detached
			if (three_pane && !three_pane->IsInsideSplit(ImGui::GetCurrentWindow()->DockNode))
			{
				if (three_pane->selected_object == obj.get_obj())
					three_pane->selected_object = nullptr;

				three_pane->RemoveObjectExplorer(this);

				if (three_pane->object_explorers.empty())
					three_pane->should_recreate = true;

				three_pane = {};
			}

			if (obj)
			{
				if (ImGui::IsWindowFocused())
				{
					unity_explorer->selected_object_ = obj.get_obj();

					if (three_pane)
						three_pane->selected_object = obj.get_obj();
				}

				if (ImGui::BeginTable("Object Info", 2, ImGuiTableFlags_Resizable))
				{
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 150.0f);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 50.0f);

					MiniRow(1, [&]() {
						MiniTableRow2("Name", [&]() {
							auto read_func = [&]() { return obj->get_name()->AsU16StringView(); };
							auto write_func = [&](const auto& new_str) { obj->set_name(System::String::New(new_str)); };
							MiniFieldText(read_func, write_func);
						});
					});

					MiniRow(2, [&]() {
						MiniTableRow2("Full Path", [&]() {
							auto read_func = [&]() { return std::string_view(GetFullPath()); };
							auto write_func = nullptr;
							MiniFieldText(read_func, write_func);
						});
					});

					MiniRow(3, [&]() {
						MiniTableRow2("Position", [&]() {
							auto read_func = [&]() { return transform->get_position(); };
							auto write_func = [&](const auto& pos) { transform->set_position(pos); };
							MiniFieldSlider3(100, read_func, write_func);
						});
					});
					MiniRow(4, [&]() {
						MiniTableRow2("Local Position", [&]() {
							auto read_func = [&]() { return transform->get_localPosition(); };
							auto write_func = [&](const auto& pos) { transform->set_localPosition(pos); };
							MiniFieldSlider3(10, read_func, write_func);
						});
					});
					MiniRow(5, [&]() {
						MiniTableRow2("Local Angles", [&]() {
							auto read_func = [&]() { return transform->get_localEulerAngles(); };
							auto write_func = [&](const auto& pos) { transform->set_localEulerAngles(pos); };
							MiniFieldSlider3(360, read_func, write_func);
						});
					});
					MiniRow(6, [&]() {
						MiniTableRow2("Local Scale", [&]() {
							auto read_func = [&]() { return transform->get_localScale(); };
							auto write_func = [&](const auto& pos) { transform->set_localScale(pos); };
							MiniFieldSlider3(10, read_func, write_func);
						});
					});

					ImGui::EndTable();

#if UC_UNITY_VERSION_NUM >= 2022308945
					for (auto comp : obj->GetComponentsView())
#else
					for (auto comp : *obj->GetComponents())
#endif // UC_UNITY_VERSION_NUM >= 2022308945
					{
						if (ImGui::Button(comp->GetClass()->GetName().data()))
						{
							auto& component_explorer = unity_explorer->OpenNewComponentExplorer(three_pane, comp);

							if (!three_pane)
							{
								auto object_window = ImGui::GetCurrentWindow();

								if (!TrySplitOrMergeWindow(object_window, component_explorer->name, "_ComponentExplorer_"))
									SplitWindowInNewDock(object_window, component_explorer->name);
							}
						}
					}
				}
			}
			else
			{
				ImGui::Text("Object Destroyed");
				obj = {};
			}

			ImGui::End();

			return active;
		}

		std::string& GetFullPath()
		{
			obj_full_path_cache.clear();

			auto tmp = obj.get_obj();

			while (tmp)
			{
				obj_full_path_cache = tmp->get_name()->AsString() + std::move(obj_full_path_cache);
				obj_full_path_cache = "/" + std::move(obj_full_path_cache);

				if (auto parent = tmp->get_transform()->get_parent())
					tmp = parent->get_gameObject();
				else
					break;
			}

			return obj_full_path_cache;
		}
	};

	struct SceneExplorer : BaseExplorer
	{
		std::string selected_scene;

		explicit SceneExplorer(UnityExplorerModule* _unity_explorer, const std::shared_ptr<ThreePaneDock>& _three_pane, int _id)
			: BaseExplorer(_unity_explorer, _three_pane, "Scene Explorer##" + su::u8(_id), _id)
		{
			System::Int32 num_scenes = UnityEngine::SceneManagement::SceneManager::get_sceneCount();

			if (num_scenes > 0)
			{
				auto scene = UnityEngine::SceneManagement::SceneManager::GetSceneAt(0);
				selected_scene = scene.get_name()->AsString();
			}
		}

		bool Draw()
		{
			assert(id != 0);

			if (!three_pane)
				three_pane = std::make_shared<ThreePaneDock>(id);

			if (three_pane && three_pane.get() != last_three_pane)
			{
				last_three_pane = three_pane.get();
				ImGui::SetNextWindowDockID(three_pane->left);
			}

			bool active = true;
			ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(2000, 1000));
			ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_MenuBar);

			if (!three_pane->IsInsideSplit(ImGui::GetCurrentWindow()->DockNode))
				three_pane = std::make_shared<ThreePaneDock>(id);

			if (ImGui::BeginMenuBar())
			{
				if (!selected_scene.empty())
					if (!UnityEngine::SceneManagement::SceneManager::GetSceneByName(System::String::New(selected_scene)).IsValid())
						selected_scene = {};

				if (ImGui::BeginMenu("Scenes"))
				{
					System::Int32 num_scenes = UnityEngine::SceneManagement::SceneManager::get_sceneCount();
					for (System::Int32 i = 0; i < num_scenes; ++i)
					{
						auto scene = UnityEngine::SceneManagement::SceneManager::GetSceneAt(i);
						auto scene_name = scene.get_name();
						auto scene_name_str = scene_name->AsString();

						if (ImGui::MenuItem(scene_name_str.c_str(), nullptr, selected_scene == scene_name_str))
							selected_scene = scene_name_str;
					}

					ImGui::EndMenu();
				}

				if (!selected_scene.empty())
					ImGui::BeginMenu(selected_scene.c_str(), false);

				ImGui::EndMenuBar();
			}

			if (selected_scene.empty())
			{
				ImGui::Text("Select Scene");
			}
			else
			{
				auto scene = UnityEngine::SceneManagement::SceneManager::GetSceneByName(System::String::New(selected_scene));

				for (auto obj : *scene.GetRootGameObjects())
					DrawObject(obj);
			}

			ImGui::End();

			return active;
		}

		void DrawObject(UnityEngine::GameObject* obj)
		{
			ImGuiTreeNodeFlags node_flags = 0;

			auto transform = obj->get_transform();
			auto child_count = transform->get_childCount();

			if (obj == unity_explorer->selected_object_)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			
			if (obj == three_pane->selected_object)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			if (child_count == 0)
				node_flags |= ImGuiTreeNodeFlags_Leaf;

			bool checked = obj->get_activeSelf();
			ImGui::PushID((int32_t)(intptr_t)obj);
			if (ImGui::Checkbox("##checkbox", &checked))
				obj->SetActive(checked);
			ImGui::PopID();

			ImGui::SameLine();

			bool node_opened = ImGui::TreeNodeEx(obj, node_flags, "%s", obj->get_name()->AsString().c_str());

			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				auto& object_explorer = unity_explorer->OpenNewObjectExplorer(three_pane, obj);

				auto scene_window = ImGui::GetCurrentWindow();
				if (!TrySplitOrMergeWindow(scene_window, object_explorer->name, "_ObjectExplorer_"))
					SplitWindowInNewDock(scene_window, object_explorer->name);
			}

			if (node_opened)
			{
				if (child_count > 0)
					for (auto child : transform->GetGameObjectsView())
						DrawObject(child);

				ImGui::TreePop();
			}
		}
	};

	template <typename T> requires std::is_base_of_v<BaseExplorer, T>
	int FindEmptyID(const std::vector<std::shared_ptr<T>>& explorers)
	{
		int new_id = 1;

		for (int i = 0; i < explorers.size(); ++i, ++new_id)
		{
			auto it = std::ranges::find(explorers, new_id, [](const auto& exp) { return exp->id; });

			if (it == explorers.end())
				break;
		}

		return new_id;
	}

	template <typename T, typename... TArgs> requires std::is_base_of_v<BaseExplorer, T>
	std::shared_ptr<T>& OpenNewExplorer(const std::shared_ptr<ThreePaneDock>& _three_pane, std::vector<std::shared_ptr<T>>& explorers, TArgs&&... args)
	{
		int new_id = FindEmptyID(explorers);
		return explorers.emplace_back(std::make_shared<T>(this, _three_pane, new_id, std::forward<TArgs>(args)...));
	}

	std::shared_ptr<ComponentExplorer>& OpenNewComponentExplorer(const std::shared_ptr<ThreePaneDock>& _three_pane, UnityEngine::Component* comp)
	{
		auto& explorer = OpenNewExplorer(_three_pane, component_explorers_, comp);

		if (_three_pane)
			_three_pane->component_explorers.push_back(explorer);

		return explorer;
	}
	std::shared_ptr<ObjectExplorer>& OpenNewObjectExplorer(const std::shared_ptr<ThreePaneDock>& _three_pane, UnityEngine::GameObject* obj)
	{
		auto& explorer = OpenNewExplorer(_three_pane, object_explorers_, obj);

		if (_three_pane)
			_three_pane->object_explorers.push_back(explorer);

		return explorer;
	}
	std::shared_ptr<SceneExplorer>& OpenNewSceneExplorer()
	{
		return OpenNewExplorer(nullptr, scene_explorers_);
	}

	std::vector<std::shared_ptr<ComponentExplorer>> component_explorers_;
	std::vector<std::shared_ptr<ObjectExplorer>> object_explorers_;
	std::vector<std::shared_ptr<SceneExplorer>> scene_explorers_;

	bool dockspace_active_ = false;

	// Only for comparison. May be non-null but invalid.
	UnityEngine::GameObject* selected_object_ = nullptr;
	UnityEngine::Component* selected_component_ = nullptr;
};

static RegisterModuleStatic<UnityExplorerModule> registered;

#endif // UC_ENABLE_IMGUI
