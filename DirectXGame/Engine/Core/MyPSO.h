#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <vector>
#include <Binary\BinaryManager.h>
#include <Shader/ShaderShelf.h>

struct PSODescData {
	D3D12_DEPTH_STENCIL_DESC dsvDesc;
	DXGI_FORMAT dsvFormat;
	std::string vs;
	std::string ps;
	D3D12_BLEND_DESC blendDesc;
	D3D12_RASTERIZER_DESC rasterizerDesc;
	UINT numRtv;
	DXGI_FORMAT rtvFormat;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
	UINT sampleMask;
};

class MyPSO;

/// <summary>
/// バイナリからPSOを作成したり、バイナリに保存するクラス
/// </summary>
class BinaryPSO {
public:

	BinaryPSO(MyPSO* myPSO);
	~BinaryPSO();

	void CreatePSOsFromBinary(const std::string& fileName, ID3D12Device* device);

	void RegistBinaryData(PSODescData pso);

private:

	std::unique_ptr<BinaryManager> binaryManager = nullptr;
	std::vector<PSODescData> psoDescList;

	MyPSO* myPSO = nullptr;
};

//あらゆるPSOを管理するクラス
class MyPSO {
public:

	MyPSO();
	~MyPSO();

	void Initialize(ID3D12Device* device, ID3D12RootSignature* rs);

	/// <summary>
	/// PSODescを一括ですべて設定する
	/// </summary>
	/// <param name="dsvDesc"></param>
	/// <param name="dsvFormat"></param>
	/// <param name="pRootSignature"></param>
	/// <param name="inputLayoutDesc"></param>
	/// <param name="vs"></param>
	/// <param name="ps"></param>
	/// <param name="blendDesc"></param>
	/// <param name="rasterizerDesc"></param>
	/// <param name="numRtv"></param>
	/// <param name="rtvFormat"></param>
	/// <param name="primitiveTopologyType"></param>
	/// <param name="sampleMask"></param>
	void SetPSODesc(D3D12_DEPTH_STENCIL_DESC dsvDesc, DXGI_FORMAT dsvFormat, ID3D12RootSignature* pRootSignature, D3D12_INPUT_LAYOUT_DESC inputLayoutDesc,
		std::string vs, std::string ps, D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc, UINT numRtv,
		DXGI_FORMAT rtvFormat, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType, UINT sampleMask);

	void SetPSODesc(PSODescData psoDesc);

	void SetDsvDesc(D3D12_DEPTH_STENCIL_DESC dsvDesc);
	void SetDsvFormat(DXGI_FORMAT dsvFormat);
	void SetPRootSignature(ID3D12RootSignature* pRootSignature);
	void SetInputLayoutDesc(D3D12_INPUT_LAYOUT_DESC inputLayoutDesc);

	void SetVS(std::string vs);
	void SetPS(std::string ps);

	void SetBlendDesc(D3D12_BLEND_DESC blendState);

	void SetRasterizerDesc(D3D12_RASTERIZER_DESC rasterizerDesc);

	void SetNumRtv(UINT numRtv);
	void SetRtvFormat(DXGI_FORMAT rtvFormat);

	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology);
	void SetSampleMask(UINT sampleMask);

	void CreatePSO(int index, bool isSave = false);

	/// <summary>
	/// PSOを生成して、返す。PSODescはリセットされません。
	/// <returns></returns>
	ID3D12PipelineState* Get(int index);

private:

	PSODescData psoDesc{};

	ID3D12Device* device = nullptr;
	bool isDeviceSet = false;

	std::vector<ID3D12PipelineState*> psoList;

	std::unique_ptr<ShaderShelf> shaderShelf = nullptr;

	std::unique_ptr<BinaryPSO> binaryPSO = nullptr;

	ID3D12RootSignature* pRootSignature = nullptr;
};
