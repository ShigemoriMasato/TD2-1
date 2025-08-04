#pragma once
#include <string>
#include <vector>

class ShaderShelf {
public:

	ShaderShelf() = default;
	~ShaderShelf() = default;

	//シェーダーの登録
	bool RegisterShader(const std::string& name);

private:

	std::vector<std::string> shaderNames_;	//登録されたシェーダーの名前を保持する

};

