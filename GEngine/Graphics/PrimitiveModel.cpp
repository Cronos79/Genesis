#include "PrimitiveModel.h"

void PrimitiveModel::Initialize()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateTexture();
	SetViewMatrix();
	CreateDescriptorHeaps();
	CreateRootSignature();
	CreatePipelineState();
}

void PrimitiveModel::Shutdown()
{
	
}

void PrimitiveModel::Draw()
{
	//// set pipeline state 
	//commandList->SetPipelineState(pipelineState.Get());
	//commandList->SetGraphicsRootSignature(rootSignature.Get());
	//// get rtv handle for the buffer used in this frame
	//const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv{
	//	rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
	//	(INT)curBackBufferIndex, rtvDescriptorSize };

	//// bind render target and depth
	//commandList->OMSetRenderTargets(1, &rtv, TRUE, &dsvHandle);
	//// configure IA 
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	//commandList->IASetIndexBuffer(&indexBufferView);
	//// bind the heap containing the texture descriptor 
	//commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	//// bind the descriptor table containing the texture descriptor 
	//commandList->SetGraphicsRootDescriptorTable(1, srvHeap->GetGPUDescriptorHandleForHeapStart());

	//// draw cube #1 
	//{
	//	// bind the transformation matrix 
	//	const auto mvp = XMMatrixTranspose(
	//		XMMatrixRotationX(1.0f * t + 1.f) *
	//		XMMatrixRotationY(1.2f * t + 2.f) *
	//		XMMatrixRotationZ(1.1f * t + 0.f) *
	//		viewProjection
	//	);
	//	commandList->SetGraphicsRoot32BitConstants(0, sizeof(mvp) / 4, &mvp, 0);
	//	// draw the geometry  
	//	commandList->DrawIndexedInstanced(nIndices, 1, 0, 0, 0);
	//}
}

void PrimitiveModel::CreateVertexBuffer()
{
	//// set the vertex count 
	//nVertices = (UINT)std::size(vertexData);
	//// create committed resource for vertex buffer 
	//{
	//	const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
	//	const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
	//	device->CreateCommittedResource(
	//		&heapProps,
	//		D3D12_HEAP_FLAG_NONE,
	//		&resourceDesc,
	//		D3D12_RESOURCE_STATE_COPY_DEST,
	//		nullptr, IID_PPV_ARGS(&vertexBuffer)
	//	) >> chk;
	//}
	//// create committed resource for cpu upload of vertex data 

	//{
	//	const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
	//	const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
	//	device->CreateCommittedResource(
	//		&heapProps,
	//		D3D12_HEAP_FLAG_NONE,
	//		&resourceDesc,
	//		D3D12_RESOURCE_STATE_GENERIC_READ,
	//		nullptr, IID_PPV_ARGS(&vertexUploadBuffer)
	//	) >> chk;
	//}
	//// copy array of vertex data to upload buffer 
	//{
	//	Vertex* mappedVertexData = nullptr;
	//	vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData)) >> chk;
	//	rn::copy(vertexData, mappedVertexData);
	//	vertexUploadBuffer->Unmap(0, nullptr);
	//}
	//ResetCommandList();
	//// copy upload buffer to vertex buffer 
	//commandList->CopyResource(vertexBuffer.Get(), vertexUploadBuffer.Get());
	//// transition vertex buffer to vertex buffer state 
	//{
	//	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//		vertexBuffer.Get(),
	//		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	//	commandList->ResourceBarrier(1, &barrier);
	//}
	//CloseAndExecuteCommandList();
	//SignalAndWait();


	//// Create the vertex buffer view. 
	//vertexBufferView = {
	//	.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
	//	.SizeInBytes = nVertices * sizeof(Vertex),
	//	.StrideInBytes = sizeof(Vertex),
	//};
}

void PrimitiveModel::CreateIndexBuffer()
{
	//// create index buffer  
	//{
	//	// the content data  

	//	// set the index count  
	//	nIndices = (UINT)std::size(indexData);
	//	// create committed resource for index buffer  
	//	{
	//		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
	//		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexData));
	//		device->CreateCommittedResource(
	//			&heapProps,
	//			D3D12_HEAP_FLAG_NONE,
	//			&resourceDesc,
	//			D3D12_RESOURCE_STATE_COPY_DEST,
	//			nullptr, IID_PPV_ARGS(&indexBuffer)
	//		) >> chk;
	//	}
	//	// create committed resource for cpu upload of index data  

	//	{
	//		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
	//		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexData));
	//		device->CreateCommittedResource(
	//			&heapProps,
	//			D3D12_HEAP_FLAG_NONE,
	//			&resourceDesc,
	//			D3D12_RESOURCE_STATE_GENERIC_READ,
	//			nullptr, IID_PPV_ARGS(&indexUploadBuffer)
	//		) >> chk;
	//	}
	//	// copy array of index data to upload buffer  
	//	{
	//		WORD* mappedIndexData = nullptr;
	//		indexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData)) >> chk;
	//		rn::copy(indexData, mappedIndexData);
	//		indexUploadBuffer->Unmap(0, nullptr);
	//	}
	//	// reset command list and allocator   
	//	ResetCommandList();
	//	// copy upload buffer to index buffer  
	//	commandList->CopyResource(indexBuffer.Get(), indexUploadBuffer.Get());
	//	// transition index buffer to index buffer state 
	//	{
	//		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//			indexBuffer.Get(),
	//			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
	//		commandList->ResourceBarrier(1, &barrier);
	//	}
	//	CloseAndExecuteCommandList();
	//	SignalAndWait();
	//}

	//// Create the index buffer view.  
	//indexBufferView = {
	//	.BufferLocation = indexBuffer->GetGPUVirtualAddress(),
	//	.SizeInBytes = nIndices * sizeof(WORD),
	//	.Format = DXGI_FORMAT_R16_UINT,
	//};
}

void PrimitiveModel::CreateTexture()
{
	//// create the cube texture 
	//{
	//	// load image data from disk 
	//	DirectX::ScratchImage image;
	//	DirectX::LoadFromWICFile(L"cube_face.jpg", DirectX::WIC_FLAGS_NONE, nullptr, image) >> chk;

	//	// generate mip chain 
	//	DirectX::ScratchImage mipChain;
	//	DirectX::GenerateMipMaps(*image.GetImages(), DirectX::TEX_FILTER_BOX, 0, mipChain) >> chk;

	//	// create texture resource 
	//	{
	//		const auto& chainBase = *mipChain.GetImages();
	//		const D3D12_RESOURCE_DESC texDesc{
	//			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
	//			.Width = (UINT)chainBase.width,
	//			.Height = (UINT)chainBase.height,
	//			.DepthOrArraySize = 1,
	//			.MipLevels = (UINT16)mipChain.GetImageCount(),
	//			.Format = chainBase.format,
	//			.SampleDesc = {.Count = 1 },
	//			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
	//			.Flags = D3D12_RESOURCE_FLAG_NONE,
	//		};
	//		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
	//		device->CreateCommittedResource(
	//			&heapProps,
	//			D3D12_HEAP_FLAG_NONE,
	//			&texDesc,
	//			D3D12_RESOURCE_STATE_COPY_DEST,
	//			nullptr,
	//			IID_PPV_ARGS(&cubeFaceTexture)
	//		) >> chk;
	//	}

	//	// collect subresource data 
	//	const auto subresourceData = vi::iota(0, (int)mipChain.GetImageCount()) |
	//		vi::transform([&](int i) {
	//		const auto img = mipChain.GetImage(i, 0, 0);
	//		return D3D12_SUBRESOURCE_DATA{
	//			.pData = img->pixels,
	//			.RowPitch = (LONG_PTR)img->rowPitch,
	//			.SlicePitch = (LONG_PTR)img->slicePitch,
	//		};
	//			}) |
	//		rn::to<std::vector>();

	//			// create the intermediate upload buffer 

	//			{
	//				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
	//				const auto uploadBufferSize = GetRequiredIntermediateSize(
	//					cubeFaceTexture.Get(), 0, (UINT)subresourceData.size()
	//				);
	//				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	//				device->CreateCommittedResource(
	//					&heapProps,
	//					D3D12_HEAP_FLAG_NONE,
	//					&resourceDesc,
	//					D3D12_RESOURCE_STATE_GENERIC_READ,
	//					nullptr,
	//					IID_PPV_ARGS(&uploadBuffer)
	//				) >> chk;
	//			}

	//			ResetCommandList();
	//			// write commands to copy data to upload texture (copying each subresource) 
	//			UpdateSubresources(
	//				commandList.Get(),
	//				cubeFaceTexture.Get(),
	//				uploadBuffer.Get(),
	//				0, 0,
	//				(UINT)subresourceData.size(),
	//				subresourceData.data()
	//			);
	//			// write command to transition texture to texture state  
	//			{
	//				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//					cubeFaceTexture.Get(),
	//					D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	//				commandList->ResourceBarrier(1, &barrier);
	//			}
	//			// close and execute command list						
	//			CloseAndExecuteCommandList();
	//			SignalAndWait();
	//}
}

void PrimitiveModel::SetViewMatrix()
{
	//// set view projection matrix
	//{
	//	// setup view (camera) matrix
	//	const auto eyePosition = XMVectorSet(0, 0, -6, 1);
	//	const auto focusPoint = XMVectorSet(0, 0, 0, 1);
	//	const auto upDirection = XMVectorSet(0, 1, 0, 0);
	//	const auto view = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
	//	// setup perspective projection matrix
	//	const auto aspectRatio = float(width) / float(height);
	//	const auto projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.f), aspectRatio, 0.1f, 100.0f);
	//	// combine matrices
	//	viewProjection = view * projection;
	//}
}

void PrimitiveModel::CreateDescriptorHeaps()
{
	//// create handle to the srv heap and to the only view in the heap 
	//CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());

	//// create the descriptor in the heap 
	//{
	//	const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
	//		.Format = cubeFaceTexture->GetDesc().Format,
	//		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
	//		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
	//		.Texture2D{.MipLevels = cubeFaceTexture->GetDesc().MipLevels },
	//	};
	//	device->CreateShaderResourceView(cubeFaceTexture.Get(), &srvDesc, srvHandle);
	//}
}

void PrimitiveModel::CreateRootSignature()
{
	//// create root signature 

	//{
	//	// define root signature with a matrix of 16 32-bit floats used by the vertex shader (mvp matrix) 
	//	CD3DX12_ROOT_PARAMETER rootParameters[2]{};
	//	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	//	{
	//		const CD3DX12_DESCRIPTOR_RANGE descRange{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 };
	//		rootParameters[1].InitAsDescriptorTable(1, &descRange);
	//	}
	//	// Allow input layout and vertex shader and deny unnecessary access to certain pipeline stages.
	//	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
	//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
	//		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
	//		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
	//		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
	//		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
	//		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	//	// define static sampler 
	//	const CD3DX12_STATIC_SAMPLER_DESC staticSampler{ 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR };
	//	// define root signature with transformation matrix
	//	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//	rootSignatureDesc.Init(
	//		(UINT)std::size(rootParameters), rootParameters,
	//		1, &staticSampler,
	//		rootSignatureFlags);
	//	// serialize root signature 

	//	ComPtr<ID3DBlob> errorBlob;
	//	if (const auto hr = D3D12SerializeRootSignature(
	//		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
	//		&signatureBlob, &errorBlob); FAILED(hr))
	//	{
	//		if (errorBlob)
	//		{
	//			auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
	//			chilog.error(utl::ToWide(errorBufferPtr)).no_trace();
	//		}
	//		hr >> chk;
	//	}
	//	// Create the root signature. 
	//	device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
	//		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)) >> chk;
	//}
}

void PrimitiveModel::CreatePipelineState()
{
	//// creating pipeline state object 
	//{
	//	// static declaration of pso stream structure 
	//	struct PipelineStateStream
	//	{
	//		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
	//		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
	//		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
	//		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
	//		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
	//		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
	//		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	//	} pipelineStateStream;

	//	// define the Vertex input layout 
	//	const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//	};

	//	// Load the vertex shader. 
	//	ComPtr<ID3DBlob> vertexShaderBlob;
	//	D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob) >> chk;

	//	// Load the pixel shader. 
	//	ComPtr<ID3DBlob> pixelShaderBlob;
	//	D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob) >> chk;

	//	// filling pso structure 
	//	pipelineStateStream.RootSignature = rootSignature.Get();
	//	pipelineStateStream.InputLayout = { inputLayout, (UINT)std::size(inputLayout) };
	//	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
	//	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
	//	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//	pipelineStateStream.RTVFormats = {
	//		.RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
	//		.NumRenderTargets = 1,
	//	};

	//	// building the pipeline state object 
	//	const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
	//		sizeof(PipelineStateStream), &pipelineStateStream
	//	};
	//	device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)) >> chk;
	//}
}
