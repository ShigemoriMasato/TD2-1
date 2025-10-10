#include "ModelData.h"
#include <Core/DXCommonFunction.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>

void ModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager, DXDevice* device) {
    Assimp::Importer importer;
    std::string path = (directoryPath + "/" + filename);
    const aiScene* scene = nullptr;
    scene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder | aiProcess_MakeLeftHanded | aiProcess_Triangulate);

	LoadMaterial(scene, directoryPath, textureManager);

	rootNode_ = LoadNode(scene->mRootNode, scene);

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
    result.nodeIndex = nodeCount_;
	aiMatrix4x4 mat = node->mTransformation;
    
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.localMatrix.m[i][j] = mat[j][i];
        }
	}

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        unsigned int meshIndex = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));

        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            ModelVertexData vertex{};

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

			vertex.nodeIndex = nodeCount_;

            vertices_[scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()].push_back(vertex);
        }

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3); //三角形以外は非対応

            for (unsigned int v = 0; v < face.mNumIndices; v++) {
                int localIndex = face.mIndices[v];
				indices_[scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()].push_back(localIndex);
            }//vertex

        }//mesh

    }//node

    nodeCount_++;

    // --- 子ノードを再帰的に処理する ---
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        result.children.push_back(LoadNode(node->mChildren[i], scene));
    }

    return result;
}

void ModelData::CreateID3D12Resource(ID3D12Device* device) {
    for (auto& [material, vertex] : vertices_) {
        VertexResource res{};
        //超点数
        res.vertexNum = static_cast<int>(vertex.size());

        //頂点リソースの作成
        res.resource = CreateBufferResource(device, sizeof(ModelVertexData) * res.vertexNum);

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
        res.indexBuffer = CreateBufferResource(device, sizeof(uint32_t) * res.indexNum);

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
