#include "PSOManager.h"
#include <cassert>

PSOManager::PSOManager(ID3D12Device* device, Logger* logger) {
	shaderShelf_ = std::make_unique<ShaderShelf>();
	depthStencilShelf_ = std::make_unique<DepthStencilShelf>();
	blendStateShelf_ = std::make_unique<BlendStateShelf>();
	rasterizerShelf_ = std::make_unique<RasterizerShelf>();
	rootSignatureShelf_ = std::make_unique<RootSignatureShelf>(device);
	inputLayoutShelf_ = std::make_unique<InputLayoutShelf>();
	binaryManager_ = std::make_unique<BinaryManager>();

	logger_ = logger;

	device_ = device;
}

PSOManager::~PSOManager() {
	for(auto& [name, pso]: psoMap_) {
		if (pso) {
			pso->Release();
		}
	}
}

void PSOManager::Initialize() {
	//PSOをすべて廃棄
	for (auto& [config, pso] : psoMap_) {
		if (pso) {
			pso->Release();
		}
	}
	psoMap_.clear();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC basicDesc = {};
	basicDesc.NumRenderTargets = 1;
	basicDesc.SampleDesc.Count = 1;
	basicDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	basicDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	basicDesc.NumRenderTargets = 1;

	//ShaderData召喚
	auto rawData = binaryManager_->Read(shaderDataFile);
	for (int i = 0; i < rawData.size(); i += 4) {
		auto ps = dynamic_cast<Value<std::string>*>(rawData[i].get())->value;
		auto vs = dynamic_cast<Value<std::string>*>(rawData[i + 1].get())->value;
		auto inputLayoutID = static_cast<InputLayoutID>(dynamic_cast<Value<int>*>(rawData[i + 2].get())->value);
		auto rootSignatureID = static_cast<RootSignatureID>(dynamic_cast<Value<int>*>(rawData[i + 3].get())->value);
		shaderData_.push_back(ShaderData{ ps, vs, inputLayoutID, rootSignatureID });
	}

	//=====================================================================
	//全通りだと作成に時間がかかるので、いくつかif文で制限をかける
	//必要になったら増やす
	//=====================================================================

	for (int isSwapChain = false; isSwapChain < 2; ++isSwapChain) {
		for (int blend = 0; blend < int(BlendStateID::Count); ++blend) {

			//SwapChain用はAddとNormalのみ
			if (isSwapChain && (blend != int(BlendStateID::Add) && blend != int(BlendStateID::Normal))) {
				continue;
			}

			for (int ds = 0; ds < int(DepthStencilID::Count); ++ds) {

				for (int topology = 0; topology < 2; ++topology) {

					for (int shader = 0; shader < shaderData_.size(); ++shader) {

						PSOConfig config;
						config.vs = shaderData_[shader].vs;
						config.ps = shaderData_[shader].ps;
						config.inputLayoutID = shaderData_[shader].inputLayoutID;
						config.rootID = shaderData_[shader].rootSignatureID;
						config.depthStencilID = static_cast<DepthStencilID>(ds);
						config.blendID = static_cast<BlendStateID>(blend);
						config.topology = (topology == 0) ? D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D_PRIMITIVE_TOPOLOGY_LINELIST;
						config.isSwapChain = bool(isSwapChain);

						//以下defaultとして上で設定したものを使う
						config.rasterizerID = RasterizerID::Default;

						config.Validate(*shaderShelf_.get(), *inputLayoutShelf_.get(), *rootSignatureShelf_.get(), logger_);

						//defaultとして設定したPSOを持ってくる
						D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = basicDesc;

						psoDesc.pRootSignature = rootSignatureShelf_->GetRootSignature(config.rootID);
						psoDesc.VS = shaderShelf_->GetShaderBytecode(ShaderType::VERTEX_SHADER, config.vs);
						psoDesc.PS = shaderShelf_->GetShaderBytecode(ShaderType::PIXEL_SHADER, config.ps);
						psoDesc.DepthStencilState = depthStencilShelf_->GetDepthStencilDesc(config.depthStencilID);
						psoDesc.BlendState = blendStateShelf_->GetBlendState(config.blendID);
						psoDesc.RasterizerState = rasterizerShelf_->GetRasterizerDesc(config.rasterizerID);
						psoDesc.InputLayout = inputLayoutShelf_->GetInputLayoutDesc(config.inputLayoutID);

						psoDesc.RTVFormats[0] = config.isSwapChain ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

						psoDesc.PrimitiveTopologyType = config.topology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE : D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

						ID3D12PipelineState* pso = nullptr;

						HRESULT hr = device_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

						if (FAILED(hr)) {
							logger_->Log(std::format("Failed to Create PSO : DepthStencilID {}, BlendStateID {}", ds, blend));
							assert(false && "Failed to create PSO");
						}

						psoMap_[config] = pso;

					}//shader

				}//topology

			}//depthStencil

		}//blend

	}//isSwapChain

}

ID3D12PipelineState* PSOManager::GetPSO(const PSOConfig& config) {
	auto it = psoMap_.find(config);
	if (it != psoMap_.end()) {
		return it->second;
	} else {
		assert(false && "PSO not found for given config");
		logger_->Log("PSO not found for given config");
		return nullptr;
	}
}

ID3D12RootSignature* PSOManager::GetRootSignature(const RootSignatureID id) const {
	return rootSignatureShelf_->GetRootSignature(id);
}
