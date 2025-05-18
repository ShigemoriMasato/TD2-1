#include "MyDirectX.h"
#include "MyMath.h"
#include "externals/imgui/imgui.h"
#include "Prism.h"
#include "Center.h"
#include "Rain.h"
#include "Circle.h"
#include "Field.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	const float kWindowWidth = 1280.0f;
	const float kWindowHeight = 720.0f;

	const float pie = 3.14159265358f;

	MyDirectX* myDirectX = new MyDirectX(int(kWindowWidth), int(kWindowHeight));
	myDirectX->CreateDrawResource(MyDirectX::kPrism, 83);
	myDirectX->CreateDrawResource(MyDirectX::kSphere, 1);
	myDirectX->CreateDrawResource(MyDirectX::kSprite3D, 316);//雨 + サークル
	myDirectX->LoadTexture("resources/white1x1.png");

	const Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

	Transform camera = { 1.0f, 1.0f, 1.0f, 0.45f, 0.0f, 0.0f, 0.0f, 4.5f, -10.0f };
	Matrix4x4* vpMatrix = new Matrix4x4();

	DirectionalLightData* dLightData = new DirectionalLightData();
	dLightData->direction = { 0.0f, -1.0f, 0.0f };
	dLightData->intensity = 1.0f;
	dLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	Prism* prism = new Prism();
	prism->Initialize(vpMatrix, dLightData, {0.140f, 0.0f, 0.176f, 1.0f}, 2.0f, 32);
	Prism* prism2 = new Prism();
	prism2->Initialize(vpMatrix, dLightData, {0.02f, 0.018f, 0.32f, 1.0f}, 1.5f, 32);

	Prism* arround = new Prism();
	arround->Initialize(vpMatrix, dLightData, { 0.2f, 0.2f, 0.2f, 1.0f }, 4.0f, 16);

	Center* center = new Center();
	center->Initialize(vpMatrix, dLightData);

	RainEmitter* rain = new RainEmitter();
	rain->Initialize(vpMatrix, dLightData);

	Field* field = new Field();
	field->Initialize(vpMatrix, dLightData);

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

			ImGui::Begin("Camera");
			ImGui::SliderFloat3("Position", &camera.position.x, -10.0f, 10.0f);
			ImGui::SliderFloat3("Rotation", &camera.rotation.x, -3.14f, 3.14f);
			ImGui::SliderFloat3("LightDirection", &dLightData->direction.x, -1.0f, 1.0f);
			ImGui::SliderFloat("LightIntensity", &dLightData->intensity, 0.0f, 10.0f);
			ImGui::ColorEdit4("LightColor", &dLightData->color.x);
			ImGui::End();

			Normalize(dLightData->direction);

			*vpMatrix = Inverse(MakeTransformMatrix(camera)) * projectionMatrix;
			
			prism->Update(Prism::kround);
			prism2->Update(Prism::kinner);
			arround->Update(Prism::kouter);
			center->Update();
			rain->Update(camera);
			field->Update();

			prism->Draw(myDirectX);
			prism2->Draw(myDirectX);
			arround->Draw(myDirectX);
			center->Draw(myDirectX);
			rain->Draw(myDirectX);
			field->Draw(myDirectX);

			myDirectX->EndFrame();
		}
	}

	delete prism;
	delete prism2;
	delete arround;
	delete center;
	delete rain;
	delete field;
	delete myDirectX;

	return 0;
}
