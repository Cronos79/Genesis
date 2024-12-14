#pragma once
#include <GEngine/src/win/IWindow.h>

#include <initguid.h> 
#include <d3d12.h> 
#include <dxgi1_6.h> 
#include <d3dcompiler.h> 
#include <DirectXMath.h> 
#pragma warning(push)
#pragma warning(disable : 26495)
#include "GEngine/Graphics/d3dx12.h" 
#pragma warning(pop)
#include <DirectXTex.h>
#include <wrl.h>

namespace Genesis
{
	class Graphics
	{
		// Public methods
	public:
		Graphics();
		virtual ~Graphics() = default;

		void Initialize(chil::win::IWindow& window);
		void BeginFrame(float deltaTime);
		void EndFrame(float deltaTime);	
		void Shutdown();
		// Private methods
	private:
		void SetupDebugLayer();
		void CreateFactory();
		void CreateDevice();
		void CreateCommandObjects();


		void TestInit(chil::win::IWindow& window);
		void TestLoop();
		// Pub;ic variables
	public:

		// Private variables
	private:
		// constants 
		const UINT width = 1280;
		const UINT height = 720;
		const static UINT bufferCount = 2;
		UINT nIndices;
		UINT nVertices;

		// d3d12 objects
		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device2> device;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[bufferCount];
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
		UINT rtvDescriptorSize;
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		Microsoft::WRL::ComPtr<ID3D12Resource> cubeFaceTexture;
		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
		DirectX::XMMATRIX viewProjection;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		uint64_t fenceValue = 0;
		HANDLE fenceEvent;


		// junk
		float deltaTime = 0.0f;
		// render loop 
		UINT curBackBufferIndex;
		float t = 0.f;
		float step = 0.01f;
	};
}
