#include "RootSignatureShelf.h"
#include <cassert>

RootSignatureShelf::RootSignatureShelf(ID3D12Device* device) {

	logger_ = std::make_unique<Logger>();
	logger_->RegistLogFile("RootSignature");

	rootSignatures_.resize(int(RootSignatureID::Count));

#pragma region DescriptorTable

	//PIXELShader,Register0
    D3D12_DESCRIPTOR_RANGE textureDescriptor[1] = {};
    textureDescriptor[0].BaseShaderRegister = 0;
    textureDescriptor[0].NumDescriptors = 1;
    textureDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    textureDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//VertexShader,Register0
	D3D12_DESCRIPTOR_RANGE instancingDescriptor[1] = {};
    instancingDescriptor[0].BaseShaderRegister = 0;
    instancingDescriptor[0].NumDescriptors = 1;
    instancingDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    instancingDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//VertexShader,Register1
    D3D12_DESCRIPTOR_RANGE matrixDescriptor[1] = {};
    matrixDescriptor[0].BaseShaderRegister = 1;
    matrixDescriptor[0].NumDescriptors = 1;
    matrixDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	matrixDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

    D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
    staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//フィルタリングの方法
    staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//比較しない
    staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;    //ありったけのMipmapを使う
    staticSampler[0].ShaderRegister = 0;    //レジスタ番号0
    staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShaderで使う

    //ありあり
    {
        //RootSignature作成
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        //RootParameter作成
        D3D12_ROOT_PARAMETER rootParameters[4] = {};

        //Material
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;     //PixelShaderで使う
        rootParameters[0].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド

        //Matrix
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;     //VertexShaderで使う
        rootParameters[1].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド

        //Texture
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//テーブルを使う
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
        rootParameters[2].DescriptorTable.pDescriptorRanges = textureDescriptor;	//テーブルの中身
        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(textureDescriptor);	//テーブルの数

        //DirectionalLight
        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//CBVを使う
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
        rootParameters[3].Descriptor.ShaderRegister = 1;	//レジスタ番号1とバインド

        descriptionRootSignature.pParameters = rootParameters;                  //ルートパラメータ配列へのポインタ
        descriptionRootSignature.NumParameters = _countof(rootParameters);      //配列の長さ

        descriptionRootSignature.pStaticSamplers = staticSampler;              //StaticSamplerの配列へのポインタ
        descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);   //配列の長さ

        CreateRootSignature(descriptionRootSignature, RootSignatureID::Default, device);
    }

    //NonMatrix
    {
        //RootSignature作成
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        //RootParameter作成
        D3D12_ROOT_PARAMETER rootParameters[3] = {};

        //Material
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;     //PixelShaderで使う
        rootParameters[0].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド

        //Texture
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//テーブルを使う
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
        rootParameters[1].DescriptorTable.pDescriptorRanges = textureDescriptor;	//テーブルの中身
        rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(textureDescriptor);	//テーブルの数

        //DirectionalLight
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//CBVを使う
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
        rootParameters[2].Descriptor.ShaderRegister = 1;	//レジスタ番号1とバインド

        descriptionRootSignature.pParameters = rootParameters;                  //ルートパラメータ配列へのポインタ
        descriptionRootSignature.NumParameters = _countof(rootParameters);      //配列の長さ

        descriptionRootSignature.pStaticSamplers = staticSampler;              //StaticSamplerの配列へのポインタ
        descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);   //配列の長さ

        CreateRootSignature(descriptionRootSignature, RootSignatureID::NonMatrix, device);
    }

    //Particle
    {
        //RootSignature作成
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        //RootParameter作成
        D3D12_ROOT_PARAMETER rootParameters[2] = {};

        //Matrix
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;       //CBVを使う
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;                //VertexShaderで使う
		rootParameters[0].DescriptorTable.pDescriptorRanges = instancingDescriptor;         //テーブルの中身
		rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(instancingDescriptor); //テーブルの数

        //Texture
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//テーブルを使う
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
        rootParameters[1].DescriptorTable.pDescriptorRanges = textureDescriptor;	//テーブルの中身
        rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(textureDescriptor);	//テーブルの数

        descriptionRootSignature.pParameters = rootParameters;                  //ルートパラメータ配列へのポインタ
        descriptionRootSignature.NumParameters = _countof(rootParameters);      //配列の長さ

        descriptionRootSignature.pStaticSamplers = staticSampler;              //StaticSamplerの配列へのポインタ
        descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);   //配列の長さ

        CreateRootSignature(descriptionRootSignature, RootSignatureID::Particle, device);
    }

    //Model
    {
        //RootSignature作成
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        //RootParameter作成
        D3D12_ROOT_PARAMETER rootParameters[3] = {};
        //Material
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;     //PixelShaderで使う
        rootParameters[0].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド
        //Matrix
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;       //CBVを使う
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;                //VertexShaderで使う
        rootParameters[1].DescriptorTable.pDescriptorRanges = instancingDescriptor;         //テーブルの中身
        rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(instancingDescriptor); //テーブルの数
		//Texture
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//テーブルを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = textureDescriptor;	//テーブルの中身
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(textureDescriptor);	//テーブルの数

        descriptionRootSignature.pParameters = rootParameters;                  //ルートパラメータ配列へのポインタ
        descriptionRootSignature.NumParameters = _countof(rootParameters);      //配列の長さ

        descriptionRootSignature.pStaticSamplers = staticSampler;              //StaticSamplerの配列へのポインタ
        descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);   //配列の長さ

        CreateRootSignature(descriptionRootSignature, RootSignatureID::Model, device);
    }
}

RootSignatureShelf::~RootSignatureShelf() {
    for(auto& rootSignature : rootSignatures_) {
        if (rootSignature) {
            rootSignature->Release();
        }
	}
}

ID3D12RootSignature* RootSignatureShelf::GetRootSignature(RootSignatureID id) const {
	return rootSignatures_[int(id)];
}

std::vector<ID3D12RootSignature*> RootSignatureShelf::GetRootSignatures() const {
    return rootSignatures_;
}

void RootSignatureShelf::CreateRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, RootSignatureID id, ID3D12Device* device) {

    Microsoft::WRL::ComPtr<ID3D10Blob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

    //シリアライズしてバイナリにする
    HRESULT hr = D3D12SerializeRootSignature(&desc,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        logger_->Log(std::format("RootSignatureID : {}", int(id)));
        logger_->Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    //バイナリをもとに生成
    ID3D12RootSignature* rootSignature = nullptr;
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));

    rootSignatures_[int(id)] = rootSignature;
}
