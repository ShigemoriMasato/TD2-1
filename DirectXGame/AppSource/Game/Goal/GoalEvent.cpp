#include "GoalEvent.h"
#include <Game/Scene/GameScene.h>

GoalEvent::GoalEvent(CameraManager* camera, Player* player, PostEffectResource* posteffect, TextureManager* textureManager) {
	camera_ = camera;
	player_ = player;
	postEffect_ = posteffect;

	plate_ = std::make_unique<DrawResource>();
	plate_->Initialize(ShapeType::Plane);
	plate_->camera_ = camera_->GetCamera();
	plate_->SetTextureHandle(textureManager->LoadTexture("Assets/Texture/number/ScorePlate.png"));

	for (int i = 0; i < 4; ++i) {
		if (i == 2) {
			scores_.emplace_back(std::make_unique<NumberPlate>());
			scores_.back()->Initialize(textureManager, 4, true);
			continue;
		}

		scores_.emplace_back(std::make_unique<NumberPlate>());
		scores_.back()->Initialize(textureManager, 6, false);
	}
}

GoalEvent::~GoalEvent() {
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
	camera_->SetOffset({ 0.0f, 0.0f, -10.0f });
	player_->SetIsClear();

	// グリッド遷移の初期設定
	postEffect_->data_.gridTransition.progress = 0.0f;
	postEffect_->data_.gridTransition.gridSize = 16.0f;
	postEffect_->data_.gridTransition.fadeColor = 0.0f;  // 黒にフェード
	postEffect_->data_.gridTransition.pattern = 1.0f;    // ランダムパターン

	// 最初はエフェクトなし（クリア演出中）
	postEffect_->SetJobs(PostEffectJob::None);
}

void GoalEvent::ClearUpdate(float deltatime) {
	clearTimer_ += deltatime;

	scores_[0]->Update(10000);
	scores_[1]->Update(0);
	scores_[2]->Update(int(clearTimer_));
	scores_[3]->Update(finalScore_);

	if(postEffect_->data_.gridTransition.progress >= 1.0f){
		changeScene_ = true;
	}
}

void GoalEvent::Draw(Render* render) {
	if (!isClear_) return;
	
	render->Draw(plate_.get());

	for (int i = 0; i < 4; ++i) {
		scores_[i]->Draw(render);
	}
}
