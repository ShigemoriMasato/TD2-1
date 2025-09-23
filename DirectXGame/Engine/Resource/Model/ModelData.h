#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Transform/Transform.h>
#include <Resource/Texture/TextureManager.h>

struct ModelMaterial final {
	std::string name;
	int textureHandle;
};

class ModelData {
public:

	ModelData() = default;
	~ModelData() = default;

	void LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager);

	std::unordered_map<std::string, std::vector<VertexData>> GetVertices() const { return vertices; }
	int GetTextureHandle(std::string materialName) const;

private:

	std::vector<ModelMaterial> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager);

	std::unordered_map<std::string, std::vector<VertexData>> vertices;
	std::vector<ModelMaterial> material{};

};

