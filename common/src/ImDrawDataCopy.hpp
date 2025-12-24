#pragma once

#include "common/LockedValue.hpp"

#include "imgui.h"

class ImDrawDataCopy
{
public:

	void CopyFrom(ImDrawData* data)
	{
		std::unique_lock lock(mtx_);

		draw_data_.Valid = data->Valid;
		draw_data_.CmdListsCount = data->CmdListsCount;
		draw_data_.TotalIdxCount = data->TotalIdxCount;
		draw_data_.TotalVtxCount = data->TotalVtxCount;
		// draw_data_.CmdLists = data->CmdLists;
		draw_data_.DisplayPos = data->DisplayPos;
		draw_data_.DisplaySize = data->DisplaySize;
		draw_data_.FramebufferScale = data->FramebufferScale;
		draw_data_.OwnerViewport = data->OwnerViewport;
		draw_data_.Textures = data->Textures;

		draw_data_.CmdLists.resize(data->CmdLists.size());

		if (draw_list_.size() < data->CmdLists.size())
			draw_list_.resize(data->CmdLists.size(), ImDrawList(nullptr));

		for (int i = 0; i < data->CmdLists.size(); ++i)
		{
			auto from = data->CmdLists[i];
			auto to = &draw_list_[i];

			draw_data_.CmdLists[i] = to;

			to->CmdBuffer.resize(from->CmdBuffer.size());
			std::ranges::copy(from->CmdBuffer, to->CmdBuffer.begin());

			to->IdxBuffer.resize(from->IdxBuffer.size());
			std::ranges::copy(from->IdxBuffer, to->IdxBuffer.begin());

			to->VtxBuffer.resize(from->VtxBuffer.size());
			std::ranges::copy(from->VtxBuffer, to->VtxBuffer.begin());

			to->Flags = from->Flags;
		}
	}

	LockedValue<ImDrawData> Locked() { return LockedValue<ImDrawData>(mtx_, draw_data_); }

private:

	ImDrawData draw_data_{};
	ImVector<ImDrawList> draw_list_{};
	std::mutex mtx_;
};
