#include "MySetup.h"
#include <cstdint>
#include <cassert>
#include <memory>

//ウィンドウプロシージャ
LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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

MySetup::MySetup() : log_(std::make_unique<Logger>("master")) {
}

void MySetup::Initialize() {
	SamonWindow();	        //ウィンドウ作成
	InitializeDirectX();	//DirectX初期化
}

//==========================================以下private=============================================\\

void MySetup::SamonWindow() {

    WNDCLASS wc{};
    //ウィンドウプロシージャ
    wc.lpfnWndProc = windowProc;
    //ウィンドウクラス名
    wc.lpszClassName = L"CG2WindowClass";
    //インスタンスハンドル
    wc.hInstance = GetModuleHandleA(nullptr);
    //カーソル
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //ウィンドウクラスの登録
    RegisterClass(&wc);

    const int32_t kClientwidth = 1280;
    const int32_t kClientheight = 720;

    //ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0, 0, kClientwidth, kClientheight };

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

    ShowWindow(hwnd, SW_SHOW);	//ウィンドウを表示する

}

void MySetup::InitializeDirectX() {
    //DXGIファクトリーの作成
    IDXGIFactory7 *dxgiFactory = nullptr;
    //HRESULTはWindows系のエラーコードであり、
    //関数が成功したかどうかをSUCCEEDEDマクロで判定する
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    //初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    //どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr));
    //使用するアダプタ用の変数。
    IDXGIAdapter4 *useAdapter = nullptr;
    //一番いいアダプタを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
        //アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDisc{};
        hr = useAdapter->GetDesc3(&adapterDisc);
        //取得できなければエラー
        assert(SUCCEEDED(hr));
        //ソフトウェアアダプタでなければ採用
        if (!(adapterDisc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            //採用したアダプタの情報をログに出力
            log_->Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDisc.Description)));

            break;
        }
        //ソフトウェアアダプタの場合はなかったことに
        useAdapter = nullptr;
    }

    //適切なアダプタが見つからなかった場合は起動できない
    assert(useAdapter != nullptr);

    ID3D12Device *device = nullptr;
    //機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };
    std::string featureLevelStrings[] = { "12.2","12.1","12.0" };
    //高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        //採用したアダプタでデバイスを生成
        hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
        //指定した機能レベルでデバイスが生成できたか確認
        if (SUCCEEDED(hr)) {
            //生成できたのでログを出力してループを抜ける
            log_->Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
            break;
        }
    }

    //デバイスの生成が上手くいかなかったので起動できない
    assert(device != nullptr);
    log_->Log("Complete create D3D12Device\n");

}
