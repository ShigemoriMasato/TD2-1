#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"

using namespace Matrix;

ValueBase* selectedValue = nullptr;

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
			} else if(dynamic_cast<Value<Struct>*>(values[i])) {
				ImGui::Text("This is Struct Type");
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

			ImGui::SameLine();
			ImGui::PushID(useImGui++);
			if (ImGui::Button("Select")) {
				selectedValue = values[i];
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
			} else if (dynamic_cast<Value<Struct>*>(v)) {
				ans.push_back(new Value<Struct>(dynamic_cast<Value<Struct>*>(v)->value, v->name));
			}
		}

		return ans;
	}

	ValueBase* MakeSameValue(ValueBase* value) {
		ValueBase* ans{};

		if (dynamic_cast<Value<int>*>(value)) {
			ans = new Value<int>(dynamic_cast<Value<int>*>(value)->value, value->name);
		} else if (dynamic_cast<Value<float>*>(value)) {
			ans = new Value<float>(dynamic_cast<Value<float>*>(value)->value, value->name);
		} else if (dynamic_cast<Value<Vector2>*>(value)) {
			ans = new Value<Vector2>(dynamic_cast<Value<Vector2>*>(value)->value, value->name);
		} else if (dynamic_cast<Value<Vector3>*>(value)) {
			ans = new Value<Vector3>(dynamic_cast<Value<Vector3>*>(value)->value, value->name);
		} else if (dynamic_cast<Value<Struct>*>(value)) {
			ans = new Value<Struct>(dynamic_cast<Value<Struct>*>(value)->value, value->name);
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


#pragma region 変数追加

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

#pragma endregion

#pragma region 構造体追加

	ImGui::Begin("StructAdd");
	ImGui::InputText("Struct Name", structNamebuffer, 256);
	if (ImGui::Button("Add")) {
		structs_.push_back(Value<Struct>(Struct(values_), structNamebuffer));
		values_ = MakeSameValues(values_);
	}
	ImGui::End();

	ImGui::Begin("Values");
	DrawValues(values_);
	ImGui::End();

	for (int i = 0; i < structs_.size(); ++i) {
		ImGui::Begin(structs_[i].name.c_str());
		DrawValues(structs_[i].value.members);

		//構造体を変数として登録
		if (ImGui::Button("SelectThisStruct")) {
		}
		ImGui::SameLine();

		//構造体の削除
		if(ImGui::Button("DeleteThisStruct")) {
			structs_.erase(structs_.begin() + i);
		}
		ImGui::End();
	}

#pragma endregion

#pragma region 関数追加

	ImGui::Begin("Function");

	//引数の追加
	if (ImGui::Button("SetArgs") && selectedValue) {
		args_.push_back(MakeSameValue(selectedValue));
		selectedValue = nullptr; // 選択解除
	}

	//引数の表示
	DrawValues(args_);

	//関数名入力
	ImGui::InputText("Function Name", functionNamebuffer_, 256);

	//関数登録
	if (ImGui::Button("Add")) {
		funcs_.push_back(Function(functionNamebuffer_, args_, typeid(int)));
		args_.clear();
	}

	ImGui::End();

#pragma endregion

	if (isDebugCamera) {
		debugCamera_->Update();
		*camera_ = *debugCamera_;
	}

	return nullptr;
}

void GameScene::Draw() const {
	useImGui = 0;
}
