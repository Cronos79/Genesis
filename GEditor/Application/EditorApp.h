// Copyright
// 
#pragma once
#include "GEngine/Application/EngineApp.h"

namespace Genesis
{
	class EditorApp : public EngineApp
	{
	public:
		EditorApp();
		~EditorApp();

		void Init() override;
		void  HandleInput(float deltaTime) override;
		void  Update(float deltaTime) override;
		void Shutdown() override;
	};
}

