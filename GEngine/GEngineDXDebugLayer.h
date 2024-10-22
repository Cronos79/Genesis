#pragma once
#include "WinInclude.h"
#include <wrl.h>

class GEngineDXDebugLayer
{
public:
	GEngineDXDebugLayer();
	GEngineDXDebugLayer(const GEngineDXDebugLayer&) = delete;
	GEngineDXDebugLayer& operator=(const GEngineDXDebugLayer&) = delete;

	inline static GEngineDXDebugLayer& Get()
	{
		static GEngineDXDebugLayer instance;
		return instance;
	}
private:
	~GEngineDXDebugLayer() = default;

public:
	bool Init();
	void Shutdown();

private:
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug6> m_d3d12Debug;
	Microsoft::WRL::ComPtr<IDXGIDebug1> m_dxgiDebug;
#endif
};

