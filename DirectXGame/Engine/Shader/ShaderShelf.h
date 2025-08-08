#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include <vector>
#include <array>
#include <wrl.h>
#include <Logger/Logger.h>
#include <unordered_map>

enum class ShaderType {
	VERTEX_SHADER,
	PIXEL_SHADER,

	Count
};;

class ShaderShelf {
public:

	ShaderShelf();
	~ShaderShelf();

	//シェーダーの登録
	void RegisterShader(const std::string& name, ShaderType shaderType);

	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() const;
	IDxcBlob* GetShaderBlob(const std::string& name, ShaderType shaderType) const;

private:

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;

	std::array<std::unordered_map<std::string, IDxcBlob*>, size_t(ShaderType::Count)> shaderNames_;
	std::array<std::wstring, size_t(ShaderType::Count)> shaderVersions_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	inline static const std::string basePath_ = "Data/Shader/";

	std::unique_ptr<Logger> logger_ = nullptr;
};

