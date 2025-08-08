#include "ShaderShelf.h"
#include <cassert>

namespace {

    IDxcBlob* CompileShader(
        //CompilerするShaderファイルへのパス
        const std::wstring& filePath,
        //Compilerに使用するprofile
        const wchar_t* profile,
        //初期化で生成したものを3つ
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler,
        Logger* logger) {

        logger->Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}", filePath, profile)));

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
        logger->Log(ConvertString(std::format(L"Compile Successed, path: {}, profile: {}\n", filePath, profile)));
        //もう使わないリソースを開放
        shaderSource->Release();
        shaderResult->Release();
        //実行用のバイナリを返却
        return shaderBlob;
    }

}

ShaderShelf::ShaderShelf() {

    //dxcCompilerを初期化
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    //includeに対応するための設定を行っておく
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));

    //InputLayout
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

	logger_ = std::make_unique<Logger>();
    logger_->SetLogStreamHandle((logger_->RegistLogFile("Shader")));

	shaderVersions_[0] = L"vs_6_0"; // Vertex Shader
    shaderVersions_[1] = L"ps_6_0"; // Pixel Shader
}

ShaderShelf::~ShaderShelf() {
    for(int i = 0; i < int(ShaderType::Count); ++i) {
        for(const auto& shader : shaderNames_[i]) {
            if (shader.second != nullptr) {
                shader.second->Release();
            }
        }
        shaderNames_[i].clear();
	}
}

void ShaderShelf::RegisterShader(const std::string& name, ShaderType shaderType) {
    int type = int(shaderType);
    for(const auto& shader : shaderNames_[type]) {
        if (shader.first == name) {
            // 既に登録されている場合は何もしない
            return;
        }
	}

    IDxcBlob* shaderBlob = CompileShader(ConvertString(basePath_ + name + ".hlsl"),
        shaderVersions_[int(shaderType)].c_str(), dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get(), logger_.get());

    assert(shaderBlob != nullptr);

	shaderNames_[type][name] = shaderBlob;
}

D3D12_INPUT_LAYOUT_DESC ShaderShelf::GetInputLayoutDesc() const {
    return inputLayoutDesc;
}

IDxcBlob* ShaderShelf::GetShaderBlob(const std::string& name, ShaderType shaderType) const {
    if(shaderNames_[int(shaderType)].find(name) != shaderNames_[int(shaderType)].end()) {
        return shaderNames_[int(shaderType)].at(name);
	}

    // 見つからなかった場合はnullptrを返す
	return nullptr;
}
