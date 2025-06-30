#include "MyPSO.h"
#include <cassert>

MyPSO::MyPSO() {
}

MyPSO::~MyPSO() {
	for(auto& pso : psoList) {
		if (pso) {
			pso->Release();
		}
	}
}

void MyPSO::SetPSODesc(D3D12_DEPTH_STENCIL_DESC dsvDesc, DXGI_FORMAT dsvFormat, ID3D12RootSignature* pRootSignature, D3D12_INPUT_LAYOUT_DESC inputLayoutDesc, D3D12_SHADER_BYTECODE vs, D3D12_SHADER_BYTECODE ps, D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc, UINT numRtv, DXGI_FORMAT rtvFormat, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType, UINT sampleMask) {
	psoDesc.DepthStencilState = dsvDesc;
	psoDesc.DSVFormat = dsvFormat;

	psoDesc.pRootSignature = pRootSignature;
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.VS = vs;
	psoDesc.PS = ps;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;

	psoDesc.NumRenderTargets = numRtv;
	psoDesc.RTVFormats[0] = rtvFormat;

	psoDesc.PrimitiveTopologyType = primitiveTopologyType;

	//複数のsampleMaskを使用するのであれば、UINTの引数を増やして初期値を適当に設定して、それ以外のものをカウントして配置する
	psoDesc.SampleDesc.Count = 1; 
	psoDesc.SampleMask = sampleMask;
}

void MyPSO::SetDsvDesc(D3D12_DEPTH_STENCIL_DESC dsvDesc) {
	psoDesc.DepthStencilState = dsvDesc;
}

void MyPSO::SetDsvFormat(DXGI_FORMAT dsvFormat) {
	psoDesc.DSVFormat = dsvFormat;
}

void MyPSO::SetPRootSignature(ID3D12RootSignature* pRootSignature) {
	if (pRootSignature) {
		psoDesc.pRootSignature = pRootSignature;
	} else {
		assert(false && "Root signature cannot be null");
	}
}

void MyPSO::SetInputLayoutDesc(D3D12_INPUT_LAYOUT_DESC inputLayoutDesc) {
	psoDesc.InputLayout = inputLayoutDesc;
}

void MyPSO::SetVS(D3D12_SHADER_BYTECODE vs) {
	psoDesc.VS = vs;
}

void MyPSO::SetPS(D3D12_SHADER_BYTECODE ps) {
	psoDesc.PS = ps;
}

void MyPSO::SetBlendDesc(D3D12_BLEND_DESC blendState) {
	psoDesc.BlendState = blendState;
}

void MyPSO::SetRasterizerDesc(D3D12_RASTERIZER_DESC rasterizerDesc) {
	psoDesc.RasterizerState = rasterizerDesc;
}

void MyPSO::SetNumRtv(UINT numRtv) {
	psoDesc.NumRenderTargets = numRtv;
}

void MyPSO::SetRtvFormat(DXGI_FORMAT rtvFormat) {
	psoDesc.RTVFormats[0] = rtvFormat;
	for (UINT i = 1; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
		psoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN; // Clear unused RTV formats
	}
}

void MyPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology) {
	psoDesc.PrimitiveTopologyType = primitiveTopology;
}

void MyPSO::SetSampleMask(UINT sampleMask) {
	psoDesc.SampleDesc.Count = 1; // Assuming single sample for simplicity
	psoDesc.SampleMask = sampleMask;
}

void MyPSO::CreatePSO(int index) {
	if(index < 0) {
		assert(false && "Index out of bounds for PSO List");
		return;
	}

	if (index >= psoList.size()) {
		psoList.resize(index + 1); // Resize the array to accommodate the new index
	}

	if (psoList[index] != nullptr) {
		psoList[index]->Release();
	}

	ID3D12PipelineState* pso = nullptr;
	HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
	assert(SUCCEEDED(hr));

	this->psoList[index] = (pso);
}

void MyPSO::SetDevice(ID3D12Device* device) {
	this->device = device;
	isDeviceSet = true; // Reset the flag when setting a new device
}

ID3D12PipelineState* MyPSO::Get(int index) {
	
	if(index < 0 || index >= psoList.size()) {
		assert(false && "Index out of bounds for PSO List");
		return nullptr;
	}

	return psoList[index];
}
