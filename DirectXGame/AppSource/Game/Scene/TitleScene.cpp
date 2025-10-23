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
		
		// タイトルシーンは最初から見えている状態で開始（alpha = 0.0）
		postEffect_->data_.fade.alpha = 0.0f;
		// 常にFadeジョブを設定（alpha値で制御）
		postEffect_->SetJobs(PostEffectJob::Fade);
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

	// スペースキー（Actionキー）でフェードアウト開始
	if (!isFading_ && keys[Key::Action]) {
		isFading_ = true;
		fadeTimer_ = 0.0f;
	}

	// フェードアウト処理（シーン遷移時）
	if (isFading_) {
		fadeTimer_ += deltaTime;
		postEffect_->data_.fade.alpha = fadeTimer_ / fadeDuration_;
		
		// フェード中は毎フレームFadeジョブを設定
		postEffect_->SetJobs(PostEffectJob::Fade);

		// フェードアウト完了でゲームシーンへ
		if (fadeTimer_ >= fadeDuration_) {
			return std::make_unique<GameScene>();
		}
	} else {
		// フェードしていない時もFadeジョブを設定（alpha=0.0で透明）
		postEffect_->SetJobs(PostEffectJob::Fade);
	}

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw()
{
	render_->PreDraw(OffScreenIndex::Title);

	// TODO: タイトル画面のUIやロゴなどの描画をここに追加

	render_->Draw(postEffect_.get());

}
