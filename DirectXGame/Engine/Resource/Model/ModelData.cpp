#include "ModelData.h"
#include <Core/DXCommonFunction.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>

void ModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager) {
    Assimp::Importer importer;
    std::string path = (directoryPath + "/" + filename);
    const aiScene* scene = nullptr;
    scene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder);

	LoadVertex(scene, directoryPath, textureManager);
}

void ModelData::SetTextureHandle(std::string materialName, int textureHandle) {
    for (auto& m : material) {
        if (m.name == materialName) {
            m.textureHandle = textureHandle;
            return;
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

void ModelData::LoadVertex(const aiScene* scene, std::string directoryPath, TextureManager* textureManager) {
    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];

        this->material.push_back(ModelMaterial());
        this->material[materialIndex].name = material->GetName().C_Str();

        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString textureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
            this->material.back().textureHandle = textureManager->LoadTexture(directoryPath + "/" + std::string(textureFilePath.C_Str()));
        } else {
            this->material.back().textureHandle = 0; //テクスチャがなかったら白テクスチャを使う
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
