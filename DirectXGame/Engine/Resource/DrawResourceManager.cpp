#include "DrawResourceManager.h"
#include <Render/MyDirectX.h>

namespace {
    ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
        //頂点リソース用のヒープの設定
        D3D12_HEAP_PROPERTIES uploadHeapProperties{};
        uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//uploadHeapを使う
        //頂点リソースの設定
        D3D12_RESOURCE_DESC bufferResourceDesc{};
        //バッファリソース、テクスチャの場合はまた別の設定をする
        bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferResourceDesc.Width = sizeInBytes;
        //バッファの場合はこれにする決まり
        bufferResourceDesc.Height = 1;
        bufferResourceDesc.DepthOrArraySize = 1;
        bufferResourceDesc.MipLevels = 1;
        bufferResourceDesc.SampleDesc.Count = 1;
        //バッファの場合はこれにする決まり
        bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        ID3D12Resource* bufferResource = nullptr;

        HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
            &bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(&bufferResource));
        assert(SUCCEEDED(hr));

        return bufferResource;
    }
}

DrawResourceManager::DrawResourceManager(ID3D12Device* device) {
    std::vector<int> vertexNum;
    std::vector<int> indexNum;

    vertexNum.push_back(3);
    vertexNum.push_back(561);
    vertexNum.push_back(4);
    vertexNum.push_back(7);
    vertexNum.push_back(24);
    vertexNum.push_back(2);

    indexNum.push_back(0);
    indexNum.push_back(3072);
    indexNum.push_back(6);
    indexNum.push_back(24);
    indexNum.push_back(36);
    indexNum.push_back(0);

    for (int i = 0; i < primitiveResource_.size(); ++i) {
		primitiveResource_[i] = DrawResource(vertexNum[i], indexNum[i]);
    }

	device_ = device;
}

DrawResourceManager::~DrawResourceManager() {
}

void DrawResourceManager::CreateResource(MyDirectX::DrawKind kind, int num) {
	int index = static_cast<int>(kind);

    primitiveResource_[index].Initialize(device_, num, kind == MyDirectX::kSphere);
}

void DrawResourceManager::CreateModelResource(int modelHandle, int num) {
    for (auto& [materialName, resource] : modelResource_[modelHandle]) {
        resource.Initialize(device_, num, true);
    }
}

void DrawResourceManager::AddModelKind(ModelData modelData, int handle) {
    for (int i = 0; i < modelData.material.size(); ++i) {
		std::string name = modelData.material[i].name;
        modelResource_[handle][name] = DrawResource(static_cast<int>(modelData.vertices[name].size()), 0);
    }
}

DrawResource::DrawResource(int vertexNum, int indexNum) {
    vertexNum_ = vertexNum;
	indexNum_ = indexNum;
}

void DrawResource::Initialize(ID3D12Device* device, int num, bool useMatrix) {
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * vertexNum_ * num);
	materialResource_ = CreateBufferResource(device, sizeof(MaterialData) * num);
	directionalLightResource_ = CreateBufferResource(device, sizeof(DirectionalLightData) * num);
    if (indexNum_ > 0) {
        indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * indexNum_ * num);
    }
    if (useMatrix) {
        wvpResource_ = CreateBufferResource(device, sizeof(TramsformMatrixData) * num);
	}
}

std::array<ID3D12Resource*, static_cast<size_t>(ResourceType::Count)> DrawResourceManager::GetPrimitiveResource(int type) {
    return primitiveResource_[type].GetResource();
}

std::array<ID3D12Resource*, static_cast<size_t>(ResourceType::Count)> DrawResourceManager::GetModelResource(int type, std::string materialName) {
	return modelResource_[type][materialName].GetResource();
}

std::array<ID3D12Resource*, static_cast<size_t>(ResourceType::Count)> DrawResource::GetResource() {
	std::array<ID3D12Resource*, static_cast<size_t>(ResourceType::Count)> resources = {
        vertexResource_.Get(),
        indexResource_.Get(),
        materialResource_.Get(),
        directionalLightResource_.Get(),
        wvpResource_.Get()
    };

    return resources;
}
