#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Transform/Transform.h>
#include <Resource/Texture/TextureManager.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

struct ModelMaterial final {
	std::string name;
	int textureHandle = -1;
};

class ModelData {
public:

	ModelData() = default;
	~ModelData() = default;

	virtual void LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager);

	std::unordered_map<std::string, std::vector<VertexData>> GetVertices() const { return vertices; }
	void SetTextureHandle(std::string materialName, int textureHandle);
	int GetTextureHandle(std::string materialName) const;

protected:
	//マテリアルもいっしょにやる
	void LoadVertex(const aiScene* scene, std::string directoryPath, TextureManager* textureManager);

	std::unordered_map<std::string, std::vector<VertexData>> vertices;
	std::vector<ModelMaterial> material{};

};

