#include "EngineApp.h"
#include "imgui.h"

namespace Genesis
{
	

	EngineApp::EngineApp()
	{

	}

	int EngineApp::Run(chil::win::IWindow& window)
	{
		GContext::Get().Initialize(window);
		m_PrimitiveModel.Initialize();
		m_PrimitiveModel2.Initialize();
		Init();
		float deltaTime = 0.0f;
		while (!window.IsClosing())
		{
			GContext::Get().GetGraphics().BeginFrame(deltaTime);
			HandleInput(deltaTime);
			Update(deltaTime);
			m_PrimitiveModel.Draw(0.001f);
			m_PrimitiveModel2.Draw(0.01f);
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::Begin("FPS");   
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
			GContext::Get().GetGraphics().EndFrame(deltaTime);
		}
		m_PrimitiveModel.Shutdown();
		Shutdown();
		return 0;
	}
}