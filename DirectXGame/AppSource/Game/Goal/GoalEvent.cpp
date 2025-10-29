#include "GoalEvent.h"
#include <Game/Scene/GameScene.h>
#include <Game/Item/Coin.h>

GoalEvent::GoalEvent(CameraManager* camera, Player* player, PostEffectResource* posteffect, TextureManager* textureManager) {
	camera_ = camera;
	player_ = player;
	postEffect_ = posteffect;

	uicamera.SetProjectionMatrix(PerspectiveFovDesc());
	uicamera.MakeMatrix();

	plate_ = std::make_unique<DrawResource>();
	plate_->Initialize(ShapeType::Plane);
	plate_->camera_ = &uicamera;
	plate_->position_ = { 0.0f, 0.0f, 1.5f };
	plate_->scale_ = { 1.0f, 0.6f, 1.0f };
	plate_->SetTextureHandle(textureManager->LoadTexture("Assets/Texture/number/ScorePlate.png"));

	for (int i = 0; i < 4; ++i) {
		scores_.emplace_back(std::make_unique<NumberPlate>());
		scores_.back()->Initialize(textureManager, 6, false);
	}

	scores_[0]->SetPosition({ 5.0f, 2.63f, 20.0f });
	scores_[1]->SetPosition({ 5.0f, 1.2f, 20.0f });
	scores_[2]->SetPosition({ 5.0f, -0.48f, 20.0f });
	scores_[3]->SetPosition({ 5.0f, -2.72f, 20.0f });
}

GoalEvent::~GoalEvent() {
}

void GoalEvent::SetClear(bool isClear, float time) {
	if (!isClear_ && isClear) {
		clearTime_ = int(time);
	}
	isClear_ = isClear;
}

std::unique_ptr<BaseScene> GoalEvent::Update(float deltatime) {
	
	if (isClear_) {
		
		if (!preIsClear_) {
			Initialize();
			preIsClear_ = true;
		}

		ClearUpdate(deltatime);

	}
	return nullptr; // 継続中の場合はnullptrを返す
}

void GoalEvent::Initialize() {
	camera_->SetOffset({ 4.16f, 0.0f, -10.0f });
	player_->SetIsClear();

	// グリッド遷移の初期設定
	postEffect_->data_.gridTransition.progress = 0.0f;
	postEffect_->data_.gridTransition.gridSize = 16.0f;
	postEffect_->data_.gridTransition.fadeColor = 0.0f;  // 黒にフェード
	postEffect_->data_.gridTransition.pattern = 1.0f;    // ランダムパターン

	// 最初はエフェクトなし（クリア演出中）
	postEffect_->SetJobs(PostEffectJob::GridTransition);

	data_.resize(4);
}

void GoalEvent::ClearUpdate(float deltatime) {
	clearTimer_ += deltatime;

	finalScore_ = 10000 + (Coin::GetScore() * 1000) + (clearTime_ * -150);

	scores_[0]->Update(10000);
	scores_[1]->Update(Coin::GetScore() * 1000);
	scores_[2]->Update(int(clearTime_) * -150);
	scores_[3]->Update(finalScore_);

	if (keys_[Key::Action]) {
		clearTimer_ = 100.0f;
	}

	if (clearTimer_ >= clearWaitTime_ && count < 4) {

		if (count == 3 && clearTimer_ <= 1.5f) {
			goto fin;
		}

		clearTimer_ = 0;

		isActive_[count] = true;

		count++;
	}

fin:

	if (count >= 4 && keys_[Key::Action]) {
		postEffect = true;
	}

	if (postEffect) {
		postEffect_->data_.gridTransition.progress += deltatime;
	}

	if (postEffect_->data_.gridTransition.progress >= 1.0f) {
		postEffect_->data_.gridTransition.progress = 1.0f;
		changeScene_ = true;
	}

}

void GoalEvent::Draw(Render* render) {
	if (!isClear_) return;
	
	render->Draw(plate_.get());

	for (int i = 0; i < 4; ++i) {
		if (isActive_[i]) {
			scores_[i]->Draw(render);
		}
	}
}
