#pragma once
#include <Scene/Data/BaseScene.h>
#include <Tools/Binary/BinaryManager.h>
#include <Core/PSO/Shelf/ShaderShelf.h>

struct ShaderDataIndex {
	int psIndex = 0;
	int vsIndex = 0;
	int inputLayoutIndex = 0;
	int rootSignatureIndex = 0;
};

class ShaderEditScene : public BaseScene {
public:

	ShaderEditScene();
	~ShaderEditScene() override;

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	void Save();

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	std::vector<ShaderDataIndex> shaderDataIndex_;

	std::vector<const char*> pss_;
	std::vector<const char*> vss_;
	std::vector<const char*> inputLayouts_;
	std::vector<const char*> rootSignatures_;

	const std::string saveFileName = "ShaderEditData.bin";

	//const char*の寿命保障用
	std::vector<std::vector<std::string>> buffer_;
};
