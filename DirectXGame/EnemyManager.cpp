#include "EnemyManager.h"
#include "externals/imgui/imgui.h"
#include <io.h>
#include <fstream>
#include <iostream>
#include <regex>

namespace {
	std::list<std::string> gciForEnemy(std::string filename) {
		intptr_t handle;
		struct _finddata_t finddata;

		std::string path = "Resources/EnemyProperty/" + filename + "*.csv";

		handle = _findfirst(path.c_str(), &finddata);

		std::list<std::string> result;

		if (handle != -1) {
			result.push_back(finddata.name);
			while (_findnext(handle, &finddata) != -1) {
				result.push_back(finddata.name);
			}
			_findclose(handle);
		}

		return result;
	}
}

EnemyManager::EnemyManager(Camera* camera, Object* player, const CommonData& commondata) {
	player_ = player;
	camera_ = camera;

	enemyModelHandle_ = commondata.modelHandle_[int(ModelType::Enemy)];
	bulletModelHandle_ = commondata.modelHandle_[int(ModelType::Bullet)];

	Enemy::RegistCommands();
}

void EnemyManager::Initialize() {
	enemies_.clear();
	bullets_.clear();

	CreateEnemyQueue();
	CreateStageQueue();
}

void EnemyManager::Update() {

	UpdateStageQueue("Normal");

	for (int i = 0; i < enemies_.size(); ++i) {
		enemies_[i]->Update();

		if (!enemies_[i]->GetIsAlive()) {
			enemies_.erase(enemies_.begin() + i--);
		}
	}

	for (int i = 0; i < bullets_.size(); ++i) {
		bullets_[i]->Update();
		if (!bullets_[i]->GetIsActive()) {
			bullets_.erase(bullets_.begin() + i--);
		}
	}
}

void EnemyManager::Draw() {
	for (const auto& enemy : enemies_) {
		enemy->Draw();
	}
	for (const auto& bullet : bullets_) {
		bullet->Draw();
	}
}

std::list<Object*> EnemyManager::GetEnemiesAndBulletCollition() {
	std::list<Object*> data;

	for (const auto& e : enemies_) {
		if (e->GetIsAlive()) {
			data.push_back(e.get());
		}
	}

	for (const auto& b : bullets_) {
		data.push_back(b.get());
	}

	return data;
}

std::list<Enemy*> EnemyManager::GetEnemiesCollition() {
	std::list<Enemy*> data;
	for (const auto& e : enemies_) {
		if (e->GetIsAlive()) {
			data.push_back(e.get());
		}
	}
	return data;
}

void EnemyManager::CreateEnemyQueue() {

	std::list<std::string> csvs = gciForEnemy("EnemyType/");

	for (const auto& csv : csvs) {

		//CSVファイルを開く
		std::ifstream file("Resources/EnemyProperty/EnemyType/" + csv);

		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << csv << std::endl;
			continue;
		}

		std::stringstream stream;
		stream << file.rdbuf();

		file.close();

		//ファイル名から敵の名前を取得
		EnemyInfo queue;
		std::string name;
		getline(std::stringstream(csv), name, '.');
		queue.name = name;

		//1行目は読み飛ばす
		std::string funcBuffer;
		std::getline(stream, funcBuffer);

		//todo 2行目から敵の見た目を設定する項目を作成する。現在は敵の見た目は1つしかないので、敵の見た目を設定する項目はない。
		queue.handle = enemyModelHandle_; //敵のモデルハンドルを設定

		int lineCount = 1;

		while (getline(stream, funcBuffer)) {
			//この時点ではFire,damage=10,position=1.0 2.0 3.0\n というような形になっているはず。

			++lineCount;

			//コメント行をはじく
			if (funcBuffer.find("//") != std::string::npos) {
				continue; 
			}

			//空行をはじく
			if (funcBuffer.empty()) {
				continue;
			}

			EnemyCommand command;

			std::stringstream func(funcBuffer);

			std::string funcName;
			std::getline(func, funcName, ','); // 行動の名前を取得
			// この時点でfuncの中身はdamage=10,position=1.0 2.0 3.0\n という形になっているはず。

			command.funcName = funcName;

			//関数の引数部分を取得
			std::string arg;
			while (std::getline(func, arg, ',')) {
				//この時点で、funcはposition=1.0 2.0 3.0\nで、argがdamage=10になってるはず
				//argの中身を対象にしたいので、argでstd::stringstreamを作成する
				std::stringstream argStream(arg);

				//フォーマットに従っていないものをはじく
				std::regex format(R"([a-z]+(?:_[a-z()]+)*=-?\d+(?:\.\d+)?(?: -?\d+(?:\.\d+)?){0,4})");
				if (!std::regex_match(arg, format)) {
					continue;
				}

				if(arg.empty()) {
					continue; //空の引数は無視
				}

				// 引数名前の取得
				std::string argName;
				std::getline(argStream, argName, '=');
				//ここで=10になるはず。

				// 引数値の10を取り出す
				std::getline(argStream, arg, ',');

				// 引数が複数の場合
				if (arg.find(' ') != std::string::npos) {

					std::stringstream argNumbers(arg);
					std::vector<float> values;

					while (std::getline(argNumbers, arg, ' ')) {
						values.push_back(std::stof(arg));
					}

					//要素数に応じて型を使い分ける
					command.args.push_back(MakeValue(argName, values));

				}//引数が複数の場合

				//引数が単数の場合
				else {

					//floatの場合
					if (arg.find('.') != std::string::npos) {
						
						command.args.push_back(std::make_shared<FloatValue>(argName, std::stof(arg)));

					} 
					//intの場合
					else {

						command.args.push_back(std::make_shared<IntValue>(argName, std::stoi(arg)));

					}

				}//引数が単数の場合

			}//引数単位while文

			queue.queue.push_back(command);
			queue.queueLines.push_back(lineCount);

		}//行単位while文

		//読み込んだ敵の行動キューを登録
		taskQueues_[queue.name] = queue;

	}//csv単位for文
}

void EnemyManager::CreateStageQueue() {
	std::list<std::string> csvs = gciForEnemy("");
	
	for (auto& csv : csvs) {

		//名前の取得
		std::string stageName;
		std::getline(std::stringstream(csv), stageName, '.');

		//CSVファイルを開く
		std::ifstream file("Resources/EnemyProperty/" + csv);

		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << csv << std::endl;
			continue;
		}

		// ファイルの内容を読み込む
		stageQueues_[stageName] << file.rdbuf();

		file.close();

		//1行目は読み飛ばす
		std::getline(stageQueues_[stageName], csv);
	}
}

void EnemyManager::UpdateStageQueue(std::string stageName) {
	while (!stageQueues_[stageName].eof() && !waiting_) {

		//1行読み込む
		std::string line;
		std::getline(stageQueues_[stageName], line);

		//関数名を読み込む
		std::string funcName;
		std::getline(std::stringstream(line), funcName, ','); // 行動の名前を取得

		if (funcName == "Wait") {
			std::string waitTimeStr;
			std::stringstream stream(line);
			std::getline(stream, waitTimeStr, '='); // 変数名を読み飛ばす
			std::getline(stream, waitTimeStr, ','); // 値を取得

			waitTime_ = std::stoi(waitTimeStr);
			waiting_ = true;

			return;

		} else if (funcName == "Pop") {
			std::string popEnemyTypeStr;
			std::stringstream stream(line);
			std::getline(stream, popEnemyTypeStr, '='); // 変数名を読み飛ばす
			std::getline(stream, popEnemyTypeStr, ','); // 値を取得
			Vector3 position{};
			std::string positionStr;
			std::getline(stream, positionStr, '=');
			std::getline(stream, positionStr, ','); // 値を取得

			std::stringstream positionStream(positionStr);

			for (int i = 0; i < 3; ++i) {
				std::string positionBuffer;
				std::getline(positionStream, positionBuffer, ' ');
				position[i] = std::stof(positionBuffer);
			}

			Transform transform;
			transform.position = position;

			//敵のポップ処理
			enemies_.push_back(std::make_unique<Enemy>(camera_, taskQueues_[popEnemyTypeStr], transform, [this](EnemyBulletDesc desc) { Fire(desc); }));

		} else if (funcName == "Return") {

			stageQueues_[stageName].clear();

			std::ifstream file("Resources/EnemyProperty/" + stageName + ".csv");

			if (!file.is_open()) {
				std::cerr << "Failed to open file: " << stageName << ".csv" << std::endl;
				continue;
			}

			stageQueues_[stageName] << file.rdbuf();

			file.close();

			std::string lineBuffer;

			std::getline(stageQueues_[stageName], lineBuffer);

		}

	}

	if (waiting_) {

		--waitTime_;

		if (waitTime_ < 0) {
			waiting_ = false;
		}

	}
}

void EnemyManager::Fire(EnemyBulletDesc desc) {
	std::unique_ptr<EnemyBullet> bullet = 
		std::make_unique<EnemyBullet>(camera_, *desc.localPosition + desc.position, bulletModelHandle_, player_);

	bullet->Initialize();
	bullets_.push_back(std::move(bullet));
}

