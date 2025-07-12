#pragma once
#include "Object/Object.h"

/// <summary>
/// Human Power BPM Measure
/// </summary>
namespace HPBM {

	class Effect : public Object {
	public:

		Effect(Camera* camera, Vector3 center);
		~Effect() = default;

		void Initialize() override {}
		void Update() override;

	private:
		static const int kMaxLifeTime = 30;
		int lifeTime_ = 0;
	};

	class Button : public Object {
	public:
		Button(Camera* camera);
		~Button() = default;

		void Initialize() override;
		void Update() override;
		void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

	private:

		std::vector<std::shared_ptr<Effect>> effects_;

	};



	class Measure {
	public:

		Measure();
		~Measure();

		void Update();
		void Draw(Camera* camera) const;

		void MeasureStart(int index);

		float GetBPM() const;

	private:

		std::unique_ptr<Button> button_ = nullptr;

		std::vector<float> gaps_;
		float bpm_ = -1.0f;
		float gap_ = 0.0f;
		float frame_ = 0.0f;
		const float deltatime = 1.0f / 60.0f;

		bool isMeasuring_ = false;
		bool isActiveSpace_ = false;

	};

}