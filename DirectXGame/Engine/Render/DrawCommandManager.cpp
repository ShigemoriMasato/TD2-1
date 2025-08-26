#include "DrawCommandManager.h"
#include "MyDirectX.h"
#include <numbers>

DrawCommandManager::DrawCommandManager(ModelManager* manager) {
	manager_ = manager;
}

DrawCommandManager::~DrawCommandManager() {
}

void DrawCommandManager::DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle) {
	int drawType = MyDirectX::kTriangle;
	
	PSOConfig pso{};

	std::list<VertexData> vertices;
	VertexData vertexData;
	//左下
	vertexData.position = left;
	vertexData.texcoord = { 0.0f, 1.0f };
	vertexData.normal = { 0.0f, 0.0f, -1.0f };
	vertices.push_back(vertexData);

	//上
	vertexData.position = top;
	vertexData.texcoord = { 0.5f, 0.0f };
	vertices.push_back(vertexData);

	//右下
	vertexData.position = right;
	vertexData.texcoord = { 1.0f, 1.0f };
	vertices.push_back(vertexData);

	TramsformMatrixData matrixData{};
	matrixData.world = worldMatrix;
	matrixData.wvp = wvpMatrix;

	DrawData drawData{};
	drawData.vertex = vertices;

	MakeDrawData(drawType, drawData, material, pso, matrixData, textureHandle);
}

void DrawCommandManager::DrawSphere(float radius, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle) {
	int drawType = MyDirectX::kSphere;

	int vertical = 16;
	int horizontal = 32;

	std::list<VertexData> vertices;

	for (int i = 0; i < vertical + 1; ++i) {
		float theta = i * std::numbers::pi_v<float> / vertical; // 緯度

		float sinTheta = std::sin(theta);
		float cosTheta = std::cos(theta);

		for (int j = 0; j < horizontal + 1; ++j) {
			float phi = j * std::numbers::pi_v<float> *2.0f / horizontal;

			float sinPhi = std::sin(phi);
			float cosPhi = std::cos(phi);

			VertexData vertexData{};

			// 球の頂点座標を計算
			vertexData.position.x = radius * cosPhi * sinTheta;
			vertexData.position.y = radius * cosTheta;
			vertexData.position.z = radius * sinPhi * sinTheta;
			vertexData.position.w = 1.0f;

			vertexData.texcoord.x = static_cast<float>(j) / horizontal;
			vertexData.texcoord.y = static_cast<float>(i) / vertical;

			vertexData.normal.x = cosPhi * sinTheta;
			vertexData.normal.y = cosTheta;
			vertexData.normal.z = sinPhi * sinTheta;

			vertices.push_back(vertexData);
		}
	}

	std::list<uint32_t> indexData{};
	for (int i = 0; i < vertical; ++i) {
		for (int j = 0; j < horizontal; ++j) {

			indexData.push_back(i * (horizontal + 1) + j);
			indexData.push_back(i * (horizontal + 1) + j + 1);
			indexData.push_back((i + 1) * (horizontal + 1) + j);

			indexData.push_back(i * (horizontal + 1) + j + 1);
			indexData.push_back((i + 1) * (horizontal + 1) + j + 1);
			indexData.push_back((i + 1) * (horizontal + 1) + j);

		}
	}

	TramsformMatrixData matrixData{};
	matrixData.world = worldMatrix;
	matrixData.wvp = wvpMatrix;

	PSOConfig pso{};

	MakeDrawData(drawType, { vertices, indexData }, material, pso, matrixData, textureHandle);

}

void DrawCommandManager::DrawModel(int modelHandle, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, std::vector<int> textureIDs) {
	TramsformMatrixData matrixData{};
	matrixData.world = worldMatrix;
	matrixData.wvp = wvpMatrix;

	PSOConfig pso{};
	if (material.color.w < 1.0f) {
		pso.depthStencilID = DepthStencilID::Transparent;
	}

	ModelDrawConfig drawConfig{};
	drawConfig.pso = pso;
	drawConfig.material = material;
	drawConfig.matrixData = matrixData;
	drawConfig.textureIDs = textureIDs;

	//画像について
	int textureNum = static_cast<int>(manager_->GetModelData(modelHandle).material.size());
	//modelが要求するテクスチャサイズと違う場合は合わせる
	if (textureNum != textureIDs.size()) {
		drawConfig.textureIDs.resize(textureNum);

		//足りてなかった場合はWhite1x1を入れる
		if (textureNum > textureIDs.size()) {
			for(int i = static_cast<int>(textureIDs.size()); i < textureNum; ++i) {
				drawConfig.textureIDs[i] = WHIETE1x1;
			}
		}
	}

	modelDrawQueue_[drawConfig].push_back(modelHandle);
}

void DrawCommandManager::DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle, bool isOffScreen) {
	int drawType = MyDirectX::kSphere;

	std::list<VertexData> vertexData;

	VertexData vertex{};

	//左下
	vertex.position = lb;
	vertex.texcoord = { 0.0f, 1.0f };
	vertex.normal = { 0.0f, 0.0f, -1.0f };
	vertexData.push_back(vertex);

	//左上
	vertex.position = lt;
	vertex.texcoord = { 0.0f, 0.0f };
	vertexData.push_back(vertex);

	//右下
	vertex.position = rb;
	vertex.texcoord = { 1.0f, 1.0f };
	vertexData.push_back(vertex);

	//右上
	vertex.position = rt;
	vertex.texcoord = { 1.0f, 0.0f };
	vertexData.push_back(vertex);

	std::list<uint32_t> indexData;
	indexData.push_back(0);
	indexData.push_back(1);
	indexData.push_back(2);
	indexData.push_back(1);
	indexData.push_back(3);
	indexData.push_back(2);

	PSOConfig pso{};
	pso.isOffScreen = isOffScreen;

	TramsformMatrixData matrixData{};
	matrixData.world = worldMatrix;
	matrixData.wvp = wvpMatrix;

	MakeDrawData(drawType, { vertexData, indexData }, material, pso, matrixData, textureHandle);
}

void DrawCommandManager::DrawPrism(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle) {
	int drawType = MyDirectX::kPrism;

	std::list<VertexData> vertexData;

	VertexData vertex;

	//上段
	vertex.position = { 0.0f, 1.0f, 0.0f, 1.0f };
	vertex.texcoord = { 0.5f, 0.0f };
	vertex.normal = { 0.0f, 1.0f, 0.0f };
	vertexData.push_back(vertex);

	float pie = 3.14159265358f;

	//中段
	for (int i = 1; i < 6; ++i) {
		vertex.position = { cosf(pie * 2.0f * (i - 1) / 4.0f) / 1.0f, 0.0f, sinf(pie * 2.0f * (i - 1) / 4.0f) / 1.0f, 1.0f };
		vertex.texcoord = { static_cast<float>(i - 1) / 5.0f, 0.5f };
		vertex.normal = { cosf(pie * 2.0f * (i - 1) / 4.0f), 0.0f, sinf(pie * 2.0f * (i - 1) / 4.0f) };
		vertexData.push_back(vertex);
	}

	//下段
	vertex.position = { 0.0f, -1.0f, 0.0f, 1.0f };
	vertex.texcoord = { 0.5f, 1.0f };
	vertex.normal = { 0.0f, -1.0f, 0.0f };
	vertexData.push_back(vertex);

	std::list<uint32_t> indexData;

	for (int i = 0; i < 4; ++i) {
		indexData.push_back(0);
		indexData.push_back(i + 2);
		indexData.push_back(i + 1);
	}

	for (int i = 4; i < 8; ++i) {
		indexData.push_back(6);
		indexData.push_back(i + 1 - 4);
		indexData.push_back(i + 2 - 4);
	}

	TramsformMatrixData matrixData{};
	matrixData.world = worldMatrix;
	matrixData.wvp = wvpMatrix;

	PSOConfig pso{};

	MakeDrawData(drawType, { vertexData, indexData }, material, pso, matrixData, textureHandle);
}

void DrawCommandManager::DrawBox(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle) {
	int drawType = MyDirectX::kBox;

	std::list<VertexData> vertexData;

	std::list<uint32_t> indexData;

	const Vector2 texcoords[4] = {
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f}
	};


	const Vector3 normals[6] = {
		{ 0,  0, -1},
		{ 0,  0,  1},
		{-1,  0,  0},
		{ 1,  0,  0},
		{ 0,  1,  0},
		{ 0, -1,  0}
	};

	const Vector2 faceVerts[4] = {
		{-0.5f, 0.5f}, // 左下
		{0.5f, 0.5f}, // 右下
		{-0.5f, -0.5f}, // 右上
		{0.5f, -0.5f}  // 左上
	};

	VertexData vertex;

	int index = 0;
	for (int face = 0; face < 6; ++face) {
		for (int i = 0; i < 4; ++i) {
			Vector3 pos;

			float x = faceVerts[i].x;
			float y = faceVerts[i].y;

			switch (face) {
			case 0: pos = { -x, y, -0.5f }; break; // back
			case 1: pos = { x, y,  0.5f }; break; // front
			case 2: pos = { -0.5f, y, x }; break; // left
			case 3: pos = { 0.5f, y, -x }; break; // right
			case 4: pos = { -x,  0.5f, y }; break; // top
			case 5: pos = { x, -0.5f, y }; break; // bottom
			}

			vertex = {
				{pos.x, pos.y, pos.z, 1.0f},
				texcoords[i],
				normals[face]
			};

			vertexData.push_back(vertex);
		}
	}

	for (int face = 0; face < 6; ++face) {
		int base = face * 4;
		// 三角形1
		indexData.push_back(base + 0);
		indexData.push_back(base + 2);
		indexData.push_back(base + 3);
		
		// 三角形2
		indexData.push_back(base + 0);
		indexData.push_back(base + 3);
		indexData.push_back(base + 1);
	}

	TramsformMatrixData matrixData{};
	matrixData.world = worldMatrix;
	matrixData.wvp = wvpMatrix;

	PSOConfig pso{};

	MakeDrawData(drawType, { vertexData, indexData }, material, pso, matrixData, textureHandle);
}

void DrawCommandManager::DrawLine(Vector4 start, Vector4 end, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle) {
	int drawType = MyDirectX::kLine;

	std::list<VertexData> vertexData;
	VertexData vertex{};

	vertex.position = start;
	vertex.texcoord = { 0.0f, 0.0f };
	vertex.normal = { 0.0f, 0.0f, -1.0f };
	vertexData.push_back(vertex);

	vertex.position = end;
	vertex.texcoord = { 1.0f, 0.0f };
	vertexData.push_back(vertex);

	TramsformMatrixData matrixData{};
	matrixData.world = worldMatrix;
	matrixData.wvp = wvpMatrix;

	PSOConfig pso;
	pso.topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	MakeDrawData(drawType, { vertexData }, material, pso, matrixData, textureHandle);
}

std::vector<std::unordered_map<DrawConfig, std::list<DrawData>>> DrawCommandManager::pushQueue() {
	std::vector<std::unordered_map<DrawConfig, std::list<DrawData>>> queue = std::move(primitiveDrawQueue_);
	
	for(auto& drawMap : primitiveDrawQueue_) {
		drawMap.clear();
	}

	return queue;
}

std::unordered_map<ModelDrawConfig, std::list<int>> DrawCommandManager::pushModelQueue() {
	std::unordered_map<ModelDrawConfig, std::list<int>> queue = std::move(modelDrawQueue_);
	modelDrawQueue_.clear();
	return queue;
}

void DrawCommandManager::MakeDrawData(int drawType, DrawData drawData, MaterialData material, PSOConfig pso, TramsformMatrixData data, int textureID) {
	if (material.color.w < 1.0f) {
		pso.depthStencilID = DepthStencilID::Transparent;
	}

	if (drawType == MyDirectX::kSphere) {
		pso.vs = "Sphere.VS.hlsl";
	}

	DrawConfig drawConfig{};
	drawConfig.pso = pso;
	drawConfig.material = material;
	drawConfig.matrixData = data;
	drawConfig.textureID = textureID;

	if(primitiveDrawQueue_.size() <= static_cast<size_t>(drawType)) {
		primitiveDrawQueue_.resize(drawType + 1);
	}

	primitiveDrawQueue_[drawType][drawConfig].push_back(drawData);
}
