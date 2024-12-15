#include "Graphics.h"
#include "GEngine/Graphics/GraphicsError.h"
#include <GEngine/src/log/Log.h> 
#include <GEngine/src/utl/String.h> 

#include <cmath> 
#include <numbers> 
#include <ranges>

namespace Genesis
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;
	using namespace chil;
	using namespace chil::app;
	namespace rn = std::ranges;
	namespace vi = rn::views;

	// data structure for vertex data 
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 tc;
	};

	// the content data 
	const Vertex vertexData[] = {
		{ {-1.0f, -1.0f, -1.0f}, { 0.f, 0.f } }, // 0 
		{ {-1.0f,  1.0f, -1.0f}, { 0.f, 1.f } }, // 1 
		{ {1.0f,  1.0f, -1.0f}, { 1.f, 1.f } }, // 2 
		{ {1.0f, -1.0f, -1.0f}, { 1.f, 0.f } }, // 3 
		{ {-1.0f, -1.0f,  1.0f}, { 0.f, 1.f } }, // 4 
		{ {-1.0f,  1.0f,  1.0f}, { 0.f, 0.f } }, // 5 
		{ {1.0f,  1.0f,  1.0f}, { 1.f, 0.f } }, // 6 
		{ {1.0f, -1.0f,  1.0f}, { 1.f, 1.f } }  // 7 
	};

	const WORD indexData[] = {
					0, 1, 2, 0, 2, 3,
					4, 6, 5, 4, 7, 6,
					4, 5, 1, 4, 1, 0,
					3, 2, 6, 3, 6, 7,
					1, 5, 6, 1, 6, 2,
					4, 0, 3, 4, 3, 7
	};

	Graphics::Graphics()
	{
	}
	void Graphics::Initialize(chil::win::IWindow& window)
	{
		// init COM
		CoInitializeEx(nullptr, COINIT_MULTITHREADED) >> chk;
		SetupDebugLayer();
		CreateFactory();
		CreateDevice();
		CreateCommandObjects();
		CreateSwapChain(window);
		CreateRenderTargets();
		CreateDepthBuffer();
		CreateFence();
		SetScissorAndViewport();

		// Prep for model class
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateTexture();
		SetViewMatrix();
		CreateDescriptorHeaps();
		CreateRootSignature();
		CreatePipelineState();	
		
	}

	void Graphics::BeginFrame(float deltaTime)
	{
		// advance back buffer
		curBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
		ResetCommandList();

		// transition buffer resource to render target state 
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffers[curBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &barrier);

		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv{
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		(INT)curBackBufferIndex, rtvDescriptorSize };

		// calculate clear color 
		const FLOAT clearColor[] = {
			sin(2.f * t + 1.f) / 2.f + .5f,
			sin(3.f * t + 2.f) / 2.f + .5f,
			sin(5.f * t + 3.f) / 2.f + .5f,
			1.0f
		};
		// clear rtv 
		commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		// clear the depth buffer 
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

		// configure RS 
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorRect);
	}

	void Graphics::EndFrame(float deltaTime)
	{
		

		// prepare buffer for presentation by transitioning to present state
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffers[curBackBufferIndex].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);
		}

		CloseAndExecuteCommandList();

		// insert fence to mark command list completion 
		commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
		// present frame 
		swapChain->Present(1, 0) >> chk;

		SignalAndWait();

		// update simulation time 
		t += step;
	}

	void Graphics::TestLoop()
	{
#pragma region Chili code
		// set pipeline state 
		commandList->SetPipelineState(pipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.Get());
		// get rtv handle for the buffer used in this frame
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv{
			rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			(INT)curBackBufferIndex, rtvDescriptorSize };

		// bind render target and depth
		commandList->OMSetRenderTargets(1, &rtv, TRUE, &dsvHandle);
		// configure IA 
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);
		// bind the heap containing the texture descriptor 
		commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
		// bind the descriptor table containing the texture descriptor 
		commandList->SetGraphicsRootDescriptorTable(1, srvHeap->GetGPUDescriptorHandleForHeapStart());		

		// draw cube #1 
		{
			// bind the transformation matrix 
			const auto mvp = XMMatrixTranspose(
				XMMatrixRotationX(1.0f * t + 1.f) *
				XMMatrixRotationY(1.2f * t + 2.f) *
				XMMatrixRotationZ(1.1f * t + 0.f) *
				viewProjection
			);
			commandList->SetGraphicsRoot32BitConstants(0, sizeof(mvp) / 4, &mvp, 0);
			// draw the geometry  
			commandList->DrawIndexedInstanced(nIndices, 1, 0, 0, 0);
		}
#pragma endregion
	}

	void Graphics::SetupDebugLayer()
	{
		// enable the software debug layer for d3d12 
		{
			ComPtr<ID3D12Debug1> debugController;
			D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) >> chk;
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(true);
		}
	}

	void Graphics::CreateFactory()
	{
		// dxgi factory 
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory)) >> chk;
	}

	void Graphics::CreateDevice()
	{
		// device
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)) >> chk;
	}

	void Graphics::CreateCommandObjects()
	{
		// command queue
		const D3D12_COMMAND_QUEUE_DESC desc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0,
		};
		device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)) >> chk;

		// command allocator
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator)) >> chk;

		// command list
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)) >> chk;
		// initially close the command list so it can be reset at top of draw loop 
		commandList->Close() >> chk;
	}

	void Graphics::CreateSwapChain(chil::win::IWindow& window)
	{
		// swap chain
		const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
			.Width = width,
			.Height = height,
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.Stereo = FALSE,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = bufferCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = 0,
		};
		ComPtr<IDXGISwapChain1> swapChain1;
		dxgiFactory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			window.GetHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1) >> chk;
		swapChain1.As(&swapChain) >> chk;
	}

	void Graphics::CreateRenderTargets()
	{
		// rtv descriptor heap
		const D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = bufferCount,
		};
		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap)) >> chk;

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// rtv descriptors and buffer references
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < bufferCount; i++)
		{
			swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])) >> chk;
			device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}
	}

	void Graphics::CreateDepthBuffer()
	{
		// depth buffer 
		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			width, height,
			1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		const D3D12_CLEAR_VALUE clearValue = {
			.Format = DXGI_FORMAT_D32_FLOAT,
			.DepthStencil = { 1.0f, 0 },
		};
		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&depthBuffer)) >> chk;

		// dsv descriptor heap
		const D3D12_DESCRIPTOR_HEAP_DESC desc1 = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1,
		};
		device->CreateDescriptorHeap(&desc1, IID_PPV_ARGS(&dsvDescriptorHeap)) >> chk;

		// dsv and handle 
		dsvHandle = {
			dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

		device->CreateDepthStencilView(depthBuffer.Get(), nullptr, dsvHandle);
	}

	void Graphics::CreateFence()
	{
		// fence
		device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) >> chk;

		// fence signalling event
		fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent)
		{
			GetLastError() >> chk;
			throw std::runtime_error{ "Failed to create fence event" };
		}
	}

	void Graphics::CreateDescriptorHeaps()
	{
		// descriptor heap for the shader resource view 
		{
			const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
			};
			device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)) >> chk;
		}

		// create handle to the srv heap and to the only view in the heap 
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());

		// create the descriptor in the heap 
		{
			const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = cubeFaceTexture->GetDesc().Format,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D{.MipLevels = cubeFaceTexture->GetDesc().MipLevels },
			};
			device->CreateShaderResourceView(cubeFaceTexture.Get(), &srvDesc, srvHandle);
		}
	}

	void Graphics::ResetCommandList()
	{
		// reset command list and allocator  
		commandAllocator->Reset() >> chk;
		commandList->Reset(commandAllocator.Get(), nullptr) >> chk;
	}

	void Graphics::CloseAndExecuteCommandList()
	{
		// close command list  
		commandList->Close() >> chk;
		// submit command list to queue as array with single element 
		ID3D12CommandList* const commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
	}

	void Graphics::SignalAndWait()
	{
		// insert fence to detect when upload is complete 
		commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
		fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
		if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
		{
			GetLastError() >> chk;
		}
	}

	void Graphics::CreateRootSignature()
	{
		// create root signature 

		{
			// define root signature with a matrix of 16 32-bit floats used by the vertex shader (mvp matrix) 
			CD3DX12_ROOT_PARAMETER rootParameters[2]{};
			rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			{
				const CD3DX12_DESCRIPTOR_RANGE descRange{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 };
				rootParameters[1].InitAsDescriptorTable(1, &descRange);
			}
			// Allow input layout and vertex shader and deny unnecessary access to certain pipeline stages.
			const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
			// define static sampler 
			const CD3DX12_STATIC_SAMPLER_DESC staticSampler{ 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR };
			// define root signature with transformation matrix
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(
				(UINT)std::size(rootParameters), rootParameters,
				1, &staticSampler,
				rootSignatureFlags);
			// serialize root signature 

			ComPtr<ID3DBlob> errorBlob;
			if (const auto hr = D3D12SerializeRootSignature(
				&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
				&signatureBlob, &errorBlob); FAILED(hr))
			{
				if (errorBlob)
				{
					auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
					chilog.error(utl::ToWide(errorBufferPtr)).no_trace();
				}
				hr >> chk;
			}
			// Create the root signature. 
			device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
				signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)) >> chk;
		}
	}

	void Graphics::CreatePipelineState()
	{
		// creating pipeline state object 
		{
			// static declaration of pso stream structure 
			struct PipelineStateStream
			{
				CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
				CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
				CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
				CD3DX12_PIPELINE_STATE_STREAM_VS VS;
				CD3DX12_PIPELINE_STATE_STREAM_PS PS;
				CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
				CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			} pipelineStateStream;

			// define the Vertex input layout 
			const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			// Load the vertex shader. 
			ComPtr<ID3DBlob> vertexShaderBlob;
			D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob) >> chk;

			// Load the pixel shader. 
			ComPtr<ID3DBlob> pixelShaderBlob;
			D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob) >> chk;

			// filling pso structure 
			pipelineStateStream.RootSignature = rootSignature.Get();
			pipelineStateStream.InputLayout = { inputLayout, (UINT)std::size(inputLayout) };
			pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
			pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
			pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			pipelineStateStream.RTVFormats = {
				.RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
				.NumRenderTargets = 1,
			};

			// building the pipeline state object 
			const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
				sizeof(PipelineStateStream), &pipelineStateStream
			};
			device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)) >> chk;
		}
	}

	void Graphics::SetScissorAndViewport()
	{
		// define scissor rect
		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = LONG_MAX;
		scissorRect.bottom = LONG_MAX;

		// define viewport 
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = float(width);
		viewport.Height = float(height);
		//viewport.MinDepth = 0;
		//viewport.MaxDepth = 1;
	}

	void Graphics::CreateVertexBuffer()
	{
		// set the vertex count 
		nVertices = (UINT)std::size(vertexData);
		// create committed resource for vertex buffer 
		{
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
			device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr, IID_PPV_ARGS(&vertexBuffer)
			) >> chk;
		}
		// create committed resource for cpu upload of vertex data 

		{
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
			device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&vertexUploadBuffer)
			) >> chk;
		}
		// copy array of vertex data to upload buffer 
		{
			Vertex* mappedVertexData = nullptr;
			vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData)) >> chk;
			rn::copy(vertexData, mappedVertexData);
			vertexUploadBuffer->Unmap(0, nullptr);
		}
		ResetCommandList();
		// copy upload buffer to vertex buffer 
		commandList->CopyResource(vertexBuffer.Get(), vertexUploadBuffer.Get());
		// transition vertex buffer to vertex buffer state 
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				vertexBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			commandList->ResourceBarrier(1, &barrier);
		}
		CloseAndExecuteCommandList();
		SignalAndWait();


		// Create the vertex buffer view. 
		vertexBufferView = {
			.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = nVertices * sizeof(Vertex),
			.StrideInBytes = sizeof(Vertex),
		};
	}

	void Graphics::CreateIndexBuffer()
	{
		// create index buffer  
		{
			// the content data  

			// set the index count  
			nIndices = (UINT)std::size(indexData);
			// create committed resource for index buffer  
			{
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexData));
				device->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr, IID_PPV_ARGS(&indexBuffer)
				) >> chk;
			}
			// create committed resource for cpu upload of index data  

			{
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexData));
				device->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr, IID_PPV_ARGS(&indexUploadBuffer)
				) >> chk;
			}
			// copy array of index data to upload buffer  
			{
				WORD* mappedIndexData = nullptr;
				indexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData)) >> chk;
				rn::copy(indexData, mappedIndexData);
				indexUploadBuffer->Unmap(0, nullptr);
			}
			// reset command list and allocator   
			ResetCommandList();
			// copy upload buffer to index buffer  
			commandList->CopyResource(indexBuffer.Get(), indexUploadBuffer.Get());
			// transition index buffer to index buffer state 
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					indexBuffer.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
				commandList->ResourceBarrier(1, &barrier);
			}
			CloseAndExecuteCommandList();
			SignalAndWait();
		}

		// Create the index buffer view.  
		indexBufferView = {
			.BufferLocation = indexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = nIndices * sizeof(WORD),
			.Format = DXGI_FORMAT_R16_UINT,
		};
	}

	void Graphics::CreateTexture()
	{
		// create the cube texture 
		{
			// load image data from disk 
			DirectX::ScratchImage image;
			DirectX::LoadFromWICFile(L"cube_face.jpg", DirectX::WIC_FLAGS_NONE, nullptr, image) >> chk;

			// generate mip chain 
			DirectX::ScratchImage mipChain;
			DirectX::GenerateMipMaps(*image.GetImages(), DirectX::TEX_FILTER_BOX, 0, mipChain) >> chk;

			// create texture resource 
			{
				const auto& chainBase = *mipChain.GetImages();
				const D3D12_RESOURCE_DESC texDesc{
					.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
					.Width = (UINT)chainBase.width,
					.Height = (UINT)chainBase.height,
					.DepthOrArraySize = 1,
					.MipLevels = (UINT16)mipChain.GetImageCount(),
					.Format = chainBase.format,
					.SampleDesc = {.Count = 1 },
					.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
					.Flags = D3D12_RESOURCE_FLAG_NONE,
				};
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
				device->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&texDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&cubeFaceTexture)
				) >> chk;
			}

			// collect subresource data 
			const auto subresourceData = vi::iota(0, (int)mipChain.GetImageCount()) |
				vi::transform([&](int i) {
				const auto img = mipChain.GetImage(i, 0, 0);
				return D3D12_SUBRESOURCE_DATA{
					.pData = img->pixels,
					.RowPitch = (LONG_PTR)img->rowPitch,
					.SlicePitch = (LONG_PTR)img->slicePitch,
				};
					}) |
				rn::to<std::vector>();

					// create the intermediate upload buffer 

					{
						const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
						const auto uploadBufferSize = GetRequiredIntermediateSize(
							cubeFaceTexture.Get(), 0, (UINT)subresourceData.size()
						);
						const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
						device->CreateCommittedResource(
							&heapProps,
							D3D12_HEAP_FLAG_NONE,
							&resourceDesc,
							D3D12_RESOURCE_STATE_GENERIC_READ,
							nullptr,
							IID_PPV_ARGS(&uploadBuffer)
						) >> chk;
					}

					ResetCommandList();
					// write commands to copy data to upload texture (copying each subresource) 
					UpdateSubresources(
						commandList.Get(),
						cubeFaceTexture.Get(),
						uploadBuffer.Get(),
						0, 0,
						(UINT)subresourceData.size(),
						subresourceData.data()
					);
					// write command to transition texture to texture state  
					{
						const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
							cubeFaceTexture.Get(),
							D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
						commandList->ResourceBarrier(1, &barrier);
					}
					// close and execute command list						
					CloseAndExecuteCommandList();
					SignalAndWait();
		}
	}

	void Graphics::SetViewMatrix()
	{
		// set view projection matrix
		{
			// setup view (camera) matrix
			const auto eyePosition = XMVectorSet(0, 0, -6, 1);
			const auto focusPoint = XMVectorSet(0, 0, 0, 1);
			const auto upDirection = XMVectorSet(0, 1, 0, 0);
			const auto view = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
			// setup perspective projection matrix
			const auto aspectRatio = float(width) / float(height);
			const auto projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.f), aspectRatio, 0.1f, 100.0f);
			// combine matrices
			viewProjection = view * projection;
		}
	}

	void Graphics::Shutdown()
	{
		// wait for queue to become completely empty (2 seconds max) 
		commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
		fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
		if (WaitForSingleObject(fenceEvent, 2000) == WAIT_FAILED)
		{
			GetLastError() >> chk;
		}
	}

}