#pragma once
#include "GEngineGameObject.h"
#include <wrl.h>
#include <d3d12.h>


class TestTriangle : public GEngineGameObject
{
public:
	void OnUpdate(float dt) override;

private:
	void Start();

private:
	bool m_FirstRun = true;	
	size_t m_NumVerticies;
	D3D12_VERTEX_BUFFER_VIEW vbv{};

	Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer; 
	Microsoft::WRL::ComPtr<ID3D12Resource2> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature; 
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_Pso;
};

