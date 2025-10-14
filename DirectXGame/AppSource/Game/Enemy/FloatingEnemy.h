#pragma once
#include "BaseEnemy.h"

class FloatingEnemy : public BaseEnemy
{
public:
    FloatingEnemy();
    ~FloatingEnemy() override = default;

    void Initialize(ModelData* modelData, Camera* camera) override;
    void Update(float deltaTime) override;
    void Draw(Render* render) override;

private:
    // 浮遊効果の設定
    float baseHeight_ = 0.0f;           // 基準の高さ
    float currentHeight_ = 0.0f;        // 現在の高さ
    float targetHeight_ = 0.0f;         // 目標の高さ
    float floatHeight_ = 2.0f;          // 浮く高さ
    float effectRange_ = 3.0f;          // 効果範囲（プレイヤーがこの距離内にいると浮く）
    float floatSpeed_ = 2.0f;           // 浮遊速度
    
    // プレイヤーが範囲内にいるかチェック
    bool IsPlayerInRange() const;
    
    // プレイヤーまでの距離を計算（Y軸は除外）
    float GetDistanceToPlayer() const;
    
    // 浮遊効果の更新
    void UpdateFloatingEffect(float deltaTime);
};