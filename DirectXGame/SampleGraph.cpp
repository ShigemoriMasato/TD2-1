#include "SampleGraph.h"
#include "MoveUpLine.h"

SampleGraph::SampleGraph(Camera* camera, int* beat) : Actor(camera, ShapeType::Count) {
	beat_ = beat;
	Initialize();
}

void SampleGraph::Initialize() {
	ClearQueue();
	std::shared_ptr<MoveUpLine> line = std::make_shared<MoveUpLine>(
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1.0f, 1.0f, 1.0f),
		60,
		camera_
	);
	EnqueueAction(line);

	*beat_ = 0;
}

void SampleGraph::Update() {

	//Queueの追加とかをもろもろする
	if (*beat_ != preBeat_) {
		std::shared_ptr<MoveUpLine> line = std::make_shared<MoveUpLine>(
			Vector3(0.0f, 0.0f, 0.0f),
			Vector3(1.0f, 1.0f, 1.0f),
			60,
			camera_
		);
		EnqueueAction(line);
	}

	//最後の実行
	ExecuteQueue();

	preBeat_ = *beat_;
}

void SampleGraph::Draw(const Matrix4x4* worldMatrix) const {
	ExecuteDrawQueue();
}
