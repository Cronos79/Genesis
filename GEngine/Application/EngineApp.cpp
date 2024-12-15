#include "EngineApp.h"

namespace Genesis
{
	

	EngineApp::EngineApp()
	{

	}

	int EngineApp::Run(chil::win::IWindow& window)
	{
		m_Graphics.Initialize(window);
		Init();
		float deltaTime = 0.0f;
		while (!window.IsClosing())
		{
			m_Graphics.BeginFrame(deltaTime);
			HandleInput(deltaTime);
			Update(deltaTime);
			m_Graphics.TestLoop();
			m_Graphics.EndFrame(deltaTime);
		}

		Shutdown();
		return 0;
	}
}