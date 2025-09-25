#include "DXResource.h"
#include <Core/DXCommonFunction.h>
#include <Math/MyMath.h>

DXDevice* DXResource::dxDevice_ = nullptr;
using namespace Matrix;

DXResource::DXResource() {
}

DXResource::~DXResource() {
}

void DXResource::Initialize(uint32_t vertexNum, uint32_t indexNum, bool useMatrix) {
	psoConfig_ = PSOConfig{};

	auto device = dxDevice_->GetDevice();

	//リソースの生成とマップ
	vertexResource.Attach(CreateBufferResource(device, sizeof(VertexData) * vertexNum));
	vertexResource->Map(0, nullptr, (void**)&vertex_);
	localPos_.resize(vertexNum);
	texcoord_.resize(vertexNum);
	normal_.resize(vertexNum);

	//頂点のバッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	
	//マテリアル
	materialResource.Attach(CreateBufferResource(device, sizeof(MaterialData)));
	materialResource->Map(0, nullptr, (void**)&material_);
	*material_ = MaterialData{};
	
	lightResource.Attach(CreateBufferResource(device, sizeof(DirectionalLightData)));
	lightResource->Map(0, nullptr, (void**)&light_);

	if (indexNum > 0) {
		indexResource.Attach(CreateBufferResource(device, sizeof(uint32_t) * indexNum));
		indexResource->Map(0, nullptr, (void**)&indices_);

		indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = sizeof(uint32_t) * indexNum;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	if (useMatrix) {
		matrixResource.Attach(CreateBufferResource(device, sizeof(MatrixData)));
		matrixResource->Map(0, nullptr, (void**)&matrix_);
		matrix_->world = Matrix::MakeIdentity4x4();
		matrix_->wvp = Matrix::MakeIdentity4x4();
	}

	vertexNum_ = vertexNum;
	indexNum_ = indexNum;

	if (!useMatrix) {
		psoConfig_.rootID = RootSignatureID::NonMatrix;
		psoConfig_.vs = "NonMatrix3d.VS.hlsl";
	}
}

void DXResource::DrawReady() {
	//InputLayout
	localPos_.resize(vertexNum_);
	texcoord_.resize(vertexNum_);
	normal_.resize(vertexNum_);
	for (uint32_t i = 0; i < vertexNum_; ++i) {
		vertex_[i].position = { localPos_[i].x, localPos_[i].y, localPos_[i].z, 1.0f };
		vertex_[i].texcoord = texcoord_[i];
		vertex_[i].normal = normal_[i];
	}

	//IndexBuffer
	if (indexNum_ > 0) {
		index_.resize(indexNum_);
		for (uint32_t i = 0; i < indexNum_; ++i) {
			
			if (index_[i] < 0 || index_[i] >= vertexNum_) {
				index_[i] = 0;
			}

			indices_[i] = index_[i];
		}
	}

	//Material
	material_->color = {
		((color_ >> 24) & 0xff) / 255.0f,
		((color_ >> 16) & 0xff) / 255.0f,
		((color_ >> 8) & 0xff) / 255.0f,
		((color_ >> 0) & 0xff) / 255.0f
	};
	material_->enableLighting = enableLighting_ ? 1 : 0;
	material_->uvTransform = {
		textureScale_.x * cosf(textureRotate_), -textureScale_.y * sinf(textureRotate_), 0.0f, 0.0f,
		textureScale_.x * sinf(textureRotate_), textureScale_.y * cosf(textureRotate_), 0.0f, 0.0f,
		texturePos_.x, texturePos_.y, 1.0f, 0.0f,
	};

	//Matrix
	Matrix4x4 worldMat = MakeScaleMatrix(scale_) * MakeRotationMatrix(rotate_) * MakeTranslationMatrix(position_);

	if (matrix_) {
		matrix_->world = worldMat;
		
		if (camera_) {
			matrix_->wvp = matrix_->world * camera_->VPMatrix();
		}
	} else {

		Matrix4x4 wvpMat = worldMat * camera_->VPMatrix();

		//Shaderの代わりに行列計算を受け持つ
		for (uint32_t i = 0; i < vertexNum_; ++i) {
			Vector3 prePos = Vector3(vertex_[i].position.x, vertex_[i].position.y, vertex_[i].position.z);
			Vector3 pos = prePos * wvpMat;
			vertex_[i].position = { pos.x, pos.y, pos.z, 1.0f };
			vertex_[i].normal = (normal_[i] * worldMat).Normalize();
		}
	}

	//Lighting
	if (enableLighting_) {
		light_->color = {
			((lightColor_ >> 24) & 0xff) / 255.0f,
			((lightColor_ >> 16) & 0xff) / 255.0f,
			((lightColor_ >> 8) & 0xff) / 255.0f,
			((lightColor_ >> 0) & 0xff) / 255.0f
		};
		light_->intensity = lightIntensity_;
		lightDirection_ = lightDirection_.Normalize();
		light_->direction = lightDirection_;
	}
}

D3D12_INDEX_BUFFER_VIEW DXResource::GetIndexBufferView() const {
	if (!indexResource) {
		return D3D12_INDEX_BUFFER_VIEW{};
	}
	return indexBufferView;
}

ID3D12Resource* DXResource::GetMatrixResource() const {
	if(matrixResource) {
		return matrixResource.Get();
	}
	return nullptr;
}
