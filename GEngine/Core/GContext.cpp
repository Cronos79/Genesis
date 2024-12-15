#include "GContext.h"

namespace Genesis
{
	void GContext::Initialize(chil::win::IWindow& window)
	{
		m_graphics = new Graphics();
		m_graphics->Initialize(window);
	}
}