#include "genginepch.h"
#include "GEngineImGuiObject.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "imgui_internal.h"
#include "GEngineContext.h"

GEngineImGuiObject::GEngineImGuiObject(std::string objectName)
	: GEngineGameObject(objectName)
{

}

uint32_t GEngineImGuiObject::GetActiveWidgetID() const
{
	return GImGui->ActiveId;
}
