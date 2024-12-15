#pragma once
#include "ModelBase.h"

//
using namespace DirectX;


namespace Genesis
{
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

	class PrimitiveModel : public ModelBase
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void Draw(float deltaTime) override;

	protected:
		void CreateVertexBuffer() override;
		void CreateIndexBuffer() override;
		void CreateTexture() override;
		void SetViewMatrix() override;
		void CreateDescriptorHeaps() override;
		void CreateRootSignature() override;
		void CreatePipelineState() override;
	};
}
