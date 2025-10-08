#include "ModelData.h"
#include <Core/DXCommonFunction.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

void ModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager) {
    Assimp::Importer importer;
    std::string path = (directoryPath + "/" + filename);
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder);

    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];

		this->material.push_back(ModelMaterial());
        this->material[materialIndex].name = material->GetName().C_Str();

        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString textureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
            this->material.back().textureHandle = textureManager->LoadTexture(directoryPath + "/" + std::string(textureFilePath.C_Str()));
        }
    }

    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
        
		assert(mesh->HasNormals());
        assert(mesh->HasTextureCoords(0));

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); //三角形限定

            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t vertexIndex = face.mIndices[element];

                aiVector3D& position = mesh->mVertices[vertexIndex];
                aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

				VertexData vertex = {};
				vertex.position = { position.x * -1.0f, position.y, position.z * -1.0f, 1.0f };
				vertex.normal = { normal.x * -1.0f, normal.y, normal.z };
				vertex.texcoord = { texcoord.x, 1.0f - texcoord.y };

                vertex.position.x *= -1.0f;
                vertex.normal.x *= -1.0f;

				vertices[material[mesh->mMaterialIndex].name].push_back(vertex);
            }
        }
    }

}

int ModelData::GetTextureHandle(std::string materialName) const {
    for (auto m : material) {
        if (m.name == materialName) {
            return m.textureHandle;
        }
    }
    return -1;
}

std::vector<ModelMaterial> ModelData::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager) {
    std::vector<ModelMaterial> material = {};
    int index = -1;
    std::string textureFilePath;
    std::string line;
    std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
    assert(file.is_open() && "MyDirectX::LoadMaterialTemplateFile cannot open the mtlFile");

    while (std::getline(file, line)) {
        std::string identifier;
        std::stringstream s(line);
        s >> identifier;

        if (identifier == "newmtl") {
            material.push_back(ModelMaterial());
            ++index;
            s >> material[index].name; //新しいマテリアル名を取得
        }
        //identifierに応じて処理
        else if (identifier == "map_Kd") {
            std::string textureFilename;
            s >> textureFilename;
            //連結してファイルパスにする
            textureFilePath = directoryPath + "/" + textureFilename;
			material[index].textureHandle = textureManager->LoadTexture(textureFilePath);
        }
    }

    if (material[index].textureHandle == -1) {
        //テクスチャが読み込めなかった場合は白い1x1のテクスチャを使う
        material[index].textureHandle = 0;
    }

    return material;
}
