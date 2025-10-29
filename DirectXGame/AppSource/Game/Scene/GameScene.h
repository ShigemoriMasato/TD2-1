#pragma once
#include "../BaseObject.h"

#include <Game/Camera/CameraManager.h>
#include <Scene/Data/BaseScene.h>
#include <unordered_map>
#include "../Enemy/EnemyManager.h"
#include "../Player/Wire.h"
#include "../Physics/PhysicsEngine.h"
#include "../LevelLoader.h"
#include "../Tile/TileMap.h"
#include "../Goal/GoalTape.h"
#include "../Goal/GoalEvent.h"
#include "../Player/TargetScope.h"
#include "../BackGround/BackGround.h"
#include "../Emitter/DeathParticle.h"
#include "../Emitter/DeathPoint.h"
#include <Render/Resource/TimerResource.h>
#include <Game/FPS/TimeSlower.h>

class Player;
class GameScene : public BaseScene
{
public:

	void Initialize() override;
	void Initialize(std::string levelName);
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private://管理用メンバ変数
	//シーン内全てのオブジェクトリスト
	std::vector<std::unique_ptr<BaseObject>> objects_;
	std::unique_ptr<CameraManager> camera_ = nullptr;
	std::unique_ptr<Camera> uiCamera_ = nullptr;
	std::unordered_map<Key, bool> keys_{};

	PhysicsEngine physicsEngine_;
	LevelLoader levelLoader_;
	std::unique_ptr<TileMap> tileMap_ = nullptr;
	bool isExit = false;

private://パラメータ変数

	std::unique_ptr<TimeSlower> timeSlower_ = nullptr;

private://特定なオブジェクト
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
	Player* player_ = nullptr;
	std::unique_ptr<Wire> wire_ = nullptr;
	std::unique_ptr<GoalTape> goalTape_ = nullptr;
	std::unique_ptr<TargetScope> targetScope_ = nullptr;
	std::unique_ptr<BackGround> backGround_ = nullptr;
	std::unique_ptr<TimerResource> timer_ = nullptr;

private:
	float time_ = 0.0f;

private://Emitter
	std::unique_ptr<DeathParticle> deathParticle_ = nullptr;
	std::unique_ptr<DeathPoint> deathPoint_ = nullptr;

private://イベント
	std::unique_ptr<GoalEvent> goalEvent_ = nullptr;

private://postEffect
	std::unique_ptr<PostEffectResource> postEffect_ = nullptr;
	bool isFadingIn_ = false;
	float fadeTimer_ = 0.0f;
	float fadeDuration_ = 1.0f;
	float goalX_ = 20.0f;		//どこからがゴールか
	
	// スローモーションエフェクトのパラメータ（強度を上げる）
	float slowMotionChromaticAberration_ = 0.8f;   // 色収差強度（0.3→0.8）
	float slowMotionVignetteStrength_ = 0.85f;     // ビネット強度（0.6→0.85）
	float slowMotionSaturationMin_ = 0.3f;         // 最小彩度（0.5→0.3でより強い効果）

private://使える関数

	void CheckAllCollision();
	void CheckPlayerWireField();
	
	// ポストエフェクト関連の関数
	void UpdateFadeIn(float deltaTime);
	void UpdateSlowMotionEffect();
};

