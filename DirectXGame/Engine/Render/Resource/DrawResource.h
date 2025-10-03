#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include <Transform/Transform.h>
#include <Core/PSO/PSOConfig.h>
#include <Camera/Camera.h>

/// <summary>
/// CG2で作成した基本的(?)な描画情報
/// </summary>
class DrawResource {
public:

	DrawResource();
	~DrawResource();

	static void SetDevice(DXDevice* device) { dxDevice_ = device; }

	void Initialize(uint32_t vertexNum, uint32_t indexNum = 0, bool useMatrix = false);

	/// <summary>
	/// 描画前準備(Render内で呼ばれるため、プログラム時に呼ぶ必要はない)
	/// </summary>
	void DrawReady();

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
	ID3D12Resource* GetMaterialResource() const { return materialResource.Get(); }
	ID3D12Resource* GetParticleDataResource() const;
	ID3D12Resource* GetLightResource() const { return lightResource.Get(); }

	uint32_t GetVertexNum() const { return vertexNum_; }
	uint32_t GetIndexNum() const { return indexNum_; }

	PSOConfig psoConfig_{};

	int textureHandle_ = 0;

	std::vector<Vector3> localPos_{};
	std::vector<Vector2> texcoord_{};
	std::vector<Vector3> normal_{};

	std::vector<uint32_t> index_{};

	Vector3 position_{};
	Vector3 rotate_{};
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	Vector2 texturePos_{};
	Vector2 textureScale_{ 1.0f, 1.0f };
	float textureRotate_ = 0.0f;

	uint32_t color_ = 0xffffffff;

	bool enableLighting_ = false;
	Vector3 lightDirection_ = { 0.0f, 0.0f, 1.0f };
	uint32_t lightColor_ = 0xffffffff;
	float lightIntensity_ = 1.0f;

	Camera* camera_ = nullptr;

private:

	VertexData* vertex_ = nullptr;
	uint32_t* indices_ = nullptr;
	Material* material_ = nullptr;
	MatrixData* matrix_ = nullptr;
	DirectionalLightData* light_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> particleDataResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	static DXDevice* dxDevice_;

	uint32_t vertexNum_ = 0;
	uint32_t indexNum_ = 0;

};
