#pragma once
#include "GEngineGameObject.h"
class GEngineImGuiObject : public GEngineGameObject
{
public:
	GEngineImGuiObject(std::string objectName);
	~GEngineImGuiObject() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnImGuiRender() override;

	void Begin();
	void End();

	void SetDarkThemeColors();

	uint32_t GetActiveWidgetID() const;
};

