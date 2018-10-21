

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include <array>
#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>



#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/WICTextureLoader.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include <DirectXTK12/DirectXHelpers.h>



#include <Hawk/Components/Camera.hpp>
#include <Hawk/Components/Transform.hpp>
#include <Hawk/Common/NonCopyable.hpp>
#include <Hawk/Common/Singleton.hpp>



using namespace Hawk;



namespace DX {

	struct ComException : public std::exception {
	public:
		ComException(HRESULT hr) : result(hr) {}
		virtual auto what() const -> const char* override {
			static char s_str[64] = {};

			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<uint32_t>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	auto ThrowIfFailed(HRESULT hr) -> void {
		if (FAILED(hr)) throw ComException(hr);
	}

}


struct DescriptorHandle {
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPU;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPU;
};

class DescriptorHeap {
public:
	DescriptorHeap(ID3D12Device* device, U32 size, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	auto GetCapacity() const->U32;
	auto GetSize()     const->U32;
	auto GetHeap()     const->ID3D12DescriptorHeap*;
	auto GenerateHandle()->DescriptorHandle;
private:
	U32 m_Capacity;
	U32 m_Size;
	U32 m_Offset;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
};

class Shader{
public:
	Shader(std::string const& fileName, std::string const& entryPoint, std::string const& target, std::initializer_list<D3D_SHADER_MACRO> defines = {});
	auto GetShaderBytecode() const->D3D12_SHADER_BYTECODE;
	auto GetInputLayout()    const->D3D12_INPUT_LAYOUT_DESC;
private:
	Microsoft::WRL::ComPtr<ID3DBlob>               m_Code;
	Microsoft::WRL::ComPtr<ID3D12ShaderReflection> m_Reflection;
	std::vector<D3D12_INPUT_ELEMENT_DESC>          m_InputElemetDescArray;
};

class CommandContext {
public:
	CommandContext(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, D3D12_FENCE_FLAGS fenceFlags);
	auto WaitForGPU()     -> void;

	auto ExecuteCmdList() -> void;
	auto CloseCmdList()   -> void;
	auto ResetCmdList(ID3D12PipelineState* initPSO = nullptr) -> void;

	auto GetCmdList()          const->ID3D12GraphicsCommandList*;
	auto GetCmdQueue()         const->ID3D12CommandQueue*;
	auto GetCommandAllocator() const->ID3D12CommandAllocator*;
	auto GetFence()            const->ID3D12Fence*;
	auto GetFenceValue()       const->U64;
	auto GetFenceEvent()       const->HANDLE;
private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>        m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    m_CommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	Microsoft::WRL::ComPtr<ID3D12Fence>               m_Fence;

	U64    m_FenceValue;
	HANDLE m_FenceEvent;

};

Shader::Shader(std::string const & fileName, std::string const & entryPoint, std::string const& target, std::initializer_list<D3D_SHADER_MACRO> defines) {

	U32 compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
	compileFlags = D3DCOMPILE_DEBUG;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> error = nullptr;
	DX::ThrowIfFailed(::D3DCompileFromFile(std::wstring(fileName.begin(), fileName.end()).c_str(), std::data(defines), D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), compileFlags, 0, &m_Code, &error));

	::D3DReflect(m_Code->GetBufferPointer(), m_Code->GetBufferSize(), IID_PPV_ARGS(m_Reflection.GetAddressOf()));

	D3D12_SHADER_DESC shaderDesc;
	m_Reflection->GetDesc(&shaderDesc);

	m_InputElemetDescArray.resize(shaderDesc.InputParameters);
	auto index = 0;
	for (auto& e : m_InputElemetDescArray) {

		auto parameterDesc = D3D12_SIGNATURE_PARAMETER_DESC{};
		m_Reflection->GetInputParameterDesc(index, &parameterDesc);
		e.SemanticName = parameterDesc.SemanticName;
		e.SemanticIndex = parameterDesc.SemanticIndex;
		e.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		if (parameterDesc.Mask == 1) {
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				e.Format = DXGI_FORMAT_R32_UINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				e.Format = DXGI_FORMAT_R32_SINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				e.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (parameterDesc.Mask <= 3) {
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				e.Format = DXGI_FORMAT_R32G32_UINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				e.Format = DXGI_FORMAT_R32G32_SINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				e.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (parameterDesc.Mask <= 7) {
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				e.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				e.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				e.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (parameterDesc.Mask <= 15)
		{
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				e.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				e.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				e.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		index++;
	}

}

auto Shader::GetShaderBytecode() const -> D3D12_SHADER_BYTECODE {
	return { m_Code->GetBufferPointer(), static_cast<U32>(m_Code->GetBufferSize()) };
}

auto Shader::GetInputLayout() const -> D3D12_INPUT_LAYOUT_DESC {
	return { std::data(m_InputElemetDescArray), static_cast<U32>(std::size(m_InputElemetDescArray)) };
}



DescriptorHeap::DescriptorHeap(ID3D12Device* device, U32 size, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = size;
	heapDesc.Type = type;
	heapDesc.Flags = flags;
	DX::ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_Heap.GetAddressOf())));
	m_Capacity = size;
	m_Size = 0;
	m_Offset = device->GetDescriptorHandleIncrementSize(type);

}

auto DescriptorHeap::GetCapacity() const -> U32 {
	return m_Capacity;
}

auto DescriptorHeap::GetSize() const -> U32 {
	return m_Size;
}

auto DescriptorHeap::GetHeap() const -> ID3D12DescriptorHeap *
{
	return m_Heap.Get();
}

auto DescriptorHeap::GenerateHandle() -> DescriptorHandle {
	assert(m_Size < m_Capacity);
	DescriptorHandle handle;
	handle.CPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_Heap->GetCPUDescriptorHandleForHeapStart(), m_Size, m_Offset);
	handle.GPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_Heap->GetGPUDescriptorHandleForHeapStart(), m_Size, m_Offset);
	m_Size++;
	return handle;
}

CommandContext::CommandContext(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, D3D12_FENCE_FLAGS fenceFlags) {
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = type;

	DX::ThrowIfFailed(device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
	DX::ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	DX::ThrowIfFailed(device->CreateCommandList(0, type, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
	DX::ThrowIfFailed(device->CreateFence(0, fenceFlags, IID_PPV_ARGS(m_Fence.GetAddressOf())));
	m_FenceValue = 1;
	m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
}

auto CommandContext::WaitForGPU() -> void {

	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
	if (m_Fence->GetCompletedValue() < m_FenceValue) {
		m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
	m_FenceValue++;
}

auto CommandContext::CloseCmdList() -> void {
	DX::ThrowIfFailed(m_CommandList->Close());
}

auto CommandContext::ResetCmdList(ID3D12PipelineState* initPSO) -> void {
	DX::ThrowIfFailed(m_CommandAllocator->Reset());
	DX::ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), initPSO));
}

auto CommandContext::ExecuteCmdList() -> void {
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}

auto CommandContext::GetCmdList() const -> ID3D12GraphicsCommandList * {
	return m_CommandList.Get();
}

auto CommandContext::GetCmdQueue() const -> ID3D12CommandQueue * {
	return m_CommandQueue.Get();
}

auto CommandContext::GetCommandAllocator() const -> ID3D12CommandAllocator * {
	return m_CommandAllocator.Get();
}

auto CommandContext::GetFence() const -> ID3D12Fence * {
	return m_Fence.Get();
}

auto CommandContext::GetFenceValue() const -> U64 {
	return m_FenceValue;
}

auto CommandContext::GetFenceEvent() const -> HANDLE {
	return m_FenceEvent;
}







struct FrameConstantBuffer {
	Math::Mat4x4 View;
	Math::Mat4x4 Project;
};

struct ObjectConstantBuffer {
	Math::Mat4x4 WVP;
	Math::Mat4x4 World;
	Math::Mat4x4 Normal;
};



#pragma pack(push,1)
struct Vertex {
	Math::Vec3 Position;
	Math::Vec3 Normal;
	Math::Vec3 Tangent;
	Math::Vec2 Texcoord;
};
#pragma pack(pop)


struct Material {
	Microsoft::WRL::ComPtr<ID3D12Resource> TexDiffuse;
	Microsoft::WRL::ComPtr<ID3D12Resource> TexSpecular;
	Microsoft::WRL::ComPtr<ID3D12Resource> TexNormal;
	Math::Vec4 Ambient;
	Math::Vec4 Diffuse;
	Math::Vec4 Specular;
	Math::Vec4 Emission;
	Math::Vec4 Shininess;

};

struct Mesh {
	uint32_t IndexMaterial;
	uint32_t CountIndexes;
	uint32_t VertexBase;
	uint32_t Offset;
};


class Model
{
public:
	Model(Microsoft::WRL::ComPtr<ID3D12Device> device, CommandContext& context, DescriptorHeap& heap, std::string filename);
	auto Draw(CommandContext& context) const noexcept -> void;
	auto DrawDepth(CommandContext& context) const noexcept -> void;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource>  m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>  m_IndexBuffer;
	D3D12_VERTEX_BUFFER_VIEW                m_VBV;
	D3D12_INDEX_BUFFER_VIEW                 m_IBV;
	uint32_t                                m_CountIndexes;
	uint32_t                                m_CountVertices;
	std::vector<DescriptorHandle>           m_SRVs;
	std::vector<Material>                   m_Materials;
	std::vector<Mesh>                       m_Meshes;
	std::string                             m_Directory;

};






#undef main
int main(int arc, char* argv[]) {



	auto WINDOW_WIDTH = 1280;
	auto WINDOW_HEIGHT = 1000;

	SDL_Init(SDL_INIT_EVERYTHING);
	auto* window = SDL_CreateWindow("D3D12", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	SDL_SysWMinfo info{};
	SDL_GetWindowWMInfo(window, &info);

	U32 dxgiFactoryFlags = 0;

#if defined(_DEBUG)

	{
		Microsoft::WRL::ComPtr<ID3D12Debug> pDebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)))) {
			pDebugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}

#endif

	Microsoft::WRL::ComPtr<ID3D12Device> pDevice;
	DX::ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(pDevice.GetAddressOf())));
	CommandContext cmdGraphicsContext{ pDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_FENCE_FLAG_NONE };
	CommandContext cmdComputeContext{ pDevice.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_FENCE_FLAG_NONE };

#if defined(_DEBUG)

	{
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(pDevice.As(&pInfoQueue))) {
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);


			D3D12_MESSAGE_SEVERITY severities[] = {
				D3D12_MESSAGE_SEVERITY_INFO
			};

			D3D12_MESSAGE_ID denyIds[] = {
				D3D12_MESSAGE_ID_DEPTH_STENCIL_FORMAT_MISMATCH_PIPELINE_STATE,
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
				D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH,
				D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_DEPTHSTENCILVIEW_NOT_SET
				//D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				//D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
				
			};

			D3D12_INFO_QUEUE_FILTER newFilter = {};
			newFilter.DenyList.NumSeverities = _countof(severities);
			newFilter.DenyList.pSeverityList = severities;
			newFilter.DenyList.NumIDs = _countof(denyIds);
			newFilter.DenyList.pIDList = denyIds;

			DX::ThrowIfFailed(pInfoQueue->PushStorageFilter(&newFilter));
		}
	}


#endif


	Microsoft::WRL::ComPtr<IDXGISwapChain3> pSwapChain;
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainAs;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Width = WINDOW_WIDTH;
		swapChainDesc.Height = WINDOW_HEIGHT;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc = { 1, 0 };

		Microsoft::WRL::ComPtr<IDXGIFactory4> pFactory;
		DX::ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(pFactory.GetAddressOf())));


		{
			std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapterList;
			Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;
			U32 index = 0;
			while (pFactory->EnumAdapters1(index, pAdapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
				DXGI_ADAPTER_DESC1 descAdapter;
				pAdapter->GetDesc1(&descAdapter);
				std::wcout << "Description: " << descAdapter.Description << std::endl;
				std::wcout << "Device ID: " << descAdapter.DeviceId << std::endl;
				std::wcout << "Video memory: " << descAdapter.DedicatedVideoMemory / (2 << 20) << " Mb" << std::endl;
				std::wcout << "System memory: " << descAdapter.DedicatedSystemMemory / (2 << 20) << " Mb" << std::endl;
				std::wcout << "Shared sys memory: " << descAdapter.SharedSystemMemory / (2 << 20) << " Mb" << std::endl;
				std::wcout << "------------------------------------------------------" << std::endl;
				adapterList.push_back(pAdapter);
				index++;
			}
		}


		DX::ThrowIfFailed(pFactory->CreateSwapChainForHwnd(cmdGraphicsContext.GetCmdQueue(), info.info.win.window, &swapChainDesc, nullptr, nullptr, swapChainAs.GetAddressOf()));
		DX::ThrowIfFailed(pFactory->MakeWindowAssociation(info.info.win.window, DXGI_MWA_NO_ALT_ENTER));
		DX::ThrowIfFailed(swapChainAs.As(&pSwapChain));
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> pRenderTargets[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> pDepthTargets[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> pConstantBuffers[2];

	Microsoft::WRL::ComPtr<ID3D12Resource> pGBufferDiffuse;
	Microsoft::WRL::ComPtr<ID3D12Resource> pGBufferNormal;
	Microsoft::WRL::ComPtr<ID3D12Resource> pGBufferDepth;
	Microsoft::WRL::ComPtr<ID3D12Resource> pTextureAmbient;
	Microsoft::WRL::ComPtr<ID3D12Resource> pTextureReflection;

	
	DescriptorHandle RTVs[2];
	DescriptorHandle DSVs[2];
	DescriptorHandle CBVs[2];

	DescriptorHandle UAVAmbient;
	DescriptorHandle UAVReflection;
	DescriptorHandle RTVGBufferDiffuse;
	DescriptorHandle RTVGBufferNormal;
	DescriptorHandle DSVGBufferDepth;
	DescriptorHandle SRVGBufferDiffuse;
	DescriptorHandle SRVGBufferNormal;
	DescriptorHandle SRVGBufferDepth;


	auto descriptorHeaps = std::make_unique<std::unique_ptr<DescriptorHeap>[]>(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);

	descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = std::make_unique<DescriptorHeap>(pDevice.Get(), 128, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = std::make_unique<DescriptorHeap>(pDevice.Get(), 32, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = std::make_unique<DescriptorHeap>(pDevice.Get(), 256, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	RTVs[0] = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GenerateHandle();
	RTVs[1] = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GenerateHandle();
	RTVGBufferDiffuse = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GenerateHandle();
	RTVGBufferNormal  = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GenerateHandle();
	


	DSVs[0] = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->GenerateHandle();
	DSVs[1] = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->GenerateHandle();
	DSVGBufferDepth = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GenerateHandle();

	CBVs[0] = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GenerateHandle();
	CBVs[1] = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GenerateHandle();

	UAVAmbient    = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GenerateHandle();
	UAVReflection = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GenerateHandle();

	SRVGBufferDiffuse = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GenerateHandle();
	SRVGBufferNormal  = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GenerateHandle();
	SRVGBufferDepth   = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GenerateHandle();

	DX::ThrowIfFailed(pSwapChain->GetBuffer(0, IID_PPV_ARGS(pRenderTargets[0].GetAddressOf())));
	DX::ThrowIfFailed(pSwapChain->GetBuffer(1, IID_PPV_ARGS(pRenderTargets[1].GetAddressOf())));

	


	D3D12_CLEAR_VALUE depthTargetClearValue = { DXGI_FORMAT_D32_FLOAT, 0.0f };

	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthTargetClearValue,
		IID_PPV_ARGS(pDepthTargets[0].GetAddressOf())));

	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&depthTargetClearValue,
		IID_PPV_ARGS(pDepthTargets[1].GetAddressOf())));

	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthTargetClearValue,
		IID_PPV_ARGS(pGBufferDepth.GetAddressOf())));

	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(64 * (2 << 10)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(pConstantBuffers[0].GetAddressOf())));
	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(64 * (2 << 10)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(pConstantBuffers[1].GetAddressOf())));

	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		nullptr,
		IID_PPV_ARGS(pGBufferDiffuse.GetAddressOf())));


	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R16G16_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		nullptr,
		IID_PPV_ARGS(pGBufferNormal.GetAddressOf())));


	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(pTextureAmbient.GetAddressOf())));

	DX::ThrowIfFailed(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(pTextureReflection.GetAddressOf())));






	std::array<uint8_t*, 2> pDataConstBuffer;
	DX::ThrowIfFailed(pConstantBuffers[0]->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&pDataConstBuffer[0])));
	DX::ThrowIfFailed(pConstantBuffers[1]->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&pDataConstBuffer[1])));


	Model model(pDevice, cmdGraphicsContext, *descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], "C:/Users/Mikhail Gorobets/Desktop/RenderAPI/_Output/v141/x64/Release/Resource/data/sponza.dae");
	

	pDevice->CreateRenderTargetView(pRenderTargets[0].Get(), nullptr, RTVs[0].CPU);
	pDevice->CreateRenderTargetView(pRenderTargets[1].Get(), nullptr, RTVs[1].CPU);
	pDevice->CreateRenderTargetView(pGBufferDiffuse.Get(), nullptr, RTVGBufferDiffuse.CPU);
	pDevice->CreateRenderTargetView(pGBufferNormal.Get(), nullptr, RTVGBufferNormal.CPU);

	pDevice->CreateDepthStencilView(pDepthTargets[0].Get(), nullptr, DSVs[0].CPU);
	pDevice->CreateDepthStencilView(pDepthTargets[1].Get(), nullptr, DSVs[1].CPU);
	pDevice->CreateDepthStencilView(pGBufferDepth.Get(), nullptr, DSVGBufferDepth.CPU);

	pDevice->CreateConstantBufferView(&CD3D12_CONSTANT_BUFFER_VIEW_DESC(pConstantBuffers[0]->GetGPUVirtualAddress(), (sizeof(FrameConstantBuffer) + 255) & ~255), CBVs[0].CPU);
	pDevice->CreateConstantBufferView(&CD3D12_CONSTANT_BUFFER_VIEW_DESC(pConstantBuffers[1]->GetGPUVirtualAddress(), (sizeof(ObjectConstantBuffer) + 255) & ~255), CBVs[1].CPU);


	pDevice->CreateUnorderedAccessView(pTextureAmbient.Get(), nullptr, nullptr, UAVAmbient.CPU);
	pDevice->CreateUnorderedAccessView(pTextureReflection.Get(), nullptr, nullptr, UAVReflection.CPU);

	DirectX::CreateShaderResourceView(pDevice.Get(), pGBufferDiffuse.Get(), SRVGBufferDiffuse.CPU);
	DirectX::CreateShaderResourceView(pDevice.Get(), pGBufferNormal.Get(), SRVGBufferNormal.CPU);
	DirectX::CreateShaderResourceView(pDevice.Get(), pGBufferDepth.Get(), SRVGBufferDepth.CPU);

	////////////////////////////////////////////////


	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRSFillGBuffer;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pPSOFillGBuffer;
	{

		U32 compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS  | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> pCodeVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pCodePSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorPSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pRootSignatureBlob;

		if (FAILED(D3DCompileFromFile(L"FillGBuffer.hlsl", nullptr, nullptr, "VSMain", "vs_5_1", compileFlags, 0, pCodeVSBlob.GetAddressOf(), pErrorVSBlob.GetAddressOf()))) {
			std::cout << static_cast< const char*>(pErrorVSBlob->GetBufferPointer());		
		}
		
		if (FAILED(D3DCompileFromFile(L"FillGBuffer.hlsl", nullptr, nullptr, "PSMain", "ps_5_1", compileFlags, 0, pCodePSBlob.GetAddressOf(), pErrorPSBlob.GetAddressOf()))) {
			std::cout << static_cast< const char*>(pErrorPSBlob->GetBufferPointer());
		}

		DX::ThrowIfFailed(D3DStripShader(pCodeVSBlob->GetBufferPointer(), pCodeVSBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, pRootSignatureBlob.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateRootSignature(0, pRootSignatureBlob->GetBufferPointer(), pRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(pRSFillGBuffer.GetAddressOf())));

		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC piplineDesc = {};
		piplineDesc.InputLayout = { inputLayout, _countof(inputLayout) };
		piplineDesc.pRootSignature = pRSFillGBuffer.Get();
		piplineDesc.VS = { pCodeVSBlob->GetBufferPointer(), pCodeVSBlob->GetBufferSize() };
		piplineDesc.PS = { pCodePSBlob->GetBufferPointer(), pCodePSBlob->GetBufferSize() };
		piplineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		piplineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		piplineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		piplineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		
		piplineDesc.SampleMask = UINT_MAX;
		piplineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		piplineDesc.NumRenderTargets = 1;
		piplineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		piplineDesc.SampleDesc.Count = 1;

		DX::ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&piplineDesc, IID_PPV_ARGS(pPSOFillGBuffer.GetAddressOf())));
	}


	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRSAmbientOclussion;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pPSOAmbientOclission;
	{

		U32 compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> pCodeCSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorCSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pRootSignatureBlob;

		if (FAILED(D3DCompileFromFile(L"SSAO.hlsl", nullptr, nullptr, "CSMain", "cs_5_1", compileFlags, 0, pCodeCSBlob.GetAddressOf(), pErrorCSBlob.GetAddressOf()))) {
			std::cout << static_cast< const char*>(pErrorCSBlob->GetBufferPointer());
		}

		DX::ThrowIfFailed(D3DStripShader(pCodeCSBlob->GetBufferPointer(), pCodeCSBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, pRootSignatureBlob.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateRootSignature(0, pRootSignatureBlob->GetBufferPointer(), pRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(pRSAmbientOclussion.GetAddressOf())));
	
		D3D12_COMPUTE_PIPELINE_STATE_DESC piplineDesc = {};
		piplineDesc.pRootSignature = pRSAmbientOclussion.Get();
		piplineDesc.CS = { pCodeCSBlob->GetBufferPointer(), pCodeCSBlob->GetBufferSize() };

		DX::ThrowIfFailed(pDevice->CreateComputePipelineState(&piplineDesc, IID_PPV_ARGS(pPSOAmbientOclission.GetAddressOf())));
	}


	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRSReflection;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pPSOReflection;
	{
		U32 compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> pCodeCSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorCSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pRootSignatureBlob;

		if (FAILED(D3DCompileFromFile(L"SSLR.hlsl", nullptr, nullptr, "CSMain", "cs_5_1", compileFlags, 0, pCodeCSBlob.GetAddressOf(), pErrorCSBlob.GetAddressOf()))) {
			std::cout << static_cast< const char*>(pErrorCSBlob->GetBufferPointer());
		}

		DX::ThrowIfFailed(D3DStripShader(pCodeCSBlob->GetBufferPointer(), pCodeCSBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, pRootSignatureBlob.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateRootSignature(0, pRootSignatureBlob->GetBufferPointer(), pRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(pRSReflection.GetAddressOf())));

		D3D12_COMPUTE_PIPELINE_STATE_DESC piplineDesc = {};
		piplineDesc.pRootSignature = pRSReflection.Get();
		piplineDesc.CS = { pCodeCSBlob->GetBufferPointer(), pCodeCSBlob->GetBufferSize() };

		DX::ThrowIfFailed(pDevice->CreateComputePipelineState(&piplineDesc, IID_PPV_ARGS(pPSOReflection.GetAddressOf())));
	}
	

	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRSFinal;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pPSOFinal;
	{
		U32 compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> pCodeVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pCodePSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorPSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pRootSignatureBlob;

		if (FAILED(D3DCompileFromFile(L"FinalPass.hlsl", nullptr, nullptr, "VSMain", "vs_5_1", compileFlags, 0, pCodeVSBlob.GetAddressOf(), pErrorVSBlob.GetAddressOf()))) {
			std::cout << static_cast< const char*>(pErrorVSBlob->GetBufferPointer());
		}

		if (FAILED(D3DCompileFromFile(L"FinalPass.hlsl", nullptr, nullptr, "PSMain", "ps_5_1", compileFlags, 0, pCodePSBlob.GetAddressOf(), pErrorPSBlob.GetAddressOf()))) {
			std::cout << static_cast< const char*>(pErrorPSBlob->GetBufferPointer());
		}

		DX::ThrowIfFailed(D3DStripShader(pCodeVSBlob->GetBufferPointer(), pCodeVSBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, pRootSignatureBlob.GetAddressOf()));
		DX::ThrowIfFailed(pDevice->CreateRootSignature(0, pRootSignatureBlob->GetBufferPointer(), pRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(pRSFillGBuffer.GetAddressOf())));

	
		D3D12_GRAPHICS_PIPELINE_STATE_DESC piplineDesc = {};
		piplineDesc.pRootSignature = pRSFillGBuffer.Get();
		piplineDesc.VS = { pCodeVSBlob->GetBufferPointer(), pCodeVSBlob->GetBufferSize() };
		piplineDesc.PS = { pCodePSBlob->GetBufferPointer(), pCodePSBlob->GetBufferSize() };
		piplineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		piplineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		piplineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		piplineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;

		piplineDesc.SampleMask = UINT_MAX;
		piplineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		piplineDesc.NumRenderTargets = 1;
		piplineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		piplineDesc.SampleDesc.Count = 1;

		DX::ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&piplineDesc, IID_PPV_ARGS(pPSOFillGBuffer.GetAddressOf())));
	}


	auto camera = Components::Camera{};
	auto transform = Components::Transform{};




	//cmdContext.CloseCmdList();
	//cmdContext.ExecuteCmdList();
	//cmdContext.WaitForGPU();

	bool running = true;
	SDL_Event event;

	bool mouseDown = false;

	while (running) {

		{

			auto rotateSensivity = 0.001f;
			auto moveSensivity   = 0.01f;

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
					mouseDown = true;
					break;
				case SDL_MOUSEBUTTONUP:
					mouseDown = false;
					break;
				case SDL_MOUSEMOTION: {
					if (mouseDown) {							
						camera.Rotate(Components::Camera::LocalUp, -rotateSensivity * static_cast<F32>(event.motion.xrel));
						camera.Rotate(camera.Right(),  -rotateSensivity * static_cast<F32>(event.motion.yrel));
					}
					break;			
				}
				case SDL_QUIT:
					running = false;
					break;
				default:
					break;
				}
			}

			auto const* keyboardState = SDL_GetKeyboardState(NULL);

			auto distance = Math::Vec3{ 0.0f, 0.0f, 0.0f };

			if ((keyboardState[SDL_SCANCODE_DOWN]) || (keyboardState[SDL_SCANCODE_W]))
				distance += moveSensivity * camera.Forward();

			if ((keyboardState[SDL_SCANCODE_DOWN]) || (keyboardState[SDL_SCANCODE_S]))
				distance -= moveSensivity * camera.Forward();


			if ((keyboardState[SDL_SCANCODE_DOWN]) || (keyboardState[SDL_SCANCODE_A]))
				distance -= moveSensivity * camera.Right();

			if ((keyboardState[SDL_SCANCODE_DOWN]) || (keyboardState[SDL_SCANCODE_D]))
				distance += moveSensivity * camera.Right();


			if ((keyboardState[SDL_SCANCODE_DOWN]) || (keyboardState[SDL_SCANCODE_Q]))
				distance += moveSensivity * camera.Up();

			if ((keyboardState[SDL_SCANCODE_DOWN]) || (keyboardState[SDL_SCANCODE_E]))
				distance -= moveSensivity * camera.Up();

			camera.Translate(distance);

		}

	
	


		cmdGraphicsContext.ResetCmdList();
		cmdComputeContext.ResetCmdList();

		U32 frameIndex = pSwapChain->GetCurrentBackBufferIndex();
		ID3D12GraphicsCommandList* pCmdListGraphics = cmdGraphicsContext.GetCmdList();
		ID3D12GraphicsCommandList* pCmdListCompute  = cmdComputeContext.GetCmdList();



		//Z-PrePass
		{


		}

		//


		//G-Buffer Pass
		{


			ID3D12DescriptorHeap* pHeaps[] = {
				descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetHeap()
			};

			D3D12_RESOURCE_BARRIER pGBufferBeginBarriers[] = {
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferDiffuse.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferNormal.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferDepth.Get(),  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,  D3D12_RESOURCE_STATE_DEPTH_WRITE)
			};

			D3D12_RESOURCE_BARRIER pGBufferEndBarriers[] = {
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferNormal.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ),
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferDepth.Get(),  D3D12_RESOURCE_STATE_DEPTH_WRITE,  D3D12_RESOURCE_STATE_DEPTH_READ)
			};
		
		
			D3D12_CPU_DESCRIPTOR_HANDLE pRTVs[] = {
				RTVGBufferDiffuse.CPU,
				RTVGBufferNormal.CPU
			};

		
			pCmdListGraphics->RSSetViewports(1, &CD3DX12_VIEWPORT{ 0.0f, 0.0f, static_cast<F32>(WINDOW_WIDTH),static_cast<F32>(WINDOW_HEIGHT), 0.0f, 1.0f });
			pCmdListGraphics->RSSetScissorRects(1, &CD3DX12_RECT{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT });
			pCmdListGraphics->ResourceBarrier(_countof(pGBufferBeginBarriers), pGBufferBeginBarriers);
			pCmdListGraphics->OMSetRenderTargets(_countof(pRTVs), pRTVs, false, &DSVGBufferDepth.CPU);


		

			pCmdListGraphics->ClearRenderTargetView(RTVGBufferDiffuse.CPU, std::data({ 0.0f, 0.0f, 0.0f, 1.0f }), 0, nullptr);
			pCmdListGraphics->ClearRenderTargetView(RTVGBufferNormal.CPU,  std::data({ 0.0f, 0.0f, 0.0f, 1.0f }), 0, nullptr);
			pCmdListGraphics->ClearDepthStencilView(DSVGBufferDepth.CPU, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);


			pCmdListGraphics->SetPipelineState(pPSOFillGBuffer.Get());
			pCmdListGraphics->SetDescriptorHeaps(_countof(pHeaps), pHeaps);
			pCmdListGraphics->SetGraphicsRootSignature(pRSFillGBuffer.Get());


			pCmdListGraphics->SetGraphicsRootConstantBufferView(1, pConstantBuffers[0]->GetGPUVirtualAddress());
			pCmdListGraphics->SetGraphicsRootConstantBufferView(2, pConstantBuffers[1]->GetGPUVirtualAddress());
			pCmdListGraphics->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			model.Draw(cmdGraphicsContext);
			pCmdListGraphics->ResourceBarrier(_countof(pGBufferEndBarriers), pGBufferEndBarriers);

		

		}

		//Ambient Pass
		{

			ID3D12DescriptorHeap* pHeaps[] = {
				descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetHeap()
			};

			D3D12_RESOURCE_BARRIER pAmbientBarriers[] = {
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferDiffuse.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferNormal.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
				CD3DX12_RESOURCE_BARRIER::Transition(pGBufferDepth.Get(),  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,  D3D12_RESOURCE_STATE_DEPTH_WRITE)
			};


			pCmdListCompute->SetPipelineState(pPSOAmbientOclission.Get());
			pCmdListCompute->SetDescriptorHeaps(_countof(pHeaps), pHeaps);
			//pCmdListCompute->SetDescriptorTable(_countof(pHeaps), pHeaps);
			pCmdListCompute->ResourceBarrier(_countof(pAmbientBarriers), pAmbientBarriers);
			pCmdListCompute->Dispatch(0, 0, 0);

		}

		//SSLR Pass
		{



		}


		//Final Pass
		{

			D3D12_RESOURCE_BARRIER pBeginFrameBarriers[] = {
				CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
			};

			D3D12_RESOURCE_BARRIER pEndFrameBarriers[] = {
				CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,  D3D12_RESOURCE_STATE_PRESENT)
			};

			pCmdListGraphics->RSSetViewports(1, &CD3DX12_VIEWPORT{ 0.0f, 0.0f, static_cast<F32>(WINDOW_WIDTH),static_cast<F32>(WINDOW_HEIGHT), 0.0f, 1.0f });
			pCmdListGraphics->RSSetScissorRects(1, &CD3DX12_RECT{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT });
			pCmdListGraphics->ResourceBarrier(_countof(pBeginFrameBarriers), pBeginFrameBarriers);
			pCmdListGraphics->OMSetRenderTargets(1, &RTVs[frameIndex].CPU, false, &DSVs[0].CPU);


			pCmdListGraphics->ClearRenderTargetView(RTVs[frameIndex].CPU, std::data({ 0.0f, 0.0f, 0.0f, 1.0f }), 0, nullptr);
			pCmdListGraphics->ClearDepthStencilView(DSVs[0].CPU, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);




			
			pCmdListGraphics->ResourceBarrier(_countof(pEndFrameBarriers), pEndFrameBarriers);


		}


		cmdGraphicsContext.CloseCmdList();
		cmdGraphicsContext.ExecuteCmdList();
		DX::ThrowIfFailed(pSwapChain->Present(0, 0));
		cmdGraphicsContext.WaitForGPU();




		{
			FrameConstantBuffer frameBuffer;
			frameBuffer.View    = camera.ToMatrix();
			frameBuffer.Project = Math::Perspective(3.14f / 4.0f, static_cast<F32>(WINDOW_WIDTH) / static_cast<F32>(WINDOW_HEIGHT), 0.1f, 1000.0f);
			std::memcpy(pDataConstBuffer[0], &frameBuffer, sizeof(FrameConstantBuffer));

			ObjectConstantBuffer objectBuffer;		
			objectBuffer.World  = transform.ToMatrix();
			objectBuffer.WVP    = frameBuffer.Project * camera.ToMatrix() * transform.ToMatrix();
			objectBuffer.Normal = Math::Convert<Math::Quat, Math::Mat4x4>(transform.Rotation());
			std::memcpy(pDataConstBuffer[1], &objectBuffer, sizeof(ObjectConstantBuffer));

		}

		


	}


	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}





Model::Model(Microsoft::WRL::ComPtr<ID3D12Device> device, CommandContext& context, DescriptorHeap& heap, std::string filename) {

	m_Directory = filename.substr(0, filename.find_last_of('/'));

	Assimp::Importer importer;

	auto pScene = importer.ReadFile(filename,
		aiProcess_FlipUVs |
		aiProcess_GenNormals |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_FixInfacingNormals |
		aiProcess_ImproveCacheLocality |
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes |
		aiProcessPreset_TargetRealtime_MaxQuality);

	if (pScene == NULL)
		throw std::runtime_error("Dont't load file: " + filename);


	std::vector<Vertex>   vertices;
	std::vector<uint32_t> indices;

	Material materialDefault;	
	
	auto filenameDiffuse  = m_Directory + "/sponza/dummy.dds";
	auto filenameSpecular = m_Directory + "/sponza/dummy_specular.dds";
	auto filenameNormal   = m_Directory + "/sponza/dummy_ddn.dds";


	DirectX::ResourceUploadBatch resourceUpload{ device.Get() };
	resourceUpload.Begin();
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device.Get(), resourceUpload, std::wstring(filenameDiffuse.begin(),  filenameDiffuse.end()).c_str(), materialDefault.TexDiffuse.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device.Get(), resourceUpload, std::wstring(filenameSpecular.begin(), filenameSpecular.end()).c_str(), materialDefault.TexSpecular.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device.Get(), resourceUpload, std::wstring(filenameNormal.begin(),   filenameNormal.end()).c_str(), materialDefault.TexNormal.ReleaseAndGetAddressOf()));
	resourceUpload.End(context.GetCmdQueue()).wait();

	m_Materials.push_back(materialDefault);
	
	for (auto indexMaterial = 0u; indexMaterial < pScene->mNumMaterials; indexMaterial++) {

		aiString  strDiffuse;
		aiString  strSpecular;
		aiString  strNormal;
		aiColor3D ambient;
		aiColor3D specular;
		aiColor3D emissive;
		aiColor3D transperent;

		pScene->mMaterials[indexMaterial]->GetTexture(aiTextureType_DIFFUSE, 0, &strDiffuse);
		pScene->mMaterials[indexMaterial]->GetTexture(aiTextureType_SPECULAR, 0, &strSpecular);
		pScene->mMaterials[indexMaterial]->GetTexture(aiTextureType_NORMALS, 0, &strNormal);
		pScene->mMaterials[indexMaterial]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		pScene->mMaterials[indexMaterial]->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		pScene->mMaterials[indexMaterial]->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		pScene->mMaterials[indexMaterial]->Get(AI_MATKEY_COLOR_TRANSPARENT, transperent);
	
		Material material{ materialDefault };
		
		DirectX::ResourceUploadBatch resourceUpload{ device.Get() };
		resourceUpload.Begin();

		if (pScene->mMaterials[indexMaterial]->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			filenameDiffuse = m_Directory + strDiffuse.C_Str();
			DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device.Get(), resourceUpload, std::wstring(filenameDiffuse.begin(), filenameDiffuse.end()).c_str(), material.TexDiffuse.GetAddressOf()));
		}

		if (pScene->mMaterials[indexMaterial]->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			filenameSpecular = m_Directory + strSpecular.C_Str();
			DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device.Get(), resourceUpload, std::wstring(filenameSpecular.begin(), filenameSpecular.end()).c_str(), material.TexSpecular.GetAddressOf()));
		}

		if (pScene->mMaterials[indexMaterial]->GetTextureCount(aiTextureType_NORMALS) > 0) {
			filenameNormal = m_Directory + strNormal.C_Str();
			DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device.Get(), resourceUpload, std::wstring(filenameNormal.begin(), filenameNormal.end()).c_str(), material.TexNormal.GetAddressOf()));
		}

		resourceUpload.End(context.GetCmdQueue()).wait();

		m_SRVs.push_back(heap.GenerateHandle());
		m_SRVs.push_back(heap.GenerateHandle());
		m_SRVs.push_back(heap.GenerateHandle());

		DirectX::CreateShaderResourceView(device.Get(), material.TexDiffuse.Get(),  m_SRVs[3 * indexMaterial + 0].CPU);
		DirectX::CreateShaderResourceView(device.Get(), material.TexSpecular.Get(), m_SRVs[3 * indexMaterial + 1].CPU);
		DirectX::CreateShaderResourceView(device.Get(), material.TexNormal.Get(),   m_SRVs[3 * indexMaterial + 2].CPU);

		m_Materials.push_back(material);
	
	}

	
	m_CountVertices = 0;
	m_CountIndexes  = 0;

	for (auto indexMesh = 0u; indexMesh < pScene->mNumMeshes; indexMesh++) {

		for (auto indexElement = 0u; indexElement < pScene->mMeshes[indexMesh]->mNumVertices; indexElement++) {

			Vertex vertex;

			vertex.Position = {
				pScene->mMeshes[indexMesh]->mVertices[indexElement].x,
				pScene->mMeshes[indexMesh]->mVertices[indexElement].y,
				pScene->mMeshes[indexMesh]->mVertices[indexElement].z };
			vertex.Normal = {
				pScene->mMeshes[indexMesh]->mNormals[indexElement].x,
				pScene->mMeshes[indexMesh]->mNormals[indexElement].y,
				pScene->mMeshes[indexMesh]->mNormals[indexElement].z };

			vertex.Tangent = {
				pScene->mMeshes[indexMesh]->mTangents[indexElement].x,
				pScene->mMeshes[indexMesh]->mTangents[indexElement].y,
				pScene->mMeshes[indexMesh]->mTangents[indexElement].z };

			vertex.Texcoord = {
				pScene->mMeshes[indexMesh]->mTextureCoords[0][indexElement].x,
				pScene->mMeshes[indexMesh]->mTextureCoords[0][indexElement].y };

			vertices.push_back(vertex);
		}

		for (auto indexElement = 0u; indexElement < pScene->mMeshes[indexMesh]->mNumFaces; indexElement++) {
			aiFace face = pScene->mMeshes[indexMesh]->mFaces[indexElement];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		
		Mesh mesh;
		mesh.IndexMaterial = pScene->mMeshes[indexMesh]->mMaterialIndex;
		mesh.CountIndexes = 3 * pScene->mMeshes[indexMesh]->mNumFaces;
		mesh.Offset     = m_CountIndexes;
		mesh.VertexBase = m_CountVertices;
		m_Meshes.push_back(mesh);

		m_CountVertices += pScene->mMeshes[indexMesh]->mNumVertices;
		m_CountIndexes  += pScene->mMeshes[indexMesh]->mNumFaces * 3;
			
	}

	




	{
		Microsoft::WRL::ComPtr<ID3D12Resource> pUploadBuffer;

		DX::ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(Vertex)),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_VertexBuffer.GetAddressOf())));


		DX::ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_VertexBuffer.Get(), 0, 1)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pUploadBuffer.GetAddressOf())));


		D3D12_SUBRESOURCE_DATA pData = {};
		pData.pData = vertices.data();
		pData.RowPitch = sizeof(Vertex) * vertices.size();
		pData.SlicePitch = sizeof(Vertex) * vertices.size();

	//	context.ResetCmdList();
		UpdateSubresources(context.GetCmdList(), m_VertexBuffer.Get(), pUploadBuffer.Get(), 0, 0, 1, &pData);
		context.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
		context.CloseCmdList();
		context.ExecuteCmdList();
		context.WaitForGPU();

		m_VBV.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VBV.StrideInBytes  = sizeof(Vertex);
		m_VBV.SizeInBytes    = static_cast<uint32_t>(sizeof(Vertex) * vertices.size());

	}




	{

		std::sort(m_Meshes.begin(), m_Meshes.end(), [](auto const& x, auto const& y) { return  x.IndexMaterial < y.IndexMaterial;  });

		std::vector<uint32_t> optimazeIndices;
		for (auto& e : m_Meshes) {
			auto index  = e.Offset;
			auto count  = e.CountIndexes;
			auto offset = index + count;
			e.Offset = static_cast<uint32_t>(optimazeIndices.size());
			for (; index < offset; index++)
				optimazeIndices.push_back(indices[index]);
		}


		Microsoft::WRL::ComPtr<ID3D12Resource>  pUploadBuffer;

		DX::ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(uint32_t)),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_IndexBuffer.GetAddressOf())));

		DX::ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_IndexBuffer.Get(), 0, 1)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pUploadBuffer.GetAddressOf())));

		D3D12_SUBRESOURCE_DATA pData = {};
		pData.pData = optimazeIndices.data();
		pData.RowPitch = optimazeIndices.size() * sizeof(uint32_t);
		pData.SlicePitch = optimazeIndices.size() * sizeof(uint32_t);


		context.ResetCmdList();
		UpdateSubresources(context.GetCmdList(), m_IndexBuffer.Get(), pUploadBuffer.Get(), 0, 0, 1, &pData);
		context.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));
		context.CloseCmdList();
		context.ExecuteCmdList();
		context.WaitForGPU();

		m_IBV.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IBV.Format = DXGI_FORMAT_R32_UINT;
		m_IBV.SizeInBytes = static_cast<uint32_t>(optimazeIndices.size() * sizeof(uint32_t));


	}

	

}

auto Model::Draw(CommandContext& context) const noexcept -> void {

	ID3D12GraphicsCommandList* pCommandList = context.GetCmdList();	
	pCommandList->IASetVertexBuffers(0, 1, &m_VBV);
	pCommandList->IASetIndexBuffer(&m_IBV);
	pCommandList->SetGraphicsRootDescriptorTable(3, m_SRVs[0].GPU);

	for (auto const& e : m_Meshes) {
		pCommandList->SetGraphicsRoot32BitConstant(0, e.IndexMaterial, 0);
		pCommandList->DrawIndexedInstanced(e.CountIndexes, 1, e.Offset, e.VertexBase, 0);
	}

}



