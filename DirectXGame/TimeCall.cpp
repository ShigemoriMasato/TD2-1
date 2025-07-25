#include "TimeCall.h"

TimeCall::TimeCall() {
	timeCallData_.clear();
	frame_ = 0;
}

void TimeCall::Initialize() {
}

void TimeCall::Update() {
	++frame_;

	for (int i = 0; i < timeCallData_.size(); ++i) {
		if (timeCallData_[i].executeFrame <= frame_) {
			
			timeCallData_[i].func();

			//繰り返し実行するか
			if (timeCallData_[i].repeat) {
				// 次の実行フレームを設定
				timeCallData_[i].executeFrame = frame_ + (timeCallData_[i].executeFrame - timeCallData_[i].registerFrame);
				timeCallData_[i].registerFrame = frame_;
			} else {
				// 削除
				timeCallData_.erase(timeCallData_.begin() + i--);
			}
		}
	}
}

int TimeCall::Register(std::function<void()> func, int afterFrame, bool repete) {
	TimeCallData data;
	data.executeFrame = frame_ + afterFrame;
	data.registerFrame = frame_;
	data.func = func;
	data.repeat = repete;
	data.id = ++id;
	timeCallData_.push_back(data);

	return data.id;
}

void TimeCall::Clear() {
	timeCallData_.clear();
}

void TimeCall::Delete(int id) {
	for(int i = 0; i < timeCallData_.size(); ++i) {
		if (timeCallData_[i].id == id) {
			timeCallData_.erase(timeCallData_.begin() + i);
			return;
		}
	}
}
