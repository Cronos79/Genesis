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

	GEngineContext::GetInstance().GetGFX()->GetCmdList()->DrawInstanced(m_NumVerticies, 1, 0, 0);
}

//struct CD3DX12_ROOT_SIGNATURE_DESC : public D3D12_ROOT_SIGNATURE_DESC {
//	CD3DX12_ROOT_SIGNATURE_DESC();
//	explicit CD3DX12_ROOT_SIGNATURE_DESC(const D3D12_ROOT_SIGNATURE_DESC& o);
//	CD3DX12_ROOT_SIGNATURE_DESC(UINT numParameters, const D3D12_ROOT_PARAMETER* _pParameters, UINT numStaticSamplers = 0, const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
//	//CD3DX12_ROOT_SIGNATURE_DESC();
//	void inline Init(UINT numParameters, const D3D12_ROOT_PARAMETER* _pParameters, UINT numStaticSamplers = 0, const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
//	void static inline Init(D3D12_ROOT_SIGNATURE_DESC& desc, UINT numParameters, const D3D12_ROOT_PARAMETER* _pParameters, UINT numStaticSamplers = 0, const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
//};

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

	// ==== Vertext data ====	
	Vertex verticies[] =
	{
		{ -1.f, -1.f },
		{ 0.f,   1.f },
		{ 1.f, -1.f },
	};
	m_NumVerticies = sizeof(verticies);
	
	D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } // D3D12_APPEND_ALIGNED_ELEMENT == 0? 
	};

	// ==== Upload and vertex buffer ====
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
	 
	GEngineContext::GetInstance().GetGFX()->GetDevice()->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(m_UploadBuffer.GetAddressOf()));
	GEngineContext::GetInstance().GetGFX()->GetDevice()->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(m_VertexBuffer.GetAddressOf()));

	// Copy void* ---> CPU Resource
	void* uploadBufferAddress;
	D3D12_RANGE uploadRange;
	uploadRange.Begin = 0;
	uploadRange.End = 1023;
	m_UploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
	memcpy(uploadBufferAddress, verticies, sizeof(verticies));
	m_UploadBuffer->Unmap(0, &uploadRange);

	// Copy CPU to GPU
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->CopyBufferRegion(*m_VertexBuffer.GetAddressOf(), 0, *m_UploadBuffer.GetAddressOf(), 0, 1024);
	
	// ==== Shaders ====
	GEngineShader rootSignatureShader("RSTestTriangle.cso");
	GEngineShader vertexShader("VSTestTriangle.cso");
	GEngineShader pixelShader("PSTestTriangle.cso");

	// ==== Create root signature ====
	HRESULT hr = GEngineContext::GetInstance().GetGFX()->GetDevice()->CreateRootSignature(0, rootSignatureShader.GetBuffer(), rootSignatureShader.GetSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf()));

	// ==== Pipeline state ====
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gfxPsod{};
	gfxPsod.pRootSignature = *m_RootSignature.GetAddressOf();
	gfxPsod.InputLayout.NumElements = _countof(vertexLayout);
	gfxPsod.InputLayout.pInputElementDescs = vertexLayout;
	gfxPsod.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gfxPsod.VS.pShaderBytecode = vertexShader.GetBuffer();
	gfxPsod.VS.BytecodeLength = vertexShader.GetSize();
	// #TODO: Rasterizer 
	gfxPsod.PS.pShaderBytecode = pixelShader.GetBuffer();
	gfxPsod.PS.BytecodeLength = pixelShader.GetSize();
	// #TODO: Output merger

	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(verticies);
	vbv.StrideInBytes = sizeof(Vertex);
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->IASetVertexBuffers(0, 1, &vbv);
	GEngineContext::GetInstance().GetGFX()->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	BB_INFO("TestTriangle first run");
}
