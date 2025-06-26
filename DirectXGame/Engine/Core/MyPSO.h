#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

class MyPSO {
public:

	MyPSO();
	~MyPSO();

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
		D3D12_SHADER_BYTECODE vs, D3D12_SHADER_BYTECODE ps, D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc, UINT numRtv,
		DXGI_FORMAT rtvFormat, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType, UINT sampleMask);

	void SetDsvDesc(D3D12_DEPTH_STENCIL_DESC dsvDesc);
	void SetDsvFormat(DXGI_FORMAT dsvFormat);
	void SetPRootSignature(ID3D12RootSignature* pRootSignature);
	void SetInputLayoutDesc(D3D12_INPUT_LAYOUT_DESC inputLayoutDesc);

	void SetVS(D3D12_SHADER_BYTECODE vs);
	void SetPS(D3D12_SHADER_BYTECODE ps);

	void SetBlendDesc(D3D12_BLEND_DESC blendState);

	void SetRasterizerDesc(D3D12_RASTERIZER_DESC rasterizerDesc);

	void SetNumRtv(UINT numRtv);
	void SetRtvFormat(DXGI_FORMAT rtvFormat);

	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology);
	void SetSampleMask(UINT sampleMask);

	void CreatePSO(int index);

	void SetDevice(ID3D12Device* device);

	/// <summary>
	/// PSOを生成して、返す。PSODescはリセットされません。
	/// </summary>
	/// <returns></returns>
	ID3D12PipelineState* Get(int index);

private:

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};

	ID3D12Device* device = nullptr;
	bool isDeviceSet = false;

	std::vector<ID3D12PipelineState*> psoList;
};
