#include"GameEngine.h"
#include<vector>
#include<cstdlib>

typedef HWND(WINAPI *PROCGETCONSOLEWINDOW)();
extern GameEngine	*g_pGame;
const int			g_iEnemyNum = 5;
extern int			g_iCurEnemyNum;
extern int			g_iLimEnemyNum;
extern int			g_iCurKillNum;
extern int			g_iTotKillNum;
extern BOOL			g_pGotBuff;
extern POINT		g_rCreateBuffPos;
extern int			g_iBloodLosing;
extern int			g_iScore;
extern Painter		*g_pEffect[g_iEnemyNum + 1];
extern Painter		*g_pBuff;
extern Controller	*g_pCPlayer;

GameEngine::GameEngine(HWND hWindow, int iWidth, int iHeight) {

	m_hWindow = hWindow;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iFrameDelay = 20;
	m_bSleep = FALSE;
}

GameEngine::~GameEngine() { }

// Check collision between pC and others
BOOL GameEngine::CheckCollision(Controller *pC) {

	for (auto it : m_vController) {

		if (pC == it || it->GetDie()) continue;
		if (pC->TestColli(it)) return Collision(it, pC);
	}
	return FALSE;
}

// Add a sprite pC
void GameEngine::AddController(Controller *pC) {

	if (pC != NULL) {

		std::vector<Controller *>::iterator it;
		for (it = m_vController.begin(); it != m_vController.end(); ++it) {

			if (pC->GetZ() < (*it)->GetZ()) {

				m_vController.insert(it, pC);
				return;
			}
		}
		m_vController.push_back(pC);
	}
}

// Draw in Z order
void GameEngine::DrawController(HDC hDC) {

	std::vector<Controller *>::iterator it1, it2;
	it1 = m_vEffect.begin(), it2 = m_vController.begin();
	while (it1 != m_vEffect.end() && it2 != m_vController.end()) {

		if ((*it1)->GetZ() <= (*it2)->GetZ()) (*it1)->Draw(hDC), ++it1;
		else (*it2)->Draw(hDC), ++it2;
	}
	while (it1 != m_vEffect.end()) (*it1)->Draw(hDC), ++it1;
	while (it2 != m_vController.end()) (*it2)->Draw(hDC), ++it2;
}

// Update sprite and effect for repainting
void GameEngine::UpdateController() {

	RECT rOldPos;
	ConAction caAction;
	std::vector<Controller *>::iterator it;
	for (it = m_vEffect.begin(); it != m_vEffect.end(); ) {

		caAction = (*it)->Update();
		if (caAction == KILL) {

			delete (*it);
			it = m_vEffect.erase(it);
			continue;
		}
		++it;
	}
	for (it = m_vController.begin(); it != m_vController.end(); ) {

		rOldPos = (*it)->GetPos();
		caAction = (*it)->Update();
		if (caAction == KILL) {

			if ((*it)->GetType() == ENEMY) {

				--g_iCurEnemyNum, ++g_iCurKillNum;
				int iW = rOldPos.right - rOldPos.left, iH = rOldPos.bottom - rOldPos.top;
				g_rCreateBuffPos = { rOldPos.left + iW / 2, rOldPos.top + iH / 2 };
			}
			delete (*it);
			it = m_vController.erase(it);
			continue;
		}
		if (CheckCollision(*it)) (*it)->SetPos(rOldPos);
		++it;
	}
}

// Clear two controllers
void GameEngine::ClearController() {

	for (auto it : m_vEffect) delete it;
	for (auto it : m_vController) delete it;
	m_vEffect.clear();
	m_vController.clear();
}

// Clear Screen, all but player die
void GameEngine::SetEnemyDie() {

	for (auto it : m_vController) {

		if (it->GetType() != PLAYER) {

			g_pGame->CreateEffect(it, it->GetColor(), 125 + it->GetBlood() * 4, 30);
			it->SetDie(TRUE);
		}
	}
}

// Add an effect, pC
void GameEngine::AddEffect(Controller *pC) {

	if (pC != NULL) {

		m_vEffect.push_back(pC);
	}
}

// Create special effect, in pBomb, with iColor, within iRange, with iSize
void GameEngine::CreateEffect(Controller *pBomb, int iColor, int iRange, int iSize) {

	RECT rBound = pBomb->GetPos();
	int w = rBound.right - rBound.left;
	int h = rBound.bottom - rBound.top;
	int dw = g_pEffect[0]->GetWidth();
	int dh = g_pEffect[0]->GetHeight();
	POINT pPos = { rBound.left + w / 2 - dw / 2, rBound.top + h / 2 - dh / 2};
	RECT rRange = { pPos.x - iRange, pPos.y - iRange, pPos.x + iRange, pPos.y + iRange };

	// Randomly create small particle
	for (int i = 0; i < iSize; ++i) {

		int fx = rand() % 2, fy = rand() % 2;
		fx = fx ? 1 : -1, fy = fy ? 1 : -1;
		int dx = rand() % 4 + 1, dy = rand() % 4 + 1;
		int vx = (rand() % 5 + 2) * dx * fx;
		int vy = (rand() % 5 + 2) * dy * fy;
		int x = rand() % w + pPos.x - w / 2, y = rand() % h + pPos.y - h / 2;
		Controller *pTmp = new Controller(g_pEffect[iColor], 0, pPos, rRange,
			POINT{ vx, vy }, 1, 0, 0, 0, 0, 1, DIE, TRUE);
		g_pGame->AddEffect(pTmp);
	}
}

// Coliision between pC1 and pC2
BOOL Collision(Controller *pC1, Controller *pC2) {

	ConType cType1 = (ConType)pC1->GetType(), cType2 = (ConType)pC2->GetType();

	// Player and buff
	if (cType1 == PLAYER && cType2 == BUFF || cType1 == BUFF && cType2 == PLAYER) {

		g_pGotBuff = TRUE;
		Controller *pBomb = cType1 == BUFF ? pC1 : pC2;
		g_pGame->CreateEffect(pBomb, BLUE, 75, 10);
		g_pGame->CreateEffect(pBomb, RED, 75, 10);
		if (cType1 == BUFF) pC1->SetDie(TRUE);
		else pC2->SetDie(TRUE);
		return FALSE;
	}

	// Enemy and bullet
	else if (cType1 == ENEMY && cType2 == BULLET || cType1 == BULLET && cType2 == ENEMY) {

		Controller *pBomb = cType1 == ENEMY ? pC1 : pC2;
		g_pGame->CreateEffect(pBomb, pBomb->GetColor(), 125, 30);
		pC1->SetDie(TRUE), pC2->SetDie(TRUE);
		if (int b = pBomb->GetBlood()) {

			pBomb->UpdateBlood();
			g_iScore += 5;
		}
		else g_iScore += 10;
		return TRUE;
	}

	// Player and enemy
	else if (cType1 == PLAYER && cType2 == ENEMY || cType1 == ENEMY && cType2 == PLAYER) {

		Controller *pBomb = cType1 == PLAYER ? pC1 : pC2;
		Controller *pBomb2 = cType1 == ENEMY ? pC1 : pC2;
		g_pGame->CreateEffect(pBomb, pBomb->GetColor(), 220, 100);
		g_pGame->CreateEffect(pBomb2, pBomb2->GetColor(), 125 + pBomb2->GetBlood() * 4, 30);
		if (cType1 == ENEMY) pC1->SetDie(TRUE);
		else pC2->SetDie(TRUE);
		++g_iBloodLosing;
		g_iScore += pBomb2->GetBlood() * 5 + 10;
		return FALSE;
	}

	// Otherwise
	return FALSE;
}

int main() {

	// Initialize
	HMODULE hKernel32 = GetModuleHandle("kernel32");
	PROCGETCONSOLEWINDOW GetConsoleWindow;
	GetConsoleWindow = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32, "GetConsoleWindow");
	HWND hWin = GetConsoleWindow();
	
	unsigned iTickTrigger = 0;
	unsigned iTickCount;
	if (GameInitialize(hWin)) {

		GameStart(hWin);
		while (TRUE) {

			iTickCount = GetTickCount();
			if (iTickCount > iTickTrigger) {

				iTickTrigger = iTickCount + g_pGame->GetFrameDelay();
				HandleKeys();
				GameCycle();
			}
		}
		GameEnd();
	}
	return 0;
}