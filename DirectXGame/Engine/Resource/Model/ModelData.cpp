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
    scene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder | aiProcess_MakeLeftHanded | aiProcess_Triangulate);

	LoadMaterial(scene, directoryPath, textureManager);

	rootNode_ = LoadNode(scene->mRootNode, scene);
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

void ModelData::LoadMaterial(const aiScene* scene, std::string directoryPath, TextureManager* textureManager) {
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

}

Node ModelData::LoadNode(aiNode* node, const aiScene* scene) {
    Node result{};
	result.name = node->mName.C_Str();
	aiMatrix4x4 mat = node->mTransformation;

    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.localMatrix.m[i][j] = mat[j][i];
        }
	}

    std::unordered_map<std::string, std::vector<VertexData>> vertices;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        unsigned int meshIndex = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));

        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            VertexData vertex{};

            vertex.position.x = mesh->mVertices[v].x;
            vertex.position.y = mesh->mVertices[v].y;
            vertex.position.z = mesh->mVertices[v].z;

            vertex.normal.x = mesh->mNormals[v].x;
            vertex.normal.y = mesh->mNormals[v].y;
            vertex.normal.z = mesh->mNormals[v].z;

            vertex.texcoord.x = mesh->mTextureCoords[0][v].x;
            vertex.texcoord.y = mesh->mTextureCoords[0][v].y;

            vertex.position.x *= -1.0f;
            vertex.normal.x *= -1.0f;

            vertices[""].push_back(vertex);
        }

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3); //三角形以外は非対応

            for (unsigned int v = 0; v < face.mNumIndices; v++) {
                int localIndex = face.mIndices[v];
				//indecies.push_back(localIndex);
            }//vertex

        }//mesh

    }//node

    result.vertices = vertices;

    // --- 子ノードを再帰的に処理する ---
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        result.children.push_back(LoadNode(node->mChildren[i], scene));
    }

    return result;
}
