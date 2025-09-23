#pragma once
#include <string>
#include <unordered_map>
#include <d3d12.h>
#include <wrl.h>
#include <Core/DXDevice.h>
#include "TextureData.h"

/// <summary>
/// Textureの入出力をするクラス
/// </summary>
class TextureManager {
public:

	TextureManager();
	~TextureManager();

	void Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList);

	int LoadTexture(std::string filePath);

	TextureData* GetTextureData(int handle);

private:

	DXDevice* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	std::unordered_map<std::string, int> textureHandleMap_;
	std::vector<std::unique_ptr<TextureData>> textures_;

	//SRVHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;	

	const int maxTextureCount = 128;

};
