#pragma once
#include "GEngineObject.h"

class GEngineGameObject : public GEngineObject
{
public:
	GEngineGameObject(std::string objectName = "GObject");
	virtual ~GEngineGameObject() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float dt) {}
	virtual void OnImGuiUpdate() {}	
};

