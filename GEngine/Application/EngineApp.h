#pragma once
#include <GEngine/src/win/IWindow.h>
#include <GEngine/Graphics/Graphics.h>


namespace Genesis
{
	class EngineApp
	{
	public:
		EngineApp();
		virtual ~EngineApp() = default;
		int Run(chil::win::IWindow& window);
		virtual void Init() = 0;
		void virtual HandleInput(float deltaTime) = 0;
		void virtual Update(float deltaTime) = 0;
		virtual void Shutdown() = 0;

	private:
		
		// Private variables
	private:
		Graphics m_Graphics;
	};
}

// To be defined in CLIENT
Genesis::EngineApp* CreateEngineApp();