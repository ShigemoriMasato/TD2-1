#include "TitleScene.h"
#include "GameScene.h"

void TitleScene::Initialize()
{

	{
		//PostEffect初期化
		postEffect_ = std::make_unique<PostEffectResource>();
		postEffect_->Initialize();
		postEffect_->input_ = OffScreenIndex::Title;
		postEffect_->output_ = OffScreenIndex::SwapChain;
		
		// グリッドトランジション初期化
		postEffect_->data_.gridTransition.progress = 0.0f;
		postEffect_->data_.gridTransition.gridSize = 16.0f;  // 16x16グリッド
		postEffect_->data_.gridTransition.fadeColor = 0.0f;  // 黒にフェード
		postEffect_->data_.gridTransition.pattern = 0.0f;    // 波紋状パターン（外→内）
	}

	// フェード状態の初期化
	isFading_ = false;
	fadeTimer_ = 0.0f;
}

std::unique_ptr<BaseScene> TitleScene::Update()
{
	float deltaTime = fpsObserver_->GetDeltatime();

	// キー状態を取得
	auto keys = commonData->keyManager_->GetKeyStates();

	// スペースキー（Actionキー）でグリッドトランジション開始
	if (!isFading_ && keys[Key::Action]) {
		isFading_ = true;
		fadeTimer_ = 0.0f;
	}

	// グリッドトランジション処理（シーン遷移時）
	if (isFading_) {
		fadeTimer_ += deltaTime;
		postEffect_->data_.gridTransition.progress = fadeTimer_ / fadeDuration_;
		
		// グリッドトランジションジョブを設定
		postEffect_->SetJobs(PostEffectJob::GridTransition);

		// トランジション完了でゲームシーンへ
		if (fadeTimer_ >= fadeDuration_) {
			return std::make_unique<GameScene>();
		}
	} else {
		// トランジションしていない時は通常描画
		postEffect_->data_.gridTransition.progress = 0.0f;
		postEffect_->SetJobs(PostEffectJob::None);
	}

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw()
{
	render_->PreDraw(OffScreenIndex::Title);

	// TODO: タイトル画面のUIやロゴなどの描画をここに追加

	render_->Draw(postEffect_.get());

}
