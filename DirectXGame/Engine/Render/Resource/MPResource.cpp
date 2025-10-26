#include "MPResource.h"

MPResource::MPResource()
{

}

MPResource::~MPResource()
{
}

void MPResource::Initialize(ModelData* modelData, int instanceNum)
{
	psoConfig_ = PSOConfig{};

	logger_ = std::make_unique<Logger>();
	logger_->RegistLogFile("MPResource");

	InitComputPSO();

	int vertexNum = modelData->GetVertexResource().begin()->second.vertexNum;
	int indexNum = modelData->GetIndexResource().begin()->second.indexNum;

	auto device = dxDevice_->GetDevice();

	vertexBufferView = *modelData->GetVertexResource().begin()->second.bufferView;
	indexBufferView = *modelData->GetIndexResource().begin()->second.bufferView;

	//particleData
	particleDataResource.Attach(CreateBufferResource(device, sizeof(ModelParticleData) * instanceNum));
	particleDataResource->Map(0, nullptr, reinterpret_cast<void**>(&particleData_));

	waveBuffer_.Attach(CreateBufferResource(device, sizeof(WaveParams)));
	waveBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&waveData_));

	for (int i = 0; i < instanceNum; ++i)
	{
		particleData_[i].world = MakeIdentity4x4();
		particleData_[i].wvp = MakeIdentity4x4();
		particleData_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		particleData_[i].textureIndex = 0;
		particleData_[i].worldPos = { 0.0f,0.0f,0.0f };
	}

	color_.resize(instanceNum, 0xffffffff);
	textureIndex_.resize(instanceNum, 0);

	//ParticleDataのSRV作成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = instanceNum;
		srvDesc.Buffer.StructureByteStride = sizeof(ModelParticleData);

		D3D12_CPU_DESCRIPTOR_HANDLE matrixCPUHandle = srvManager_->GetCPUHandle();
		particleDataGPUHandle_ = srvManager_->GetGPUHandle();

		device->CreateShaderResourceView(particleDataResource.Get(), &srvDesc, matrixCPUHandle);
	}

	position_.resize(instanceNum);
	rotate_.resize(instanceNum);
	scale_.resize(instanceNum, { 1.0f, 1.0f, 1.0f });

	vertexNum_ = vertexNum;
	indexNum_ = indexNum;
	instanceNum_ = instanceNum;
}

void MPResource::Initialize(ModelManager* manager, int modelHandle, int instanceNum)
{
	auto modelData = manager->GetModelData(modelHandle);
	if (!modelData)
	{
		return;
	}
	Initialize(modelData, instanceNum);
}

void MPResource::DrawReady()
{
	//particleData
	Matrix4x4 bill = MakeIdentity4x4();
	if (billboard_)
	{
		// ビルボード
		bill = Inverse(camera_->GetTranformMatrix());
		for (int i = 0; i < 3; ++i)
		{
			bill.m[3][i] = 0.0f;
		}
	}

	//各行列の作成
	for (uint32_t i = 0; i < instanceNum_; ++i)
	{
		particleData_[i].world = MakeScaleMatrix(scale_[i]) * MakeRotationMatrix(rotate_[i]) * bill * MakeTranslationMatrix(position_[i]);

		if (camera_)
		{
			particleData_[i].wvp = particleData_[i].world * camera_->GetVPMatrix();
		}

		particleData_[i].color = {
			((color_[i] >> 24) & 0xff) / 255.0f,
			((color_[i] >> 16) & 0xff) / 255.0f,
			((color_[i] >> 8) & 0xff) / 255.0f,
			1.0f
		};

		particleData_[i].textureIndex = textureIndex_[i];
	}



	psoConfig_.ps = "MP.PS.hlsl";
	psoConfig_.vs = "MP.VS.hlsl";
	psoConfig_.rootID = RootSignatureID::MP;
	psoConfig_.inputLayoutID = InputLayoutID::Default;
}

D3D12_GPU_DESCRIPTOR_HANDLE MPResource::GetCurrentTileSRV() const
{
	return tilePing_ ? texA.srvGpuHandle : texB.srvGpuHandle;
}

void MPResource::InitComputPSO()
{
	auto device = dxDevice_->GetDevice();

	CD3DX12_DESCRIPTOR_RANGE1 srvRange;
	srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

	CD3DX12_DESCRIPTOR_RANGE1 uavRange;
	uavRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0

	CD3DX12_ROOT_PARAMETER1 rootParams[3]{};
	rootParams[0].InitAsDescriptorTable(1, &srvRange);
	rootParams[1].InitAsDescriptorTable(1, &uavRange);
	rootParams[2].InitAsConstants(10, 0);
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init_1_1(
		_countof(rootParams),
		rootParams,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> sigBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DX12SerializeVersionedRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_1,
		&sigBlob,
		&errorBlob);

	device->CreateRootSignature(
		0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));


	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	//dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	//includeに対応するための設定を行っておく
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	auto tileCS = CompileShader(L"Assets/Shader/TileEffectCS.hlsl", L"cs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get(), logger_.get());

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.CS = { tileCS->GetBufferPointer(), tileCS->GetBufferSize() };
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&PSO_));
}

void MPResource::InitializeTileTexture(int mapW, int mapH)
{
	mapWidth_ = mapW;
	mapHeight_ = mapH;

	auto device = dxDevice_->GetDevice();

	texA = CreateTexture2DWithUAVSRV(
		device,
		srvManager_,
		mapW,
		mapH,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	texA.state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	texB = CreateTexture2DWithUAVSRV(
		device,
		srvManager_,
		mapW,
		mapH,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	texB.state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
}

void MPResource::DispatchTileDiffusion(ID3D12GraphicsCommandList* cmdList, float deltaTime)
{
	ID3D12Resource* prev = tilePing_ ? texA.texture.Get() : texB.texture.Get(); // read as SRV
	ID3D12Resource* next = tilePing_ ? texB.texture.Get() : texA.texture.Get(); // write as UAV

	D3D12_GPU_DESCRIPTOR_HANDLE prevSrv = tilePing_ ? texA.srvGpuHandle : texB.srvGpuHandle; // t0
	D3D12_GPU_DESCRIPTOR_HANDLE nextUav = tilePing_ ? texB.uavGpuHandle : texA.uavGpuHandle; // u0

	ID3D12DescriptorHeap* heaps[] = { srvManager_->GetHeap() };
	cmdList->SetDescriptorHeaps(1, heaps);

	cmdList->SetPipelineState(PSO_.Get());
	cmdList->SetComputeRootSignature(rootSignature_.Get());

	auto& prevState = tilePing_ ? texA.state : texB.state;
	auto& nextState = tilePing_ ? texB.state : texA.state;

	auto transition = [&](ID3D12Resource* res, D3D12_RESOURCE_STATES& state, D3D12_RESOURCE_STATES target)
		{
			if (state != target)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(res, state, target);
				cmdList->ResourceBarrier(1, &barrier);
				state = target;
			}
		};

	transition(prev, prevState, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	transition(next, nextState, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->SetComputeRootDescriptorTable(0, prevSrv);
	cmdList->SetComputeRootDescriptorTable(1, nextUav);

	waveSourceConsts_.time += deltaTime;
	int32_t consts32[10];
	consts32[0] = waveSourceConsts_.waveX;
	consts32[1] = waveSourceConsts_.waveY;
	union { float f; int i; } u;
	u.f = waveSourceConsts_.radius;   consts32[2] = u.i;
	u.f = waveSourceConsts_.intensity; consts32[3] = u.i;
	u.f = waveSourceConsts_.falloff;  consts32[4] = u.i;
	consts32[5] = waveSourceConsts_.mapW;
	consts32[6] = waveSourceConsts_.mapH;
	u.f = waveSourceConsts_.time;     consts32[7] = u.i;
	u.f = waveSourceConsts_.waveStartTime; consts32[8] = u.i;
	u.f = waveSourceConsts_.waveActiveDuration; consts32[9] = u.i;

	cmdList->SetComputeRoot32BitConstants(2, 10, consts32, 0);

	UINT gx = (mapWidth_ + 7) / 8;
	UINT gy = (mapHeight_ + 7) / 8;
	cmdList->Dispatch(gx, gy, 1);

	auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(next);
	cmdList->ResourceBarrier(1, &uavBarrier);

	tilePing_ = !tilePing_;

	auto& currentState = tilePing_ ? texA.state : texB.state;
	ID3D12Resource* currentTex = tilePing_ ? texA.texture.Get() : texB.texture.Get();

	if (currentState != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			currentTex,
			currentState,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		cmdList->ResourceBarrier(1, &barrier);
		currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	}
}

void MPResource::SetWaveSource(int x, int y, float radius, float intensity, float falloff, float duration)
{
	waveData_->color = { 1,0,1,1 };
	waveData_->gMapWorldSize = { (float)mapWidth_,(float)mapHeight_ };
	waveSourceConsts_.waveX = x;
	waveSourceConsts_.waveY = y;
	waveSourceConsts_.radius = radius;
	waveSourceConsts_.intensity = intensity;
	waveSourceConsts_.falloff = falloff;
	waveSourceConsts_.mapW = mapWidth_;
	waveSourceConsts_.mapH = mapHeight_;
	waveSourceConsts_.time = 0.0f;
	waveSourceConsts_.waveStartTime = 0.0f;
	waveSourceConsts_.waveActiveDuration = duration;

	hasActiveWave_ = true;
}

MPResource::TileTexture2DViews MPResource::CreateTexture2DWithUAVSRV(
	ID3D12Device* device,
	SRVManager* srvManager,
	UINT width, UINT height,
	DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags,
	D3D12_RESOURCE_STATES initialState)
{
	TileTexture2DViews result = {};

	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = flags;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	Microsoft::WRL::ComPtr<ID3D12Resource> texture;
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(&texture));

	result.texture = texture;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = srvManager->GetCPUHandle();
	result.srvGpuHandle = srvManager->GetGPUHandle();
	device->CreateShaderResourceView(texture.Get(), &srvDesc, srvCpuHandle);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE uavCpuHandle = srvManager->GetCPUHandle();
	result.uavGpuHandle = srvManager->GetGPUHandle();
	device->CreateUnorderedAccessView(texture.Get(), nullptr, &uavDesc, uavCpuHandle);

	return result;
}
