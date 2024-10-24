#include "genginepch.h"
#include "GEngineGameObject.h"

GEngineGameObject::GEngineGameObject(std::string objectName)
	: GEngineObject(objectName)
{

}

bool GEngineGameObject::IsFirstRun()
{
	if (m_FirstRun)
	{
		m_FirstRun = false;
		return true;
	}
	return false;
}

