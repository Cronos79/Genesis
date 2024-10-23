#pragma once
#include "GEngineGameObject.h"
#include <wrl.h>
#include <d3d12.h>

struct Vertex
{
	float x, y;
};

class TestTriangle : public GEngineGameObject
{
public:
	void OnUpdate(float dt) override;

private:
	void Start();

private:
	bool m_FirstRun = true;
	
	size_t m_NumVerticies;

	Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer; 
	Microsoft::WRL::ComPtr<ID3D12Resource2> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
};

