#pragma once
#include "GEngine/Graphics/GraphicsError.h"
#include <GEngine/src/log/Log.h> 
#include <GEngine/src/utl/String.h> 

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
	class ModelBase
	{
	public:
		ModelBase() = default;
		virtual ~ModelBase() = default;
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void Draw(float deltaTime) = 0;

	protected:
		virtual void CreateVertexBuffer() = 0;
		virtual void CreateIndexBuffer() = 0;
		virtual void CreateTexture() = 0;
		virtual void SetViewMatrix() = 0;
		virtual void CreateDescriptorHeaps() = 0;
		virtual void CreateRootSignature() = 0;
		virtual void CreatePipelineState() = 0;

	protected:
		UINT nIndices;
		UINT nVertices;

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
