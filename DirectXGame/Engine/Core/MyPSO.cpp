#include "MyPSO.h"
#include <cassert>

MyPSO::MyPSO() {
	shaderShelf = std::make_unique<ShaderShelf>();
	binaryPSO = std::make_unique<BinaryPSO>(this);
}

MyPSO::~MyPSO() {
	for(auto& pso : psoList) {
		if (pso) {
			pso->Release();
		}
	}
}

void MyPSO::Initialize(ID3D12Device* device, ID3D12RootSignature* rs) {
	this->device = device;
	isDeviceSet = true;

	//PSOをすべて廃棄
	for (auto& pso : psoList) {
		if (pso) {
			pso->Release();
		}
	}

	pRootSignature = rs;
}

void MyPSO::SetPSODesc(D3D12_DEPTH_STENCIL_DESC dsvDesc, DXGI_FORMAT dsvFormat, ID3D12RootSignature* pRootSignature, D3D12_INPUT_LAYOUT_DESC inputLayoutDesc, std::string vs, std::string ps, D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc, UINT numRtv, DXGI_FORMAT rtvFormat, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType, UINT sampleMask) {
	psoDesc.dsvDesc = dsvDesc;
	psoDesc.dsvFormat = dsvFormat;

	//psoDesc.pRootSignature = pRootSignature;
	//psoDesc.inputLayoutDesc = inputLayoutDesc;
	psoDesc.vs = vs;
	psoDesc.ps = ps;
	psoDesc.blendDesc= blendDesc;
	psoDesc.rasterizerDesc = rasterizerDesc;

	psoDesc.numRtv = numRtv;
	psoDesc.rtvFormat = rtvFormat;

	psoDesc.primitiveTopologyType = primitiveTopologyType;

	//複数のsampleMaskを使用するのであれば、UINTの引数を増やして初期値を適当に設定して、それ以外のものをカウントして配置する
	psoDesc.sampleMask = sampleMask;
}

void MyPSO::SetPSODesc(PSODescData psoDesc) {
	this->psoDesc = psoDesc;
}

void MyPSO::SetDsvDesc(D3D12_DEPTH_STENCIL_DESC dsvDesc) {
	psoDesc.dsvDesc = dsvDesc;
}

void MyPSO::SetDsvFormat(DXGI_FORMAT dsvFormat) {
	psoDesc.dsvFormat = dsvFormat;
}

void MyPSO::SetPRootSignature(ID3D12RootSignature* pRootSignature) {
	if (pRootSignature) {
		//psoDesc.pRootSignature = pRootSignature;
	} else {
		assert(false && "Root signature cannot be null");
	}
}

void MyPSO::SetInputLayoutDesc(D3D12_INPUT_LAYOUT_DESC inputLayoutDesc) {
	//psoDesc.inputLayoutDesc = inputLayoutDesc;
}

void MyPSO::SetVS(std::string vs) {
	psoDesc.vs = vs;
}

void MyPSO::SetPS(std::string ps) {
	psoDesc.ps = ps;
}

void MyPSO::SetBlendDesc(D3D12_BLEND_DESC blendState) {
	psoDesc.blendDesc = blendState;
}

void MyPSO::SetRasterizerDesc(D3D12_RASTERIZER_DESC rasterizerDesc) {
	psoDesc.rasterizerDesc = rasterizerDesc;
}

void MyPSO::SetNumRtv(UINT numRtv) {
	psoDesc.numRtv = numRtv;
}

void MyPSO::SetRtvFormat(DXGI_FORMAT rtvFormat) {
	psoDesc.rtvFormat = rtvFormat;
}

void MyPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology) {
	psoDesc.primitiveTopologyType = primitiveTopology;
}

void MyPSO::SetSampleMask(UINT sampleMask) {
	psoDesc.sampleMask = sampleMask;
}

void MyPSO::CreatePSO(int index, bool isSave) {
	//indexが負の値の場合はエラー
	if(index < 0) {
		assert(false && "Index out of bounds for PSO List");
		return;
	}

	//indexがPSOリストのサイズを超えている場合は、リストを拡張する
	if (index >= psoList.size()) {
		psoList.resize(index + 1);
	}

	//もしindexの位置にPSOが存在している場合は、Releaseする
	if (psoList[index] != nullptr) {
		psoList[index]->Release();
	}

	//PSODescDataからPSODescを設定する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.DepthStencilState = psoDesc.dsvDesc;
	desc.DSVFormat = psoDesc.dsvFormat;
	desc.pRootSignature = pRootSignature;
	desc.InputLayout = shaderShelf->GetInputLayoutDesc();
	desc.BlendState = psoDesc.blendDesc;
	desc.RasterizerState = psoDesc.rasterizerDesc;
	desc.NumRenderTargets = psoDesc.numRtv;
	desc.RTVFormats[0] = psoDesc.rtvFormat;
	desc.PrimitiveTopologyType = psoDesc.primitiveTopologyType;
	desc.SampleDesc.Count = 1;
	desc.SampleMask = psoDesc.sampleMask;
	shaderShelf->RegisterShader(psoDesc.vs, ShaderType::VERTEX_SHADER);
	IDxcBlob* vsBlob = shaderShelf->GetShaderBlob(psoDesc.vs, ShaderType::VERTEX_SHADER);
	desc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	shaderShelf->RegisterShader(psoDesc.ps, ShaderType::PIXEL_SHADER);
	IDxcBlob* psBlob = shaderShelf->GetShaderBlob(psoDesc.ps, ShaderType::PIXEL_SHADER);
	desc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };

	ID3D12PipelineState* pso = nullptr;
	HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
	assert(SUCCEEDED(hr));

	this->psoList[index] = (pso);

}

ID3D12PipelineState* MyPSO::Get(int index) {
	
	if(index < 0 || index >= psoList.size()) {
		assert(false && "Index out of bounds for PSO List");
		return nullptr;
	}

	return psoList[index];
}

BinaryPSO::BinaryPSO(MyPSO* myPSO) {
	binaryManager = std::make_unique<BinaryManager>();
	this->myPSO = myPSO;
}

BinaryPSO::~BinaryPSO() {
	for (auto& pso : psoDescList) {
		binaryManager->RegistOutput(pso, "p");
	}

	binaryManager->Write("DefaultPSO.dat");
}

void BinaryPSO::CreatePSOsFromBinary(const std::string& fileName, ID3D12Device* device) {
	
	auto data = binaryManager->Read(fileName);

	for (auto& psoDesc : data) {
		auto& desc = dynamic_cast<Value<PSODescData>*>(psoDesc.get())->value;
		myPSO->SetPSODesc(desc);
		myPSO->CreatePSO(int(psoDescList.size()), true);
	}
}

void BinaryPSO::RegistBinaryData(PSODescData pso) {
	psoDescList.push_back(pso);
}
