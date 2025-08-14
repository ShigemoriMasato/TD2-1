#pragma once
#include <vector>
#include <cstdint>
#include <d3d12.h>

enum class InputLayoutID : uint8_t {
	Default = 0,

	Count
};

class InputLayoutShelf {
public:

	InputLayoutShelf();
	~InputLayoutShelf();

	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc(InputLayoutID id) const;

private:

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	std::vector<D3D12_INPUT_LAYOUT_DESC> inputLayouts_;

};

