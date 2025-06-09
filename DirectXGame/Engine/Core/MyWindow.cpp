#include "MyWindow.h"
#include "../Render/MyDirectX.h"

std::vector<HWND> MyWindow::hwndList_;
std::vector<WNDCLASS> MyWindow::wcList_;

MyWindow::MyWindow(const int32_t kClientWidth, const int32_t kClientHeight) :
kClientHeight_(kClientHeight),
kClientWidth_(kClientWidth) {
}

MyWindow::~MyWindow() {
    for(int i = 0; i < hwndList_.size(); ++i) {
        //ウィンドウを破棄
        CloseWindow(hwndList_[i]);
	}
}

HWND MyWindow::GetHwnd(int handle) {
    if(handle < 0 || handle >= hwndList_.size()) {
        return nullptr; //無効なハンドル
	}

	return hwndList_[handle]; //有効なハンドルを返す
}

WNDCLASS MyWindow::GetWndClass(int handle) {
    if(handle < 0 || handle >= wcList_.size()) {
        return {}; //無効なハンドル
	}

	return wcList_[handle]; //ウィンドウクラスを返す
}

int MyWindow::CreateWindowForApp() {
    WNDCLASS wc{};
    //ウィンドウプロシージャ
    wc.lpfnWndProc = WindowProc;
    //ウィンドウクラス名
    wc.lpszClassName = L"CG2WindowClass";
    //インスタンスハンドル
    wc.hInstance = GetModuleHandleA(nullptr);
    //カーソル
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //ウィンドウクラスの登録
    RegisterClass(&wc);

    //ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0, 0, kClientWidth_, kClientHeight_ };

    //クライアント領域をもとに実際のサイズにwrcに変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    //ウィンドウの作成
    HWND hwnd = CreateWindow(
        wc.lpszClassName,			//利用するクラスの名前
        L"CG2",						//タイトルバーの文字
        WS_OVERLAPPEDWINDOW,		//よく見るウィンドウスタイル
        CW_USEDEFAULT,				//表示x座標
        CW_USEDEFAULT,				//表示y座標
        wrc.right - wrc.left,		//ウィンドウ幅
        wrc.bottom - wrc.top,		//ウィンドウ高さ
        nullptr,					//親ウィンドウハンドル
        nullptr,					//メニューハンドル
        wc.hInstance,				//インスタンスハンドル
        nullptr);					//オプション

    ShowWindow(hwnd, SW_SHOW);	    //ウィンドウを表示する

	hwndList_.push_back(hwnd); //ウィンドウハンドルを保存
	wcList_.push_back(wc); //ウィンドウクラスを保存

	return int(hwndList_.size() - 1); //ウィンドウハンドルのインデックスを返す
}

LRESULT MyWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    //imguiのウィンドウプロシージャを呼ぶ
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }

    //メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
    case WM_DESTROY:
        //アプリを落とす
        PostQuitMessage(0);
        return 0;
    }

    //標準のメッセージ
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
