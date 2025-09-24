#include "ImGuiWrapper.h"
#include <imgui/imgui_impl_win32.h>
#include <Render/Render.h>

ImGuiRapper::~ImGuiRapper() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiRapper::Initialize(DXDevice* device, Render* render, SRVManager* srv) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(device->GetHwnd());
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // ドッキングを有効化

	ImGui_ImplDX12_InitInfo initInfo = render->GetImGuiInitInfo(srv);

    ImGui_ImplDX12_Init(&initInfo);
}

void ImGuiRapper::StartFrame(float kClientWidth, float kClientHeight) {
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(float(kClientWidth), float(kClientHeight)), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // 角丸をなくす

    ImGui::Begin("DockSpaceWindow", nullptr, ImGuiWindowFlags_NoTitleBar | // タイトルバーなし 
        ImGuiWindowFlags_NoResize | // リサイズ不可
        ImGuiWindowFlags_NoMove | // 移動不可
        ImGuiWindowFlags_NoScrollbar | // スクロールバーなし
        ImGuiWindowFlags_NoCollapse | // 折り畳み不可
        ImGuiWindowFlags_NoBackground | // 背景なし（必要に応じて）
        ImGuiWindowFlags_NoSavedSettings);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void ImGuiRapper::EndFrame(ID3D12GraphicsCommandList* commandList) {

#ifdef _DEBUG

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#else

    ImGui::EndFrame();

#endif

}
