#include "MyEngine.h"
#include "MyDirectX.h"
#include "Audio.h"
#include "Input.h"

void MyEngine::Initialize(const float kWindowWidth, const float kWindowHeight) {
	myDirectX = new MyDirectX(int(kWindowWidth), int(kWindowHeight));
	audio = new Audio();
	input = new Input();
}

void MyEngine::BeginFrame() {
	if (!myDirectX) {
		assert(false && "MyDirectX is not initialized. Call MyEngine::Initialize() first.");
	}

	input->Update();
}

void MyEngine::PreDraw() {
	if (!myDirectX) {
		assert(false && "MyDirectX is not initialized. Call MyEngine::Initialize() first.");
	}
	myDirectX->BeginFrame();
	isColledPreDraw = true;
}

void MyEngine::PostDraw() {
	if (!myDirectX || isColledPreDraw) {
		assert(false && "MyDirectX is not PreDraw. Call MyEngine::PreDraw() first.");
	}
	myDirectX->EndFrame();
	isColledPreDraw = false;
}
