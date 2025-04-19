#include "MyDirectX.h"
#include "Logger.h"
#include <Windows.h>
#include <cstdint>
#include <cassert>
#include <strsafe.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dbghelp.h>
#include <dxgidebug.h>
#include <memory>
#include <dxcapi.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

struct Vector4 {
    float w;
    float x;
    float y;
    float z;
};

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

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
    //取得した時間を名前にしたファイルを作成し、そのなかにDumpを出力する
    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t filePath[MAX_PATH] = { 0 };
    CreateDirectory(L"./Dumps", nullptr);
    StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
    HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    //processID(このexeのID)とクラッシュ(例外)の発生したthreadIDを取得
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();
    //設定情報を入力
    MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
    minidumpInformation.ThreadId = threadId;
    minidumpInformation.ExceptionPointers = exception;
    minidumpInformation.ClientPointers = TRUE;
    //Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
    MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
    //他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
    return EXCEPTION_EXECUTE_HANDLER;
}

IDxcBlob* CompileShader(
    //CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    //Compilerに使用するprofile
    const wchar_t* profile,
    //初期化で生成したものを3つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler,
    std::shared_ptr<Logger> logger) {

	logger->Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", filePath, profile)));

    //hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    //読めなかったら止める
    assert(SUCCEEDED(hr));
    //読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;//utf8の文字コードであることを通知

    LPCWSTR arguments[] = {
        filePath.c_str(),	//コンパイル対象のhlslファイル名
        L"-E", L"main",     //エントリーポイントの指定。基本的にmain以外には市内
        L"-T", profile,    //ShaderProfileの設定
        L"-Zi", L"-Qembed_debug", //デバッグ用の情報を埋め込む
        L"-Od",     //最適化を行わない
        L"-Zpr",     //メモリレイアウトは行優先
    };
    //実際にShaderをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,	//読み込んだファイル
        arguments,			    //コンパイルオプション
        _countof(arguments),	//コンパイルオプションの数
        includeHandler,		    //includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult)		//コンパイル結果
    );
    //コンパイルエラーではなくdxcが起動できないなどの致命的な状況
    assert(SUCCEEDED(hr));

    //警告・エラーが出てたらログに出して止める
    IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		logger->Log(shaderError->GetStringPointer());
        //警告・エラーが起きている状態なので止める
        assert(false);
    }

    //コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
    //成功したログを出す
	logger->Log(ConvertString(std::format(L"Compile Successd, path: {}, profile{}\n", filePath, profile)));
    //もう使わないリソースを開放
    shaderSource->Release();
    shaderResult->Release();
    //実行用のバイナリを返却
    return shaderBlob;
}

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MyDirectX* myDirectX = new MyDirectX(1280, 720);
	myDirectX->Initialize();

	MSG msg{};

	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//ゲームの更新処理
			myDirectX->ClearScreen();
			myDirectX->DrawTriangle();
			myDirectX->EndFrame();
		}
	}

    logger->Log("Finalize DirectX\n");

    vertexResource->Release();
    graphicsPipelineState->Release();
    signatureBlob->Release();
    if (errorBlob) {
        errorBlob->Release();
    }
    rootSignature->Release();
    pixelShaderBlob->Release();
    vertexShaderBlob->Release();
    CloseHandle(fenceEvent);
    fence->Release();
    rtvDescriptorHeap->Release();
    swapChainResources[0]->Release();
    swapChainResources[1]->Release();
    swapChain->Release();
    commandList->Release();
    commandAllocator->Release();
    commandQueue->Release();
    device->Release();
    useAdapter->Release();
    dxgiFactory->Release();
#ifdef _DEBUG
    debugController->Release();
#endif

    logger->Log("Complete Finalize DirectX\n");

    IDXGIDebug* debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        debug->Release();
    }

	myDirectX->Finalize();
	delete myDirectX;

	return 0;
}
