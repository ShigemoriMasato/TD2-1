#pragma once
#include <vector>
#include <d3d12.h>
#include <Render/MyDirectX.h>
#include <Logger/Logger.h>
#include <Data/DrawData.h>
#include <array>

struct DrawResource {
	std::vector<ID3D12Resource*> vertexResource_;
	std::vector<ID3D12Resource*> materialResource_;
	std::vector<ID3D12Resource*> directionalLightResource_;
	std::vector<ID3D12Resource*> indexResource_;
	std::vector<ID3D12Resource*> wvpResource_;

	void CreateResource(ID3D12Device* device, int num, int vertexNum, bool useMatrix = false, bool useIndex = false, int indexNum = 0);
	void ClearResource();
};

class DrawResourceManager {
public:

	DrawResourceManager();
	~DrawResourceManager();

	void CreateResource(MyDirectX::DrawKind kind, int num);
	void CreateModelResource(int modelHandle, int num);

	void AddModelKind(ModelData modelData, int handle);

private:

	//1フレームに描画した数をカウントする
	std::vector<uint32_t> drawCount;

	std::array<DrawResource, size_t(MyDirectX::DrawKindCount)> primitiveResource_;
	DrawResource modelResource_;

	//modelとsphereだけ使う

	std::vector<int> vertexNum_;
	std::vector<int> indexNum_;

	ID3D12Device* device_ = nullptr;
};
