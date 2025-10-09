#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Transform/Transform.h>
#include <Resource/Texture/TextureManager.h>
#include <Math/MyMath.h>
#include <Render/Resource/DrawResource.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

struct ModelMaterial final {
	std::string name;
	int textureHandle = -1;
};

struct Node {
	Matrix4x4 localMatrix = Matrix::MakeIdentity4x4();
	std::string name{};
	std::unordered_map<std::string, std::vector<VertexData>> vertices;

	//todo 描画用。今度けす。
	std::unordered_map<std::string, DrawResource*> drawResource;

	std::vector<Node> children{};
};

class ModelData {
public:

	ModelData() = default;
	~ModelData() = default;

	virtual void LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager);

	Node GetParentNode() { return rootNode_; }
	void SetTextureHandle(std::string materialName, int textureHandle);
	int GetTextureHandle(std::string materialName) const;

protected:

	void LoadMaterial(const aiScene* scene, std::string directoryPath, TextureManager* textureManager);

	Node LoadNode(aiNode* node, const aiScene* scene);

	std::vector<ModelMaterial> material{};

	std::vector<uint32_t> meshNodeMap_;

	Node rootNode_{};

};
