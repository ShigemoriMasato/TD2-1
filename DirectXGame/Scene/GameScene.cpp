#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"

using namespace Matrix;

namespace {
	int useImGui = 0;

	void DrawValues(std::vector<ValueBase*>& values) {
		for (int i = 0; i < values.size(); ++i) {
			ImGui::Text("%s", values[i]->name.c_str());

			ImGui::SameLine();
			ImGui::PushID(useImGui++);
			if (dynamic_cast<Value<int>*>(values[i])) {
				ImGui::InputInt("##int", &dynamic_cast<Value<int>*>(values[i])->value);
			} else if (dynamic_cast<Value<float>*>(values[i])) {
				ImGui::InputFloat("##float", &dynamic_cast<Value<float>*>(values[i])->value);
			} else if (dynamic_cast<Value<Vector2>*>(values[i])) {
				ImGui::InputFloat2("##Vector2", &dynamic_cast<Value<Vector2>*>(values[i])->value.x);
			} else if (dynamic_cast<Value<Vector3>*>(values[i])) {
				ImGui::InputFloat3("##Vector3", &dynamic_cast<Value<Vector3>*>(values[i])->value.x);
			}
			ImGui::PopID();
			ImGui::SameLine();
			
			ImGui::PushID(useImGui++);
			if (ImGui::Button("Delete")) {
				delete values[i];
				values.erase(values.begin() + i);
				--i; // 削除したのでインデックスを調整
			}
			ImGui::PopID();
		}
	}

	std::vector<ValueBase*> MakeSameValues(std::vector<ValueBase*> values) {

		std::vector<ValueBase*> ans;

		for (auto& v : values) {
			if (dynamic_cast<Value<int>*>(v)) {
				ans.push_back(new Value<int>(dynamic_cast<Value<int>*>(v)->value, v->name));
			} else if (dynamic_cast<Value<float>*>(v)) {
				ans.push_back(new Value<float>(dynamic_cast<Value<float>*>(v)->value, v->name));
			} else if (dynamic_cast<Value<Vector2>*>(v)) {
				ans.push_back(new Value<Vector2>(dynamic_cast<Value<Vector2>*>(v)->value, v->name));
			} else if (dynamic_cast<Value<Vector3>*>(v)) {
				ans.push_back(new Value<Vector3>(dynamic_cast<Value<Vector3>*>(v)->value, v->name));
			}
		}

		return ans;
	}
}

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()),
debugCamera_(new DebugCamera()) {
	isDebugCamera = true;
}

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;

	for (ValueBase* value : values_) {
		delete value;
	}

	for (Struct& s : structs_) {
		for (ValueBase* value : s.members) {
			delete value;
		}
	}
}

void GameScene::Initialize() {
	debugCamera_->Initialize();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
}

std::unique_ptr<Scene> GameScene::Update() {

	if (Input::GetKeyState(DIK_R)) {
		Initialize();
	}

	ImGui::Begin("Camera");
	ImGui::Checkbox("Debug Camera", &isDebugCamera);
	ImGui::End();


	//以下Valueに関するImGUi

	const char* types[] = {
		"int",
		"float",
		"Vector2",
		"Vector3"
	};

	ImGui::Begin("AddValue");

	ImGui::Combo("Type", &typeIndex, types, 4);

	switch (typeIndex) {
	case 0:
		ImGui::InputInt("Value", &intbuffer);
		break;

	case 1:
		ImGui::InputFloat("Value", &floatbuffer);
		break;

	case 2:
		ImGui::InputFloat2("Value", &vector2buffer.x);
		break;

	case 3:
		ImGui::InputFloat3("Value", &vector3buffer.x);
		break;
	}

	ImGui::InputText("Name", valueNamebuffer, 256);

	if (ImGui::Button("Add")) {
		switch (typeIndex) {
		case 0:

			values_.push_back(new Value<int>(intbuffer, valueNamebuffer));

			break;
		case 1:

			values_.push_back(new Value<float>(floatbuffer, valueNamebuffer));

			break;
		case 2:

			values_.push_back(new Value<Vector2>(vector2buffer, valueNamebuffer));

			break;
		case 3:

			values_.push_back(new Value<Vector3>(vector3buffer, valueNamebuffer));

			break;
		}
	}

	ImGui::End();

	ImGui::Begin("StructAdd");
	ImGui::InputText("Struct Name", structNamebuffer, 256);
	if (ImGui::Button("Add")) {
		structs_.push_back(Struct(structNamebuffer, values_));
		values_ = MakeSameValues(values_);
	}
	ImGui::End();

	ImGui::Begin("Values");
	DrawValues(values_);
	ImGui::End();

	for (int i = 0; i < structs_.size(); ++i) {
		ImGui::Begin(structs_[i].name.c_str());
		DrawValues(structs_[i].members);
		if(ImGui::Button("DeleteThisStruct")) {
			structs_.erase(structs_.begin() + i);
		}
		ImGui::End();
	}

	//以上Valueに関するImGUi

	if (isDebugCamera) {
		debugCamera_->Update();
		*camera_ = *debugCamera_;
	}

	return nullptr;
}

void GameScene::Draw() const {
	useImGui = 0;
}
