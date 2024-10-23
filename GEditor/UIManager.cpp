#include "UIManager.h"
#include "UIMainMenu.h"
#include "GEngineContext.h"
#include "UIPanels.h"

void UIManager::SetUIOverlay()
{
	GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->PushOverlay(new UIMainMenu());
	GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->PushOverlay(new UIPanels());
}
