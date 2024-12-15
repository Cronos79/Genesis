#pragma once
#include <GEngine/src/win/IWindow.h>
#include <GEngine/Graphics/Graphics.h>
#include "../Graphics/PrimitiveModel.h"
#include <GEngine/Core/GContext.h>


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
		PrimitiveModel m_PrimitiveModel;
		PrimitiveModel m_PrimitiveModel2;
	};
}

// To be defined in CLIENT
Genesis::EngineApp* CreateEngineApp();