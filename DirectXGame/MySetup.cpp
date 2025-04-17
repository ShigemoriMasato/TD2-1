#include "MySetup.h"
#include <memory>

namespace {

}

MySetup::MySetup(int32_t windowWidth, int32_t windowHeight) : masterLog(std::make_unique<Logger>("master")),
    kClientwidth(windowWidth), kClientheight(windowHeight), dx(std::make_unique<MyDirectX>()) {}

void MySetup::Initialize() {

	SamonWindow();	        //ウィンドウ作成

	dx->Initialize();	//DirectX初期化
    
	dx->ClearWindow(hwnd, kClientwidth, kClientheight);	//ウィンドウをクリアする

}

void MySetup::Finalize() {
	masterLog->Log("Start Finalize");
    dx->Finalize();
    dx->ReleaseChecker();
    masterLog->Log("Comlpeted\n");
}

//==========================================以下private=============================================\\

void MySetup::SamonWindow() {

}
