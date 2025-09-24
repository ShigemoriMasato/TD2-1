#pragma once
#include <cstdint>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <Core/DXDevice.h>
#include <Resource/SRVManager.h>

class Render;

class ImGuiRapper {
public:

	ImGuiRapper() = default;
	~ImGuiRapper();
	void Initialize(DXDevice* device, Render* render, SRVManager* srv);
	void StartFrame(float kClientWidth, float kClientHeight);
	void EndFrame(ID3D12GraphicsCommandList* commandList);

private:

};

