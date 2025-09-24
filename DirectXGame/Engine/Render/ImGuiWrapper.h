#pragma once
#include <cstdint>

class ImGuiRapper {
public:

	ImGuiRapper() = default;
	~ImGuiRapper() = default;
	void Initialize(void* hwnd, void* device, void* commandQueue, int32_t width, int32_t height);
	void StartFrame();
	void EndFrame();

private:

};

