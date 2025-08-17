#include "BlendStateShelf.h"

BlendStateShelf::BlendStateShelf() {
	blendStates_.resize(int(BlendStateID::Count));

	//NoBlend
	D3D12_BLEND_DESC noBlend{};
	noBlend.AlphaToCoverageEnable = false;
	noBlend.IndependentBlendEnable = false;
	noBlend.RenderTarget[0].BlendEnable = false;
	noBlend.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	noBlend.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;

	//AlphaBlend
	D3D12_BLEND_DESC alphaBlend{};
	alphaBlend.AlphaToCoverageEnable = false;
	alphaBlend.IndependentBlendEnable = false;
	alphaBlend.RenderTarget[0].BlendEnable = true;
	alphaBlend.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	alphaBlend.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	alphaBlend.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	alphaBlend.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	alphaBlend.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	alphaBlend.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//すべての色を取り込む
	alphaBlend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendStates_[int(BlendStateID::Alpha)] = alphaBlend;

}

BlendStateShelf::~BlendStateShelf() {
}

D3D12_BLEND_DESC BlendStateShelf::GetBlendState(BlendStateID id) const {
	return blendStates_[int(id)];
}
