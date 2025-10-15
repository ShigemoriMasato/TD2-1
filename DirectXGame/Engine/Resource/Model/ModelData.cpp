#include "ModelData.h"
#include <Core/DXCommonFunction.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include <assimp/postprocess.h>

void ModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager, DXDevice* device, SRVManager* srvManager) {
    Assimp::Importer importer;
    std::string path = (directoryPath + "/" + filename);
    const aiScene* scene = nullptr;
    scene = importer.ReadFile(path.c_str(), aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);

	LoadMaterial(scene, directoryPath, textureManager);

	rootNode_ = LoadNode(scene->mRootNode, scene);

    skeleton_ = CreateSkeleton(rootNode_);

	CreateID3D12Resource(device->GetDevice());
	skinCluster_ = CreateSkinCluster(device->GetDevice(), skeleton_, srvManager);
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
    aiVector3D scale, translate;
    aiQuaternion rotate;
    node->mTransformation.Decompose(scale, rotate, translate);
    result.transform.scale = { scale.x, scale.y, scale.z };
    result.transform.rotation = { rotate.x, rotate.y, rotate.z, rotate.w };
    result.transform.position = { translate.x, translate.y, translate.z };
    result.localMatrix = Matrix::MakeAffineMatrix(result.transform);
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        for (uint32_t v = 0; v < mesh->mNumVertices; ++v) {
            aiVector3D pos = mesh->mVertices[v];
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[v] : aiVector3D(0, 1, 0);
            aiVector3D tex = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][v] : aiVector3D(0, 0, 0);

            ModelVertexData vertexData{};
            vertexData.position = { pos.x,pos.y,pos.z,1.0f };
            vertexData.normal = { normal.x,normal.y,normal.z };
            vertexData.texcoord = { tex.x,tex.y };

            vertices_[scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()].push_back(vertexData);
        }

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			int indexOffset = static_cast<int>(vertices_[scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()].size()) - static_cast<int>(mesh->mNumVertices);
            assert(face.mNumIndices == 3); //三角形以外は非対応

            for (unsigned int v = 0; v < face.mNumIndices; v++) {
                int localIndex = face.mIndices[v] + indexOffset;
				indices_[scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()].push_back(localIndex);
            }//vertex

        }//face

        for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            aiBone* bone = mesh->mBones[boneIndex];
            std::string jointName = bone->mName.C_Str();
            JointWeightData& jointWeightData = skinClusterData[jointName];

            aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
            aiVector3D scale, translate;
            aiQuaternion rotate;
            bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
            Matrix4x4 bindPoseMatrix = Matrix::MakeAffineMatrix({
                {scale.x, scale.y, scale.z},
                {rotate.x, rotate.y, rotate.z, rotate.w},
                {translate.x, translate.y, translate.z}
                });
            jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

            for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
                jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
            }
        }//bone

    }//mesh

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

SkinCluster ModelData::CreateSkinCluster(ID3D12Device* device, const Skeleton& skeleton, SRVManager* srvManager) {
    SkinCluster skinCluster{};

    //palette用のResourceを確保
	skinCluster.paletteResource.Attach(CreateBufferResource(device, sizeof(WellForGPU) * skeleton.joints.size()));
    WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
    skinCluster.mappedPalette = { mappedPalette, skeleton.joints.size() };
	skinCluster.paletteSrvHandle.first = srvManager->GetCPUHandle();
	skinCluster.paletteSrvHandle.second = srvManager->GetGPUHandle();

    //palette用のsrvを作成
    D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
    paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    paletteSrvDesc.Buffer.FirstElement = 0;
    paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
    paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.first);

    //influence用のResourceを確保
	skinCluster.influenceResource.Attach(CreateBufferResource(device, sizeof(VertexInfluence) * vertices_.begin()->second.size()));
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * vertices_.begin()->second.size());
	skinCluster.mappedInfluence = { mappedInfluence, vertices_.begin()->second.size() };

    //Influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * vertices_.begin()->second.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

    //InverseBindPoseMatrixを格納する場所を作成して単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size(), Matrix::MakeIdentity4x4());

    for (const auto& jointWeight : skinClusterData) {
		auto it = skeleton.jointMap.find(jointWeight.first);
        if (it == skeleton.jointMap.end()) {
            continue;
        }

        //(*it).secondの中にjointのindexがあるので、該当のindexのinverseBindPoseMatrixを代入する
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
        for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
            auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
            for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
                if (currentInfluence.weights[index] == 0.0f) {
                    currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
                }
            }
        }
    }

    return skinCluster;
}

void SkinCluster::Update(Skeleton& skeleton) {
    for(size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
        assert(jointIndex < inverseBindPoseMatrices.size());
		mappedPalette[jointIndex].skeletonSpaceMatrix = inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Transpose(Inverse(mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}
