#include "genginepch.h"
#include "GEngineGameObjectStack.h"

GEngineGameObjectStack::~GEngineGameObjectStack()
{
	/*for (GEngineGameObject* object : m_Objects)
	{
		if (object)
		{
			object->OnDetach();
			delete object;
		}	
	}*/
}

void GEngineGameObjectStack::PushGameObject(GEngineGameObject* object)
{
	m_Objects.emplace(m_Objects.begin() + m_LayerInsertIndex, object);
	m_LayerInsertIndex++;
}

void GEngineGameObjectStack::PushOverlay(GEngineGameObject* overlay)
{
	m_Objects.emplace_back(overlay);
}

void GEngineGameObjectStack::PopGameObject(GEngineGameObject* object)
{
	auto it = std::find(m_Objects.begin(), m_Objects.begin() + m_LayerInsertIndex, object);
	if (it != m_Objects.begin() + m_LayerInsertIndex)
	{
		object->OnDetach();
		m_Objects.erase(it);
		m_LayerInsertIndex--;
	}
}

void GEngineGameObjectStack::PopOverlay(GEngineGameObject* overlay)
{
	auto it = std::find(m_Objects.begin() + m_LayerInsertIndex, m_Objects.end(), overlay);
	if (it != m_Objects.end())
	{
		overlay->OnDetach();
		m_Objects.erase(it);
	}
}

std::vector<GEngineGameObject*> GEngineGameObjectStack::GetGameObjects()
{
	return m_Objects;
}
