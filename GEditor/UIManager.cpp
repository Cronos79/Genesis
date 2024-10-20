#include "UIManager.h"
#include "UIMainMenu.h"
#include "GEngineContext.h"

void UIManager::SetUIOverlay()
{
	GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->PushOverlay(new UIMainMenu());
}
