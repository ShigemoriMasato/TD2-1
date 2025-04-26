#include "MyDirectX.h"
#include "MyMath.h"
#include "externals/imgui/imgui.h"

namespace {
	void input(float* buffer, Vector3 vec) {
		buffer[0] = vec.x;
		buffer[1] = vec.y;
		buffer[2] = vec.z;
	}

	void input(float* buffer, Vector4 vec) {
		buffer[0] = vec.x;
		buffer[1] = vec.y;
		buffer[2] = vec.z;
		buffer[3] = vec.w;
	}

	void output(float buffer[3], Vector3& vec) {
		vec.x = buffer[0];
		vec.y = buffer[1];
		vec.z = buffer[2];
	}

	void output(float buffer[4], Vector4& vec) {
		vec.x = buffer[0];
		vec.y = buffer[1];
		vec.z = buffer[2];
		vec.w = buffer[3];
	}
}

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	const float kWindowWidth = 1280.0f;
	const float kWindowHeight = 720.0f;

	MyDirectX* myDirectX = new MyDirectX(int(kWindowWidth), int(kWindowHeight));
	myDirectX->Initialize();

	Transform transform = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Transform camera = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -5.0f };
	Matrix4x4 wvpMatrix;
	float imguiBuffer[3];
	float imguiBuffer2[4];
	Vector4 triangleColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	MSG msg{};

	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//ゲームの更新処理
			myDirectX->BeginFrame();

			transform.rotation.y += 0.01f;
			if (transform.rotation.y > 3.14) {
				transform.rotation.y -= 6.28f;
			}

			//windowの大きさ(てきとうに)
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::Begin("Triangle");

			input(imguiBuffer, transform.scale);
			ImGui::SliderFloat3("Scale", imguiBuffer, 0.0f, 10.0f);
			output(imguiBuffer, transform.scale);

			input(imguiBuffer, transform.rotation);
			ImGui::SliderFloat3("Rotation", imguiBuffer, -3.14f, 3.14f);
			output(imguiBuffer, transform.rotation);

			input(imguiBuffer, transform.position);
			ImGui::SliderFloat3("Position", imguiBuffer, -5.0f, 5.0f);
			output(imguiBuffer, transform.position);

			input(imguiBuffer2, triangleColor);
			ImGui::ColorPicker4("Color", imguiBuffer2);
			output(imguiBuffer2, triangleColor);

			ImGui::End();

			Matrix4x4 worldMatrix = MakeTransformMatrix(transform);
			Matrix4x4 cameraMatrix = MakeTransformMatrix(camera);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
			wvpMatrix = worldMatrix * viewMatrix * projectionMatrix;

			myDirectX->DrawTriangle(wvpMatrix, triangleColor);

			myDirectX->EndFrame();
		}
	}

	myDirectX->Finalize();
	delete myDirectX;

	return 0;
}
