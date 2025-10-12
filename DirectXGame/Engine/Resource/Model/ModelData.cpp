#include "ModelData.h"
#include <Core/DXCommonFunction.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>
#include <numbers>

namespace {
    void FindBoneParents(aiNode* node, std::unordered_map<std::string, int>& boneMap, std::vector<Bone>& bones, int parentIndex = -1) {
        std::string nodeName = node->mName.C_Str();

        if (boneMap.contains(nodeName)) {
            int boneIndex = boneMap[nodeName];
            bones[boneIndex].parentIndex = parentIndex;
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            FindBoneParents(node->mChildren[i], boneMap, bones, boneMap.contains(nodeName) ? boneMap[nodeName] : parentIndex);
        }
    }
}

void ModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager, DXDevice* device) {
    Assimp::Importer importer;
    std::string path = (directoryPath + "/" + filename);
    const aiScene* scene = nullptr;
    scene = importer.ReadFile(path.c_str(), aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);

	LoadMaterial(scene, directoryPath, textureManager);

	aiNode* node = scene->mRootNode;
    LoadNode(node, scene);
	FindBoneParents(scene->mRootNode, boneMap_, bones_);

    //頂点がなかったら要素を削除する
    for(auto& [material, vertex] : vertices_) {
        if (vertex.size() == 0) {

            //頂点
			vertices_.erase(material);

			//インデックス
            for (auto& [mat, index] : indices_) {
                if (mat == material) {
                    indices_.erase(mat);
					break;
                }
            }

			//マテリアル
            for (int i = 0; i < material_.size(); ++i) {
                if (material_[i].name == material) {
					material_.erase(material_.begin() + i);
                }
			}
        }
	}

	CreateID3D12Resource(device->GetDevice());
}

void ModelData::LoadMaterial(const aiScene* scene, std::string directoryPath, TextureManager* textureManager) {
    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];

        this->material_.push_back(ModelMaterial());
        this->material_[materialIndex].name = material->GetName().C_Str();

        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString textureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
            this->material_.back().textureHandle = textureManager->LoadTexture(directoryPath + "/" + std::string(textureFilePath.C_Str()));
        } else {
            this->material_.back().textureHandle = 0; //テクスチャがなかったら白テクスチャを使う
        }
    }

}

void ModelData::LoadNode(aiNode* node, const aiScene* scene, int parentIndex) {

    Node result{};
    result.name = node->mName.C_Str();
	result.parentIndex = parentIndex;
    aiMatrix4x4 mat = node->mTransformation;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.localMatrix.m[i][j] = mat[j][i];
        }
    }

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::string materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();

        for (uint32_t v = 0; v < mesh->mNumVertices; ++v) {
            aiVector3D pos = mesh->mVertices[v];
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[v] : aiVector3D(0, 1, 0);
            aiVector3D tex = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][v] : aiVector3D(0, 0, 0);

            ModelVertexData vertexData{};
            vertexData.position = { pos.x,pos.y,pos.z,1.0f };
            vertexData.normal = { normal.x,normal.y,normal.z };
            vertexData.texcoord = { tex.x,tex.y };

            vertexData.nodeIndex = nodeCount_;

            vertices_[materialName].push_back(vertexData);
        }

        int indexOffset = static_cast<int>(vertices_[materialName].size()) - mesh->mNumVertices;

        // ボーンごとのウェイト登録
        for (unsigned int i = 0; i < mesh->mNumBones; i++) {
            aiBone* aiBone = mesh->mBones[i];
            std::string boneName = aiBone->mName.C_Str();

            int boneIndex = 0;
            if (boneMap_.contains(boneName)) {
                boneIndex = boneMap_[boneName];
            } else {
                Bone bone{};
                bone.name = boneName;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        bone.offsetMatrix.m[i][j] = aiBone->mOffsetMatrix[j][i];
                    }
                }
                bone.nodeIndex = nodeCount_;
                boneMap_[boneName] = static_cast<int>(bones_.size());
                boneIndex = boneMap_[boneName];
                bones_.push_back(bone);
            }

            //ボーンの情報を頂点に登録
            for (unsigned int w = 0; w < aiBone->mNumWeights; w++) {
                unsigned int vId = aiBone->mWeights[w].mVertexId + indexOffset;
                float weight = aiBone->mWeights[w].mWeight;

                // 4ウェイト制限
                for (int j = 0; j < 4; j++) {
                    if (vertices_[materialName][vId].boneIndex[j] == -1) {
                        vertices_[materialName][vId].boneIndex[j] = boneIndex;
                        vertices_[materialName][vId].boneWeight[j] = weight;
                        break;
                    }
                }
            }
        }

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3); //三角形以外は非対応

            for (unsigned int v = 0; v < face.mNumIndices; v++) {
                int localIndex = face.mIndices[v] + indexOffset;
                indices_[materialName].push_back(localIndex);
            }//vertex

        }//face

    }//mesh

    nodeMap_[result.name] = nodeCount_;
    nodes_.push_back(result);
    nodeCount_++;
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        LoadNode(node->mChildren[i], scene, nodeMap_[result.name]);
    }
}

void ModelData::CreateID3D12Resource(ID3D12Device* device) {
    for (auto& [material, vertex] : vertices_) {
        VertexResource res{};
        //超点数
        res.vertexNum = static_cast<int>(vertex.size());

        //頂点リソースの作成
        res.resource.Attach(CreateBufferResource(device, sizeof(ModelVertexData) * res.vertexNum));

        //データの読み込み
		ModelVertexData* mappedData = nullptr;
        res.resource->Map(0, nullptr, (void**)&mappedData);
        memcpy(mappedData, vertex.data(), sizeof(ModelVertexData) * res.vertexNum);
		res.resource->Unmap(0, nullptr);

        //BufferViewの作成
		res.bufferView = std::make_shared<D3D12_VERTEX_BUFFER_VIEW>();
        res.bufferView->BufferLocation = res.resource->GetGPUVirtualAddress();
        res.bufferView->SizeInBytes = sizeof(ModelVertexData) * res.vertexNum;
        res.bufferView->StrideInBytes = sizeof(ModelVertexData);

		//登録
		vertexBufferViews_[material] = res;
    }

    for (auto& [material, index] : indices_) {
        IndexResource res{};
        //インデックス数
        res.indexNum = static_cast<int>(index.size());

        //頂点リソースの作成
        res.indexBuffer.Attach(CreateBufferResource(device, sizeof(uint32_t) * res.indexNum));

        //データの読み込み
		uint32_t* mappedData = nullptr;
        res.indexBuffer->Map(0, nullptr, (void**)&mappedData);
		memcpy(mappedData, index.data(), sizeof(uint32_t) * res.indexNum);
		res.indexBuffer->Unmap(0, nullptr);

        //BufferViewの作成
		res.bufferView = std::make_shared<D3D12_INDEX_BUFFER_VIEW>();
        res.bufferView->BufferLocation = res.indexBuffer->GetGPUVirtualAddress();
        res.bufferView->SizeInBytes = sizeof(uint32_t) * res.indexNum;
		res.bufferView->Format = DXGI_FORMAT_R32_UINT;

		//登録
		indexBufferViews_[material] = res;
    }
}
