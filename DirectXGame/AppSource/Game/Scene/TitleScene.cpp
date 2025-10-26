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

	// UI用カメラの初期化（Orthographic投影）
	// 画面中央が原点(0,0)になる座標系
	{
		uiCamera_ = std::make_unique<Camera>();
		OrthographicDesc desc{};
		desc.left = -640.0f;      // 左端 -640
		desc.top = 360.0f;        // 上端 +360
		desc.right = 640.0f;      // 右端 +640
		desc.bottom = -360.0f;    // 下端 -360
		desc.nearClip = 0.0f;
		desc.farClip = 1000.0f;
		uiCamera_->SetProjectionMatrix(desc);
		
		// カメラのトランスフォームを設定（UI用なので位置は原点）
		Transform cameraTransform{};
		cameraTransform.position = { 0.0f, 0.0f, 0.0f };
		cameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
		cameraTransform.scale = { 1.0f, 1.0f, 1.0f };
		uiCamera_->SetTransform(cameraTransform);
		uiCamera_->MakeMatrix();
	}

	// テクスチャの読み込み
	{
		titleTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/TitleScene/title.png");
		spaceStartTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/TitleScene/spaceStart.png");
	}

	// タイトルロゴの初期化
	{
		titleLogo_ = std::make_unique<DrawResource>();
		titleLogo_->Initialize(ShapeType::Plane);
		titleLogo_->SetTextureHandle(titleTextureHandle_);
		titleLogo_->camera_ = uiCamera_.get();
		
		// 画面中央上部に配置（原点は画面中央）
		titleLogo_->position_ = { 0.0f, 160.0f, 0.0f };  // Y+で上方向
		titleLogo_->scale_ = { 760.0f, 270.0f, 1.0f };
		titleLogo_->rotate_ = { 0.0f, 0.0f, 0.0f };
		titleLogo_->color_ = 0xffffffff;
	}

	// Space Startテキストの初期化
	{
		spaceStart_ = std::make_unique<DrawResource>();
		spaceStart_->Initialize(ShapeType::Plane);
		spaceStart_->SetTextureHandle(spaceStartTextureHandle_);
		spaceStart_->camera_ = uiCamera_.get();
		
		// 画面中央下部に配置（原点は画面中央）
		spaceStart_->position_ = { 0.0f, -160.0f, 0.0f };  // Y-で下方向
		spaceStart_->scale_ = { 633.0f, 66.0f, 1.0f };
		spaceStart_->rotate_ = { 0.0f, 0.0f, 0.0f };
		spaceStart_->color_ = 0xffffffff;
	}
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

	// UI描画
	render_->Draw(titleLogo_.get());
	render_->Draw(spaceStart_.get());

	render_->Draw(postEffect_.get());

}
