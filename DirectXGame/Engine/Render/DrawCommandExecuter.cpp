#include "DrawCommandExecuter.h"
#include <Math/MyMath.h>

DrawCommandExecuter::DrawCommandExecuter(DrawCommandManager* cm, ModelManager* mm, TextureManager* tm, PSOEditor* pe, DrawResourceManager* drm) {
	commandManager_ = cm;
	modelManager_ = mm;
	textureManager_ = tm;
	psoEditor_ = pe;
	resourceManager_ = drm;
}

void DrawCommandExecuter::ExecuteCommands(ID3D12GraphicsCommandList* commandList) {
	auto primitiveCommands = commandManager_->pushQueue();

	for (int i = 0; i < primitiveCommands.size(); ++i) {
		for(auto& [drawConfig, vertices] : primitiveCommands[i]) {

			auto resources = resourceManager_->GetPrimitiveResource(i);

			//Sphereは行列を送る。それ以外はここで計算してしまう。
			if (i != MyDirectX::kSphere) {
				for (auto& vertex : vertices) {
					for (auto& v : vertex.vertex) {

						v.position *= drawConfig.matrixData.world * drawConfig.matrixData.wvp;
						v.normal *= v.normal * drawConfig.matrixData.world;

					}
				}
			} else {

				TramsformMatrixData* wvpData = nullptr;
				resources[static_cast<int>(ResourceType::Matrix)]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
				wvpData->world = drawConfig.matrixData.world;
				wvpData->wvp = drawConfig.matrixData.wvp;

			}

			VertexData* vertexData = nullptr;
			resources[static_cast<int>(ResourceType::Vertex)]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
			int vertexNum = 0;


			for (auto& vertex : vertices) {
				for (auto& v : vertex.vertex) {
					vertexData[vertexNum++] = v;
				}
			}

			//頂点のバッファビューを作成する
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
			//リソースの先頭のアドレスから使う
			vertexBufferView.BufferLocation = resources[static_cast<int>(ResourceType::Vertex)]->GetGPUVirtualAddress();
			//使用するリソースのサイズは頂点3つ分のサイズ
			vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum;
			//1頂点当たりのサイズ
			vertexBufferView.StrideInBytes = sizeof(VertexData);

			MaterialData* materialData = nullptr;
			resources[static_cast<int>(ResourceType::Material)]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
			*materialData = drawConfig.material;

			DirectionalLightData* directionalLightData = nullptr;
			resources[static_cast<int>(ResourceType::DirectionalLight)]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
			*directionalLightData = {};

			psoEditor_->SetConfig(drawConfig.pso);
			psoEditor_->Setting(commandList);

			//インデックスのバッファビューを作成する
			if (resources[static_cast<int>(ResourceType::Index)]) {
				uint32_t* indexData = nullptr;
				resources[static_cast<int>(ResourceType::Index)]->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
				int indexNum = 0;

				//! indexが複数ある場合対応できていない(複数個描画するとき、二つ目以降が一個目のindexを指定するためつ開けない)
				for(auto& vertex : vertices) {
					for(auto& i : vertex.index) {
						indexData[indexNum++] = i;
					}
				}

				D3D12_INDEX_BUFFER_VIEW indexBufferView{};
				indexBufferView.BufferLocation = resources[static_cast<int>(ResourceType::Index)]->GetGPUVirtualAddress();
				indexBufferView.SizeInBytes = sizeof(uint32_t) * indexNum;
				indexBufferView.Format = DXGI_FORMAT_R32_UINT;

				commandList->IASetIndexBuffer(&indexBufferView);
			}

			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		}
	}
}
