#include "DivisionEnemy.h"
#include "../EnemySpawnParams.h"
#include <Tools/FPS/FPSObserver.h>

void DivisionEnemy::Initialize(ModelData* modelData, Camera* camera)
{
	BaseObject::Initialize(modelData, camera);

	transform_.position = { 0.0f, 0.0f, 0.0f };
	transform_.rotation = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	isActive_ = true;
	isAlive_ = true;
	isDead_ = false;

	// 初期Transform設定をModelResourceに反映
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;
	}
}

void DivisionEnemy::Update(float deltaTime)
{
	if (!isActive_ || !isAlive_) return;

	// テスト用：スペースキーで死亡
	if (keys_[Key::Action]) {
		Die();
		return;
	}

	// 簡単なアニメーション：Y軸回転
	transform_.rotation.y += 1.0f * deltaTime;

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;
	}
}

void DivisionEnemy::Draw(Render* render)
{
	if (!isActive_ || !isAlive_) return;
	BaseObject::Draw(render);
}

void DivisionEnemy::Die()
{
	if (!isAlive_) return;

	isDead_ = true;
	isAlive_ = false;  // 生存フラグをfalseに
	// isActive_はそのまま（削除処理はEnemyManagerが行う）

	// 分裂可能な場合は、コールバックを呼び出して2体の敵を生成
	if (canDivide_ && divisionCallback_) {
		// 元の位置から左右に少しずらした位置に2体生成
		Vector3 leftPosition = transform_.position;
		leftPosition.x -= 1.5f;

		Vector3 rightPosition = transform_.position;
		rightPosition.x += 1.5f;

		// 左側の敵を生成（isLeft = true）
		divisionCallback_(leftPosition, true);

		// 右側の敵を生成（isLeft = false）
		divisionCallback_(rightPosition, false);
	}
}

void DivisionEnemy::SetPosition(const Vector3& position)
{
	transform_.position = position;
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
	}
}

void DivisionEnemy::Configure(const EnemySpawnParams& params)
{
	// 基本的な Transform 設定
	transform_.position = params.position;
	transform_.rotation = params.rotation;
	transform_.scale = params.scale;

	// DivisionEnemy 固有のパラメータ設定
	bool canDivide = params.GetBool("canDivide", true);
	SetCanDivide(canDivide);

	// ModelResource に反映
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->scale_ = transform_.scale;
	}
}



