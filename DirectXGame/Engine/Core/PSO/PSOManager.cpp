#include "PSOManager.h"
#include <cassert>

PSOManager::PSOManager(ID3D12Device* device, Logger* logger) {
	shaderShelf_ = std::make_unique<ShaderShelf>();
	shaderShelf_->CompileAllShader();
	depthStencilShelf_ = std::make_unique<DepthStencilShelf>();
	blendStateShelf_ = std::make_unique<BlendStateShelf>();
	rasterizerShelf_ = std::make_unique<RasterizerShelf>();
	rootSignatureShelf_ = std::make_unique<RootSignatureShelf>(device);
	inputLayoutShelf_ = std::make_unique<InputLayoutShelf>();

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
	basicDesc.RasterizerState = rasterizerShelf_->GetRasterizerDesc(RasterizerID::Default);
	basicDesc.InputLayout = inputLayoutShelf_->GetInputLayoutDesc(InputLayoutID::Default);
	basicDesc.pRootSignature = rootSignatureShelf_->GetRootSignature(RootSignatureID::Default);
	basicDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	basicDesc.NumRenderTargets = 1;

	std::list<std::string> vsList = shaderShelf_->GetShaderNames(ShaderType::VERTEX_SHADER);
	std::list<std::string> psList = shaderShelf_->GetShaderNames(ShaderType::PIXEL_SHADER);

	for (int offscreen = false; offscreen < 2; ++offscreen) {
		for (int topology = 0; topology < 2; ++topology) {
			for (const auto& vs : vsList) {
				for (const auto& ps : psList) {
					for (int ds = 0; ds < int(DepthStencilID::Count); ++ds) {
						for (int blend = 0; blend < int(BlendStateID::Count); ++blend) {

							PSOConfig config;
							config.vs = vs;
							config.ps = ps;
							config.depthStencilID = static_cast<DepthStencilID>(ds);
							config.blendID = static_cast<BlendStateID>(blend);
							config.topology = (topology == 0) ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE : D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
							config.isOffScreen = bool(offscreen);

							//以下defaultとして上で設定したものを使う
							config.inputLayoutID = InputLayoutID::Default;
							config.rasterizerID = RasterizerID::Default;
							config.rootID = RootSignatureID::Default;

							config.Validate(*shaderShelf_.get(), *inputLayoutShelf_.get(), *rootSignatureShelf_.get(), logger_);

							//defaultとして設定したPSOを持ってくる
							D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = basicDesc;

							psoDesc.VS = shaderShelf_->GetShaderBytecode(ShaderType::VERTEX_SHADER, vs);
							psoDesc.PS = shaderShelf_->GetShaderBytecode(ShaderType::PIXEL_SHADER, ps);
							psoDesc.DepthStencilState = depthStencilShelf_->GetDepthStencilDesc(static_cast<DepthStencilID>(ds));
							psoDesc.BlendState = blendStateShelf_->GetBlendState(static_cast<BlendStateID>(blend));
							psoDesc.PrimitiveTopologyType = config.topology;
							psoDesc.RTVFormats[0] = config.isOffScreen ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

							ID3D12PipelineState* pso = nullptr;

							HRESULT hr = device_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

							if (FAILED(hr)) {
								logger_->Log(std::format("Failed to Create PSO : DepthStencilID {}, BlendStateID {}", ds, blend));
								assert(false && "Failed to create PSO");
							}

							psoMap_[config] = pso;
						}
					}
				}
			}
		}
	}

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
