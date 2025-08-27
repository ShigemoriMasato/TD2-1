#pragma once
#include "Object/Object.h"
#include "Scene/Common/CommonData.h"
#include "Tools/Binary/BinaryManager.h"
#include <array>

class Number : public Object {
public:

	Number(Camera* camera);
	~Number() override = default;

	void Update();

	void SetPosition(Vector3 pos);
	static void SetRatio(float ratio) { ratio_ = ratio; };
	static void SetHandles(std::array<int, 10> handles) { handles_ = handles; };
	void SetNumer(int num) { handle_ = handles_[num]; };

private:

	static std::array<int, 10> handles_;
	static float ratio_;
	static Vector2 size_;

};

class RankBan {
public:

	RankBan(CommonData* commonData);
	~RankBan() = default;

	void Initialize();
	void Update();
	void Draw();

private:

	std::shared_ptr<Camera> camera_;
	std::unique_ptr<BinaryManager> manager_;
	std::vector<Vector3> positions_;

	std::list<double> times_;

	float ratio_;
	std::array<std::array<std::unique_ptr<Number>, 6>, 9> numbers_;
};

