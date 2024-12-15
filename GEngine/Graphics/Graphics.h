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
		void CreateSwapChain(chil::win::IWindow& window);
		void createRenderTargets();
		void CreateDepthBuffer();
		void CreateFence();
		
		void ResetCommandList();
		void CloseAndExecuteCommandList();
		void SignalAndWait();	
		void SetScissorAndViewport();

		// Model class prep
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateTexture();
		void SetViewMatrix();
		void CreateDescriptorHeaps();
		void CreateRootSignature();
		void CreatePipelineState();

		void TestLoop();
		// Public variables
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

		// Command objects
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

		// Fence stuff
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		uint64_t fenceValue = 0;
		HANDLE fenceEvent;

		// Heaps
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
		UINT rtvDescriptorSize;	
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;	
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
		
		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;

		// other
		float deltaTime = 0.0f;
		// render loop 
		UINT curBackBufferIndex;
		float t = 0.f;
		float step = 0.01f;	

		// Model class stuff?
		DirectX::XMMATRIX viewProjection;
		// Vertex buffer stuff
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		// Index buffer stuff
		Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;		
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		// Texture stuff ?
		Microsoft::WRL::ComPtr<ID3D12Resource> cubeFaceTexture;
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;

	
	};
}

