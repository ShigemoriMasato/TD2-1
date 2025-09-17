#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include <Logger/Logger.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/d3dx12.h>

IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, Logger* logger);

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible);

DirectX::ScratchImage CreateMipImages(const std::string& filePath);

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDesscriptorHandle(ID3D12DescriptorHeap* heap, uint32_t descriptorSize, UINT index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDesscriptorHandle(ID3D12DescriptorHeap* heap, uint32_t descriptorSize, UINT index);
