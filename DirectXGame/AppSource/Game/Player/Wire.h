#pragma once
#include "../BaseObject.h"
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

#include <memory>

class Wire : public BaseObject
{
public:

	void Initialize(ModelData* modelData, Camera* camera)override;

	void Update(float deltaTime) override;
	std::pair<Transform, float>GetCollider()const { return { transform_,radius_ }; }

	void SetStartPositionPtr(Vector3* startPos) { startPos_ = startPos; }
	void SetEndPosition(const Vector3& endPos);

	/// <summary>
	/// 伸ばすときの更新処理
	/// </summary>
	/// <param name="endPos">終端(敵とか天井とかの座標)</param>
	/// <returns>伸ばしきったらtrue</returns>
	bool Extended();
	//Extendした後、PlayerがEndPosに到達したら実行。描画しないようにする
	void Shrinked();

private://関数


private://Wire情報
	//始点(基本的にプレイヤー座標)
	Vector3* startPos_ = {};
	//終点(当たった座標)
	Vector3 endPos_ = {};
	//伸ばすときの目標座標
	Vector3 targetPos_ = {};

	//伸ばす速度
	const float extendSpeed_ = 30.0f;
	//伸ばし始めたときの距離
	float initDistance_ = 0.0f;
	//伸ばしているか
	bool isExtending_ = false;
	//見えないようにする
	bool isVisible_ = true;

private:
};

