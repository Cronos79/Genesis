#pragma once
#include "GEngineGameObject.h"
class GEngineGameObjectStack
{
public:
	GEngineGameObjectStack() = default;
	~GEngineGameObjectStack();

	void PushGameObject(GEngineGameObject* object);
	void PushOverlay(GEngineGameObject* overlay);
	void PopGameObject(GEngineGameObject* object);
	void PopOverlay(GEngineGameObject* overlay);

	std::vector<GEngineGameObject*> GetGameObjects();

	std::vector<GEngineGameObject*>::iterator begin() { return m_Objects.begin(); }
	std::vector<GEngineGameObject*>::iterator end() { return m_Objects.end(); }
	std::vector<GEngineGameObject*>::reverse_iterator rbegin() { return m_Objects.rbegin(); }
	std::vector<GEngineGameObject*>::reverse_iterator rend() { return m_Objects.rend(); }

	std::vector<GEngineGameObject*>::const_iterator begin() const { return m_Objects.begin(); }
	std::vector<GEngineGameObject*>::const_iterator end()	const { return m_Objects.end(); }
	std::vector<GEngineGameObject*>::const_reverse_iterator rbegin() const { return m_Objects.rbegin(); }
	std::vector<GEngineGameObject*>::const_reverse_iterator rend() const { return m_Objects.rend(); }
private:
	std::vector<GEngineGameObject*> m_Objects;
	unsigned int m_LayerInsertIndex = 0;
};

