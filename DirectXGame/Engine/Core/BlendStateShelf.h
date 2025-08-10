#pragma once
#include <cstdint>
#include <d3d12.h>

enum class BlendStateID : uint8_t {
	Alpha,

	Count
};

class BlendStateShelf {
public:

	BlendStateShelf();
	~BlendStateShelf();

	D3D12_BLEND_DESC GetBlendState(BlendStateID id) const;

private:

	

};
