#include "ModelLoader.h"
#include <Data/Transform.h>
#include <cassert>
#include <fstream>
#include <sstream>

//white1x1用
#include "../Render/MyDirectX.h"

ModelLoader::ModelLoader(ModelManager* manager) {
    modelManager_ = manager;
}

int ModelLoader::LoadModel(const char* filePath) {
    ModelData modelData;                    //構築するデータ
    std::vector<Vector4> positions;         //位置
    std::vector<Vector3> normals;           //法線
    std::vector<Vector2> texcoords;         //テクスチャ座標
    std::string line;                       //ファイルから読んだ行を格納するバッファ
	std::string directoryPath;              //ディレクトリパスを格納する変数

    std::string directory = std::filesystem::path(filePath).parent_path().string();

    std::ifstream file(basePath_ + filePath); //ファイルを開く
    assert(file.is_open() && "MyDirectX::LoadObjFile / cannot open obj file");

    std::string materialName; //マテリアル名を格納する変数

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier; //行の先頭の文字列を取得

        if (identifier == "usemtl") {
            s >> materialName; //マテリアル名を取得

        } else if (identifier == "v") {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.x *= -1.0f;
            position.z *= -1.0f;
            position.w = 1.0f;
            positions.push_back(position); //位置を格納

        } else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y; //テクスチャ座標を格納
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);

        } else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z; //法線を格納
            normal.x *= -1.0f; //y軸も反転
            normals.push_back(normal);

        } else if (identifier == "f") {

            //面は三角形限定なので、読み込む前にEditor等で三角化させること。そのほかは未対応
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                s >> vertexDefinition; //頂点の定義を取得

                //頂点の要素へのIndexは。「位置/UV/法線」で格納されているので、分解してIndexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');// /区切りでインデックスを読む
                    if (index != "") {
                        elementIndices[element] = std::stoi(index);
                    } else {
                        elementIndices[element] = -1;
                    }
                }


                //要素へのIndexから、実際の要素の値を取得して頂点を構築する
                Vector4 position = positions[elementIndices[0] - 1];

                Vector2 texcoord;
                if (elementIndices[1] != -1) {
                    texcoord = texcoords[elementIndices[1] - 1];
                } else {
                    texcoord = { 0.0f, 0.0f }; //テクスチャ座標がない場合はデフォルト値を設定
                }

                Vector3 normal = normals[elementIndices[2] - 1];

                VertexData vertex = { position, texcoord, normal };
                modelData.vertices[materialName].push_back(vertex); //頂点を格納

            }
        } else if (identifier == "mtllib") {
            std::string materialFilename;
            s >> materialFilename;
			
            modelData.material = LoadMaterialTemplateFile(directoryPath + materialFilename); //マテリアルファイルを読み込む
        }
    }

    int handle = modelManager_->AddModelData(modelData);

    drawResourceManager_->AddModelKind(modelData, handle);

    return handle;
}

std::vector<ModelMaterial> ModelLoader::LoadMaterialTemplateFile(std::string filePath) {
    std::vector<ModelMaterial> material = {};
    int index = -1;
    std::string textureFilePath;
    std::string line;
    std::ifstream file(filePath); //ファイルを開く
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
            //todo 画像の読み込み
        }
    }

    file.close();

    //マテリアルが一つもない場合はデフォルトのマテリアルを作成
    if (material.empty()) {
        material.push_back(ModelMaterial());
        index = 0;
        material[index].textureHandle = WHIETE1x1; //白い1x1のテクスチャを使う
    }

    //テクスチャが読み込めなかった場合は白い1x1のテクスチャを使う
    if (material[index].textureHandle == 0) {
        material[index].textureHandle = WHIETE1x1;
    }

    return material;
}
