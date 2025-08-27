#pragma once
#include "Object/Object.h"

class TitleLogo : public Object {
public:

	TitleLogo(Camera* camera, int handle);
	~TitleLogo();

	void Update();

private:

	float rotFreq_ = 0.02f;
	float scaFreq_ = 0.03f;

	float rotMax_ = 0.1f;
	float scaMaxX_ = 0.1f;
	float scaMaxY_ = 0.08f;

	int frame_ = 0;

};
