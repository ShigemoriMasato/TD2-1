#pragma once
#include <Core/DXDevice.h>
#include "Data/DXResource.h"
#include <Core/DXCommonFunction.h>

#include <vector>

/// <summary>
/// どんな種類のリソースが何個必要かをすべて自分で決める。ラップして使うべし
/// しばらくあきらめる!!!!!!たのしくない！！！！！！
/// </summary>
class CustomResource {
public:
	CustomResource() = default;
	~CustomResource() = default;

	static void SetDevice(DXDevice* device) { dxDevice_ = device; }

	template<typename T>
	void CreateResource(ViewType type, uint32_t num, T* data) {
		CreateBufferResource(dxDevice_->GetDevice(), sizeof(T) * num);
	};

private:

	static DXDevice* dxDevice_;

	std::vector<DXResource> resources_;

};
