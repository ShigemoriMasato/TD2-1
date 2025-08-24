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

DrawResourceManager::DrawResourceManager() {
    vertexNum_.resize(int(MyDirectX::DrawKindCount));
    indexNum_.resize(int(MyDirectX::DrawKindCount));

    vertexNum_[0] = 3;
    vertexNum_[1] = 561;
    vertexNum_[2] = 4;
    vertexNum_[3] = 7;
    vertexNum_[4] = 24;
    vertexNum_[5] = 2;

    indexNum_[0] = 0;
    indexNum_[1] = 3072;
    indexNum_[2] = 6;
    indexNum_[3] = 24;
    indexNum_[4] = 36;
    indexNum_[5] = 0;
}

DrawResourceManager::~DrawResourceManager() {
    
}

void DrawResourceManager::CreateResource(MyDirectX::DrawKind kind, int num) {
	int index = static_cast<int>(kind);

    bool useIndex = kind != MyDirectX::kTriangle && kind != MyDirectX::kLine;

    primitiveResource_[index].CreateResource(device_, num, vertexNum_[index], kind == MyDirectX::kSphere, useIndex, indexNum_[index]);
}

void DrawResourceManager::CreateModelResource(int modelHandle, int num) {
}

void DrawResourceManager::AddModelKind(ModelData modelData, int handle) {
    for (int i = handle; i > handle - modelData.material.size(); --i) {

    }
}

void DrawResource::CreateResource(ID3D12Device* device, int num, int vertexNum, bool useMatrix, bool useIndex, int indexNum) {

    for (int i = 0; i < num; ++i) {
        vertexResource_.push_back(CreateBufferResource(device, sizeof(VertexData) * vertexNum));
        materialResource_.push_back(CreateBufferResource(device, sizeof(MaterialData)));
        //directionalLightの処遇は後で考える
        directionalLightResource_.push_back(CreateBufferResource(device, sizeof(DirectionalLightData)));

        if (useMatrix) {
            wvpResource_.push_back(CreateBufferResource(device, sizeof(TramsformMatrixData)));
        }
        if (useIndex) {
            indexResource_.push_back(CreateBufferResource(device, sizeof(uint32_t)));
        }
    }

}

void DrawResource::ClearResource() {
    for (int j = 0; j < vertexResource_.size(); ++j) {
        vertexResource_[j]->Release();
        materialResource_[j]->Release();
        directionalLightResource_[j]->Release();
    }

    for (auto& indexResource : indexResource_) {
        indexResource->Release();
    }

    for (auto& wvpResource : wvpResource_) {
        wvpResource->Release();
    }
}
