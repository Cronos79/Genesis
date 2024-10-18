#pragma once
#include "genginepch.h"

class GEngineObject
{
public:
	GEngineObject(std::string objectName);
	virtual ~GEngineObject() = default;

	const std::string& GetName() const { return m_ObjectName; }
private:
	std::string m_ObjectName = "GObject";
};

