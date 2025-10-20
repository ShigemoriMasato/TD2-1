#include "BaseEnemy.h"

void BaseEnemy::OnCollision(BaseObject* other)
{
	// プレイヤーとの衝突時の処理
	// 派生クラスで個別の処理が必要な場合はオーバーライドする
	if (modelResource_) {
		modelResource_->color_ = 0xff00ff00; // 緑色に変更（当たり判定の視覚的確認用）
	}
}

