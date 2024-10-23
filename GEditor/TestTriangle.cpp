#include "TestTriangle.h"
#include "GEngineContext.h"
#include "GEngineLog.h"
#include "GEngineShader.h"

void TestTriangle::OnUpdate(float dt)
{
	if (m_FirstRun)
	{
		m_FirstRun = false;
		Start();
	}	

	// == PSO ==
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->SetPipelineState(*m_Pso.GetAddressOf());
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->SetGraphicsRootSignature(*m_RootSignature.GetAddressOf());
	// == IA ==
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->IASetVertexBuffers(0, 1, &vbv);
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// == RS ==
	D3D12_VIEWPORT vp;
	vp.TopLeftX = vp.TopLeftY = 0;
	vp.Width = GEngineContext::GetInstance().GetGFX()->GetWidth();
	vp.Height = GEngineContext::GetInstance().GetGFX()->GetHeight();
	vp.MinDepth = 1.f;
	vp.MaxDepth = 0.f;
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->RSSetViewports(1, &vp);
	RECT scRect;
	scRect.left = scRect.top = 0;
	scRect.right = GEngineContext::GetInstance().GetGFX()->GetWidth();
	scRect.bottom = GEngineContext::GetInstance().GetGFX()->GetHeight();
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->RSSetScissorRects(1, &scRect);

	// Draw
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->DrawInstanced(m_NumVerticies, 1, 0, 0);
}

void TestTriangle::Start()
{
	D3D12_HEAP_PROPERTIES hpUpload{};
	hpUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
	hpUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hpUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hpUpload.CreationNodeMask = 0;
	hpUpload.VisibleNodeMask = 0;

	D3D12_HEAP_PROPERTIES hpDefault{};
	hpDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
	hpDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hpDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hpDefault.CreationNodeMask = 0;
	hpDefault.VisibleNodeMask = 0;

	// === Vertex Data ===
	struct Vertex
	{
		float x, y;
	};
	Vertex verticies[] =
	{
		// T1
		{ -1.f, -1.f },
		{  0.f,  1.f },
		{  1.f, -1.f },
	};
	m_NumVerticies = (_countof(verticies));
	D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// === Upload & Vertex Buffer ===
	D3D12_RESOURCE_DESC rd{};
	rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rd.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	rd.Width = 1024;
	rd.Height = 1;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = DXGI_FORMAT_UNKNOWN;
	rd.SampleDesc.Count = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	rd.Flags = D3D12_RESOURCE_FLAG_NONE;

	//ComPointer<ID3D12Resource2> uploadBuffer, vertexBuffer;
	GEngineContext::GetInstance().GetGFX()->GetDevice()->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(m_UploadBuffer.GetAddressOf()));
	GEngineContext::GetInstance().GetGFX()->GetDevice()->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(m_VertexBuffer.GetAddressOf()));

	// Copy void* --> CPU Resource
	void* uploadBufferAddress;
	D3D12_RANGE uploadRange;
	uploadRange.Begin = 0;
	uploadRange.End = 1023;
	m_UploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
	memcpy(uploadBufferAddress, verticies, sizeof(verticies));
	m_UploadBuffer->Unmap(0, &uploadRange);

	// Copy CPU Resource --> GPU Resource
	//auto* cmdList = DXContext::Get().InitCommandList();
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->CopyBufferRegion(*m_VertexBuffer.GetAddressOf(), 0, *m_UploadBuffer.GetAddressOf(), 0, 1024);
	GEngineContext::GetInstance().GetGFX()->ExecuteCommandList();
	GEngineContext::GetInstance().GetGFX()->InitCommandList();

	// === Shaders ===
	GEngineShader rootSignatureShader("RSTestTriangle.cso");
	GEngineShader vertexShader("VSTestTriangle.cso");
	GEngineShader pixelShader("PSTestTriangle.cso");

	// === Create root signature ===
	//ComPointer<ID3D12RootSignature> rootSignature;
	HRESULT hr = GEngineContext::GetInstance().GetGFX()->GetDevice()->CreateRootSignature(0, rootSignatureShader.GetBuffer(), rootSignatureShader.GetSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf()));

	// === Pipeline state ===
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gfxPsod{};
	gfxPsod.pRootSignature = *m_RootSignature.GetAddressOf();
	gfxPsod.InputLayout.NumElements = _countof(vertexLayout);
	gfxPsod.InputLayout.pInputElementDescs = vertexLayout;
	gfxPsod.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gfxPsod.VS.BytecodeLength = vertexShader.GetSize();
	gfxPsod.VS.pShaderBytecode = vertexShader.GetBuffer();
	gfxPsod.PS.BytecodeLength = pixelShader.GetSize();
	gfxPsod.PS.pShaderBytecode = pixelShader.GetBuffer();
	gfxPsod.DS.BytecodeLength = 0;
	gfxPsod.DS.pShaderBytecode = nullptr;
	gfxPsod.HS.BytecodeLength = 0;
	gfxPsod.HS.pShaderBytecode = nullptr;
	gfxPsod.GS.BytecodeLength = 0;
	gfxPsod.GS.pShaderBytecode = nullptr;
	gfxPsod.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gfxPsod.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gfxPsod.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gfxPsod.RasterizerState.FrontCounterClockwise = FALSE;
	gfxPsod.RasterizerState.DepthBias = 0;
	gfxPsod.RasterizerState.DepthBiasClamp = .0f;
	gfxPsod.RasterizerState.SlopeScaledDepthBias = .0f;
	gfxPsod.RasterizerState.DepthClipEnable = FALSE;
	gfxPsod.RasterizerState.MultisampleEnable = FALSE;
	gfxPsod.RasterizerState.AntialiasedLineEnable = FALSE;
	gfxPsod.RasterizerState.ForcedSampleCount = 0;
	gfxPsod.StreamOutput.NumEntries = 0;
	gfxPsod.StreamOutput.NumStrides = 0;
	gfxPsod.StreamOutput.pBufferStrides = nullptr;
	gfxPsod.StreamOutput.pSODeclaration = nullptr;
	gfxPsod.StreamOutput.RasterizedStream = 0;
	gfxPsod.NumRenderTargets = 1;
	gfxPsod.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gfxPsod.DSVFormat = DXGI_FORMAT_UNKNOWN;
	gfxPsod.BlendState.AlphaToCoverageEnable = FALSE;
	gfxPsod.BlendState.IndependentBlendEnable = FALSE;
	gfxPsod.BlendState.RenderTarget[0].BlendEnable = TRUE;
	gfxPsod.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	gfxPsod.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	gfxPsod.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	gfxPsod.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	gfxPsod.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	gfxPsod.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	gfxPsod.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
	gfxPsod.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	gfxPsod.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gfxPsod.DepthStencilState.DepthEnable = FALSE;
	gfxPsod.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gfxPsod.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	gfxPsod.DepthStencilState.StencilEnable = FALSE;
	gfxPsod.DepthStencilState.StencilReadMask = 0;
	gfxPsod.DepthStencilState.StencilWriteMask = 0;
	gfxPsod.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gfxPsod.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsod.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsod.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	gfxPsod.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gfxPsod.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsod.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsod.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	gfxPsod.SampleMask = 0xFFFFFFFF;
	gfxPsod.SampleDesc.Count = 1;
	gfxPsod.SampleDesc.Quality = 0;
	gfxPsod.NodeMask = 0;
	gfxPsod.CachedPSO.CachedBlobSizeInBytes = 0;
	gfxPsod.CachedPSO.pCachedBlob = nullptr;
	gfxPsod.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	//ComPointer<ID3D12PipelineState> pso;
	GEngineContext::GetInstance().GetGFX()->GetDevice()->CreateGraphicsPipelineState(&gfxPsod, IID_PPV_ARGS(m_Pso.GetAddressOf()));

	// === Vertex buffer view ===
	//D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(Vertex) * _countof(verticies);
	vbv.StrideInBytes = sizeof(Vertex);
}
