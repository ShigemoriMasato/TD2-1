#pragma once
#include <vector>
#include <d3d12.h>
#include <Render/MyDirectX.h>
#include <Logger/Logger.h>
#include <Data/DrawData.h>
#include <array>
#include <unordered_map>
#include <wrl.h>

enum class ResourceType {
	Vertex,
	Index,
	Material,
	DirectionalLight,
	Matrix,
	Count
};

class DrawResource {
public:
	DrawResource() = default;
	DrawResource(int vertexNum, int indexNum);
	~DrawResource() = default;

	void Initialize(ID3D12Device* device, int num, bool useMatrix = false);

	std::array<ID3D12Resource*, static_cast<size_t>(ResourceType::Count)> GetResource();

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;

	int vertexNum_ = 0;
	int indexNum_ = 0;
};

class DrawResourceManager {
public:

	DrawResourceManager(ID3D12Device* device);
	~DrawResourceManager();

	void CreateResource(MyDirectX::DrawKind kind, int num);
	void CreateModelResource(int modelHandle, int num);

	std::array<ID3D12Resource*, static_cast<size_t>(ResourceType::Count)> GetPrimitiveResource(int type);
	std::array<ID3D12Resource*, static_cast<size_t>(ResourceType::Count)> GetModelResource(int type, std::string materialName);

	void AddModelKind(ModelData modelData, int handle);

private:

	//1フレームに描画した数をカウントする
	std::vector<uint32_t> drawCount;

	std::array<DrawResource, size_t(MyDirectX::DrawKindCount)> primitiveResource_;
	std::vector<std::unordered_map<std::string, DrawResource>> modelResource_{};

	//modelとsphereだけ使う
	ID3D12Device* device_ = nullptr;
};
