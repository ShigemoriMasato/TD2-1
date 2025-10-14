#include "ModelData.h"
#include <Core/DXCommonFunction.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>
#include <numbers>

void ModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager, DXDevice* device) {
    Assimp::Importer importer;
    std::string path = (directoryPath + "/" + filename);
    const aiScene* scene = nullptr;
    scene = importer.ReadFile(path.c_str(), aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);

	LoadMaterial(scene, directoryPath, textureManager);

	aiNode* node = scene->mRootNode;
    rootNode_ = LoadNode(node, scene);
	skeleton_ = CreateSkeleton(rootNode_);

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

Node ModelData::LoadNode(aiNode* node, const aiScene* scene) {

    Node result{};
    result.name = node->mName.C_Str();
	aiVector3D scale, position;
	aiQuaternion rotation;
    node->mTransformation.Decompose(scale, rotation, position);
	result.scale = { scale.x,scale.y,scale.z };
    result.rotation = { rotation.w,rotation.x,rotation.y,rotation.z };
    result.translation = { position.x,position.y,position.z };

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




            //ボーンの情報を頂点に登録
            for (unsigned int w = 0; w < aiBone->mNumWeights; w++) {
                unsigned int vId = aiBone->mWeights[w].mVertexId + indexOffset;
                float weight = aiBone->mWeights[w].mWeight;

                // 4ウェイト制限
                for (int j = 0; j < 4; j++) {
                    if (vertices_[materialName][vId].boneIndex[j] == -1) {
                        vertices_[materialName][vId].boneIndex[j] = -1;
                        vertices_[materialName][vId].boneWeight[j] = 0.0f;
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

    nodeCount_++;

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        result.chileren.push_back(LoadNode(node->mChildren[i], scene));
    }

    return result;
}

Skeleton ModelData::CreateSkeleton(const Node& rootNode) {
    Skeleton skeleton{};
    skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

    //名前とindexのマッピングを行いアクセスしやすくする
    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

	return skeleton;
}

int32_t ModelData::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
    Joint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeltonSpaceMatrix = MakeIdentity4x4();
	joint.scale = node.scale;
	joint.rotation = node.rotation;
    joint.translation = node.translation;
    joint.index = static_cast<int32_t>(joints.size());
    joint.parent = parent;
	joints.push_back(joint);
    for (const Node& child : node.chileren) {
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
    }
	return joint.index;
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
