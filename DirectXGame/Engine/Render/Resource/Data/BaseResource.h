#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include <Transform/Transform.h>
#include <vector>
#include <Core/PSO/PSOConfig.h>
#include <Core/DXCommonFunction.h>
#include <Resource/SRVManager.h>
#include <Math/MyMath.h>

class BaseResource {
public:

	static void StaticInitialize(DXDevice* device, SRVManager* srvManager) { dxDevice_ = device; srvManager_ = srvManager; }

	BaseResource() = default;
	virtual ~BaseResource() = default;

	virtual void DrawReady() = 0;

	//行列を直接指定したかったら毎フレーム呼び出すこと
	void SetWorldMatrix(const Matrix4x4& world) { worldMatrix_ = world; isSetMatrix_ = true; }

	std::vector<Vector3> localPos_{};
	std::vector<Vector3> normal_{};
	std::vector<Vector2> texcoord_{};

	PSOConfig psoConfig_;

protected:

	//ワールド行列を取得。行列を直接指定していた場合は引数不要。あっても問題はない
	Matrix4x4 GetWorldMatrix(const Vector3 scale = {}, const Vector3 rotation = {}, const Vector3 position = {});

	VertexData* vertex_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	uint32_t vertexNum_ = 0;

	static DXDevice* dxDevice_;
	static SRVManager* srvManager_;

	//Matrixに関する奴
	Matrix4x4 worldMatrix_ = Matrix::MakeIdentity4x4();
	bool isSetMatrix_ = false;

};
