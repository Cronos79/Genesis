#pragma once
#include "GEngineGameObject.h"
class GEngineImGuiObject : public GEngineGameObject
{
public:
	GEngineImGuiObject(std::string objectName);
	~GEngineImGuiObject() = default;

	uint32_t GetActiveWidgetID() const;
};

