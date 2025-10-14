#pragma once
#include "BaseEnemy.h"
#include "../BaseObject.h"
#include <vector>
#include <memory>
#include <string>

class EnemyManager : public BaseObject
{
public:
	EnemyManager();
	~EnemyManager() = default;


	// 新しい初期化メソッド：ModelManagerとCameraを受け取る
	void Initialize(class ModelManager* modelManager, Camera* camera);

	void Update() override;
	void Draw(Render* render) override;

	// 敵を追加（モデル名を指定）
	void AddEnemy(std::unique_ptr<BaseEnemy> enemy, const std::string& modelName);

	// 敵を追加（ModelDataを直接指定）
	void AddEnemy(std::unique_ptr<BaseEnemy> enemy, ModelData* modelData);


	// プレイヤーの位置を全ての敵に通知
	void SetPlayerPosition(const Vector3& playerPos);



	// 全ての敵をクリア
	void ClearEnemies();

private:
	std::vector<std::unique_ptr<BaseEnemy>> enemies_;
	class ModelManager* modelManager_ = nullptr;
	Camera* camera_ = nullptr;
};

