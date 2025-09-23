#include "TextureManager.h"
#include <Core/DXCommonFunction.h>

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
	textures_.clear();
	srvDescriptorHeap.Reset();
}

void TextureManager::Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList) {
	ID3D12DescriptorHeap* rawHeap = nullptr;
	rawHeap = CreateDescriptorHeap(device->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxTextureCount, true);
	srvDescriptorHeap.Attach(rawHeap);

	device_ = device;
	commandList_ = commandList;
}

int TextureManager::LoadTexture(std::string filePath) {
	//すでに作成済みの場合はそのハンドルを返す
	if (textureHandleMap_.find(filePath) != textureHandleMap_.end()) {
		return textureHandleMap_[filePath];
	}
	
	textures_.emplace_back(std::make_unique<TextureData>(filePath, device_, commandList_, srvDescriptorHeap.Get()));
	return static_cast<int>(textures_.size() - 1);
}

TextureData* TextureManager::GetTextureData(int handle) {
	if (handle < 0 || handle >= static_cast<int>(textures_.size())) {
		return nullptr;
	}
	return textures_[handle].get();
}
