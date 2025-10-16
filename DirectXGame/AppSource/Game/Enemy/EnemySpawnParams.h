#pragma once
#include <Math/MyMath.h>
#include <string>
#include <variant>
#include <unordered_map>

/// @brief 敵のスポーンパラメータ
struct EnemySpawnParams {
	// 基本的な空間配置パラメータ
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	
	// モデル設定
	std::string modelName = "testEnemy";
	
	// ゲーム設定
	std::string teamTag = "enemy";  // チーム識別子
	int priority = 0;               // 優先度（描画順等）
	
	// 敵固有のパラメータ（柔軟に拡張可能）
	std::unordered_map<std::string, std::variant<float, int, bool, std::string>> customParams;
	
	// よく使われるパラメータのヘルパー関数
	float GetFloat(const std::string& key, float defaultValue = 0.0f) const {
		auto it = customParams.find(key);
		if (it != customParams.end() && std::holds_alternative<float>(it->second)) {
			return std::get<float>(it->second);
		}
		return defaultValue;
	}
	
	int GetInt(const std::string& key, int defaultValue = 0) const {
		auto it = customParams.find(key);
		if (it != customParams.end() && std::holds_alternative<int>(it->second)) {
			return std::get<int>(it->second);
		}
		return defaultValue;
	}
	
	bool GetBool(const std::string& key, bool defaultValue = false) const {
		auto it = customParams.find(key);
		if (it != customParams.end() && std::holds_alternative<bool>(it->second)) {
			return std::get<bool>(it->second);
		}
		return defaultValue;
	}
	
	std::string GetString(const std::string& key, const std::string& defaultValue = "") const {
		auto it = customParams.find(key);
		if (it != customParams.end() && std::holds_alternative<std::string>(it->second)) {
			return std::get<std::string>(it->second);
		}
		return defaultValue;
	}
};