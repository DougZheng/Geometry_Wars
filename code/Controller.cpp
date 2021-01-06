#include"Controller.h"

const int			g_iEnemyNum		  = 5;
const int			g_iMaxEnemyBlood  = 25;
const int			g_iMinEnemySize   = 25;
extern Controller	*g_pCPlayer;
extern Painter		*g_pEnemyArr[g_iEnemyNum][g_iMinEnemySize + g_iMaxEnemyBlood + 1];

Controller::Controller() {

	m_pBitmap = NULL;
}

Controller::Controller(Painter *pBitmap, int iZ, POINT pPos, 
	RECT rBound, POINT pSpeed, int iTotFrame, int iCurFrame,
	int iFrameDelay, int iFrameTrigger, int iAimFrame, 
	int iFrameDeta, Action aAction, BOOL bRound) {

	m_pBitmap = pBitmap;
	m_iZ = iZ;
	SetRect(&m_rPos, pPos.x, pPos.y, pPos.x + pBitmap->GetWidth(), pPos.y + pBitmap->GetHeight());
	m_rBound = rBound;
	m_pSpeed = pSpeed;

	SetTotFrame(iTotFrame);
	m_iCurFrame = iCurFrame;
	m_iFrameDelay = iFrameDelay;
	m_iFrameTrigger = iFrameTrigger;
	SetAimFrame(iAimFrame);
	SetFrameDeta(iFrameDeta);
	m_aAction = aAction;

	m_bRound = bRound;
	m_bDie = FALSE;
	m_iColor = 0;
	m_iBlood = 0;
	m_iLimSpeed = 5;
	m_iDisplay = 0;
	SetColli();
}

Controller::~Controller() { }

void Controller::SetColli() {

	int w = GetWidth(), h = GetHeight(), k = GetType() == PLAYER ? 6 : 12;
	int x1 = m_rPos.left + w / k,
		x2 = m_rPos.right - w / k,
		y1 = m_rPos.top + h / k,
		y2 = m_rPos.bottom - h / k;
	SetRect(&m_rColli, x1, y1, x2, y2);
}

BOOL Controller::TestColli(Controller *pCon) {

	if (pCon->GetDisplay() || GetDisplay()) return FALSE;
	RECT rColli = pCon->GetColli();
	SetColli();
	return m_rColli.left <= rColli.right &&
		rColli.left <= m_rColli.right &&
		m_rColli.top <= rColli.bottom &&
		rColli.top <= m_rColli.bottom;
}

void Controller::SetTotFrame(int iTotFrame) {

	m_iTotFrame = iTotFrame;
	RECT rTmp = GetPos();
	rTmp.right = rTmp.left + (rTmp.right - rTmp.left) / iTotFrame;
	SetPos(rTmp);
}

int Controller::GetLowestDir() {

	if (m_iAimFrame >= m_iCurFrame) {

		return m_iAimFrame - m_iCurFrame <= m_iTotFrame / 2 ? 1 : -1;
	}
	else {

		return m_iAimFrame + m_iTotFrame - m_iCurFrame <= m_iTotFrame / 2 ? 1 : -1;
	}
}

void Controller::UpdateFrame() {

	if (m_iFrameDelay < 0 || m_iCurFrame == m_iAimFrame) return;

	if (--m_iFrameTrigger <= 0) {

		m_iFrameTrigger = m_iFrameDelay;
		m_iCurFrame = (m_iCurFrame + m_iFrameDeta + m_iTotFrame) % m_iTotFrame;
	}
}

void Controller::SetBlood(int iBlood) {

	m_iBlood = iBlood + g_iMinEnemySize;
	SetDisplay(m_iBlood);
}

int Controller::GetBlood() {

	return m_iBlood - g_iMinEnemySize;
}

void Controller::UpdateBlood() {

	int iW1 = GetWidth(), iH1 = GetHeight();
	--m_iBlood; m_bDie = FALSE;
	m_pBitmap = g_pEnemyArr[m_iColor][m_iBlood];
	int iW2 = GetWidth(), iH2 = GetHeight();
	int dW = iW1 - iW2, dH = iH1 - iH2;
	if (dW != 1 && dH != 1) dW /= 2, dH /= 2, m_rPos = { m_rPos.left + dW, m_rPos.top + dH, m_rPos.left + iW2, m_rPos.top + iH2 };
	else if(m_iBlood & 1) m_rPos = { m_rPos.left + dW, m_rPos.top + dH, m_rPos.left + iW2, m_rPos.top + iH2 };
	SetColli();
}

ConAction Controller::Update() {

	if (m_bDie) return KILL;

	UpdateFrame();

	if (GetType() == ENEMY && m_iDisplay > 0) { // Display

		if (m_iDisplay != m_iBlood && rand() % 2) return NONE;
		--m_iDisplay;
		m_pBitmap = g_pEnemyArr[m_iColor][m_iBlood - m_iDisplay];
		if(m_iDisplay & 1) OffsetRect(&m_rPos, -1, -1);
		return NONE;
	}

	POINT pNewPos = { m_rPos.left + m_pSpeed.x, m_rPos.top + m_pSpeed.y };
	POINT pNewSpeed = m_pSpeed;

	if (m_aAction == BOUNCE) {

		if (pNewPos.x < m_rBound.left) {

			pNewPos.x = m_rBound.left;
			pNewSpeed.x = -pNewSpeed.x;
		}
		else if (pNewPos.x + (m_rPos.right - m_rPos.left) > m_rBound.right) {

			pNewPos.x = m_rBound.right - (m_rPos.right - m_rPos.left);
			pNewSpeed.x = -pNewSpeed.x;
		}
		if (pNewPos.y < m_rBound.top) {

			pNewPos.y = m_rBound.top;
			pNewSpeed.y = -pNewSpeed.y;
		}
		else if (pNewPos.y + (m_rPos.bottom - m_rPos.top) > m_rBound.bottom) {

			pNewPos.y = m_rBound.bottom - (m_rPos.bottom - m_rPos.top);
			pNewSpeed.y = -pNewSpeed.y;
		}
	}
	if (m_aAction == DIE) {

		if (m_bRound) { // Boundary with round shape

			int iCenR = (m_rBound.right - m_rBound.left) / 2;
			int iCenX = m_rBound.left + iCenR;
			int iCenY = m_rBound.top + iCenR;
			if ((iCenX - pNewPos.x) * (iCenX - pNewPos.x) + (iCenY - pNewPos.y) * (iCenY - pNewPos.y) > iCenR * iCenR) return KILL;
		}
		else { // Default, with rectangle shape

			if (pNewPos.x < m_rBound.left || pNewPos.x + (m_rPos.right - m_rPos.left) > m_rBound.right ||
				pNewPos.y < m_rBound.top || pNewPos.y + (m_rPos.bottom - m_rPos.top) > m_rBound.bottom) return KILL;
		}
	}

	SetPos(pNewPos);

	if (GetType() == ENEMY) {

		RECT rPos = g_pCPlayer->GetPos();
		int iXPlayer = rPos.left + (rPos.right - rPos.left) / 2;
		int iYPlayer = rPos.top + (rPos.bottom - rPos.top) / 2;
		int iXEnemy = pNewPos.x + (m_rPos.right - m_rPos.left) / 2;
		int iYEnemy = pNewPos.y + (m_rPos.bottom - m_rPos.top) / 2;
		int iR = 250;

		if (m_iColor == PURPLE) { // Purple, surround mode

			if ((iXEnemy - iXPlayer) * (iXEnemy - iXPlayer) + (iYEnemy - iYPlayer) * (iYEnemy - iYPlayer) > iR * iR) {

				if (rand() % 3 == 0) {

					if (iXEnemy > iXPlayer) pNewSpeed.x = max(pNewSpeed.x - 1, -m_iLimSpeed);
					else if (iXEnemy < iXPlayer) pNewSpeed.x = min(pNewSpeed.x + 1, m_iLimSpeed);
				}
				if (rand() % 3 == 0) {

					if (iYEnemy > iYPlayer) pNewSpeed.y = max(pNewSpeed.y - 1, -m_iLimSpeed);
					else if (iYEnemy < iYPlayer) pNewSpeed.y = min(pNewSpeed.y + 1, m_iLimSpeed);
				}
			}
			else {

				int r1 = rand() % 100, r2 = rand() % 100;
				if (r1 == 0) pNewSpeed.x = max(pNewSpeed.x - 3, -m_iLimSpeed);
				else if (r1 == 100) pNewSpeed.x = min(pNewSpeed.x + 3, m_iLimSpeed);
				if (r2 == 0) pNewSpeed.y = max(pNewSpeed.y - 3, -m_iLimSpeed);
				else if (r2 == 100) pNewSpeed.y = min(pNewSpeed.y + 3, m_iLimSpeed);
			}
		}
		else if (m_iColor == ORANGE){ // Orange, track within R distance

			if ((iXEnemy - iXPlayer) * (iXEnemy - iXPlayer) + (iYEnemy - iYPlayer) * (iYEnemy - iYPlayer) < iR * iR) {

				if (rand() % 4 == 0) {

					if (iXEnemy > iXPlayer) pNewSpeed.x = max(pNewSpeed.x - 1, -m_iLimSpeed);
					else if (iXEnemy < iXPlayer) pNewSpeed.x = min(pNewSpeed.x + 1, m_iLimSpeed);
				}
				if (rand() % 4 == 0) {

					if (iYEnemy > iYPlayer) pNewSpeed.y = max(pNewSpeed.y - 1, -m_iLimSpeed);
					else if (iYEnemy < iYPlayer) pNewSpeed.y = min(pNewSpeed.y + 1, m_iLimSpeed);
				}
			}
			else {

				int r1 = rand() % 100, r2 = rand() % 100;
				if (r1 == 0) pNewSpeed.x = max(pNewSpeed.x - 3, -m_iLimSpeed);
				else if (r1 == 100) pNewSpeed.x = min(pNewSpeed.x + 3, m_iLimSpeed);
				if (r2 == 0) pNewSpeed.y = max(pNewSpeed.y - 3, -m_iLimSpeed);
				else if (r2 == 100) pNewSpeed.y = min(pNewSpeed.y + 3, m_iLimSpeed);
			}
		}
		else if (m_iColor == RED) { // Red, always track

			if (rand() % 5 == 0) {

				if (iXEnemy > iXPlayer) pNewSpeed.x = max(pNewSpeed.x - 1, -m_iLimSpeed);
				else if (iXEnemy < iXPlayer) pNewSpeed.x = min(pNewSpeed.x + 1, m_iLimSpeed);
			}
			if (rand() % 5 == 0) {

				if (iYEnemy > iYPlayer) pNewSpeed.y = max(pNewSpeed.y - 1, -m_iLimSpeed);
				else if (iYEnemy < iYPlayer) pNewSpeed.y = min(pNewSpeed.y + 1, m_iLimSpeed);
			}
		}
		else if (m_iColor == GREEN) { // Green, away and rush back

			m_iLimSpeed = 3;
			if ((iXEnemy - iXPlayer) * (iXEnemy - iXPlayer) + (iYEnemy - iYPlayer) * (iYEnemy - iYPlayer) < iR * iR) {

				if (rand() % 10 == 0) {

					if (iXEnemy < iXPlayer) pNewSpeed.x = max(pNewSpeed.x - 1, -m_iLimSpeed);
					else if (iXEnemy > iXPlayer) pNewSpeed.x = min(pNewSpeed.x + 1, m_iLimSpeed);
				}
				if (rand() % 10 == 0) {

					if (iYEnemy < iYPlayer) pNewSpeed.y = max(pNewSpeed.y - 1, -m_iLimSpeed);
					else if (iYEnemy > iYPlayer) pNewSpeed.y = min(pNewSpeed.y + 1, m_iLimSpeed);
				}
			}
			else if (rand() % 80 == 0) {

				m_iLimSpeed = 8;
				int iDX = iXPlayer - iXEnemy, iDY = iYPlayer - iYEnemy;
				if ((iDX > 0 ? iDX : -iDX) > (iDY > 0 ? iDY : -iDY)) {

					if (iDX > 0) pNewSpeed.x = m_iLimSpeed, pNewSpeed.y = m_iLimSpeed * iDY / iDX;
					else if (iDX < 0) pNewSpeed.x = -m_iLimSpeed, pNewSpeed.y = m_iLimSpeed * iDY / -iDX;
				}
				else {

					if (iDY > 0) pNewSpeed.y = m_iLimSpeed, pNewSpeed.x = m_iLimSpeed * iDX / iDY;
					else if (iDY < 0) pNewSpeed.y = -m_iLimSpeed, pNewSpeed.x = m_iLimSpeed * iDX / -iDY;
				}
			}
		}
		else { // Blue, fixed speed

			if (!pNewSpeed.x && !pNewSpeed.y) {
				
				int iXDir = rand() % 2, iYDir = rand() % 2;
				iXDir = iXDir ? 1 : -1, iYDir = iYDir ? 1 : -1;
				pNewSpeed.x = m_iLimSpeed * iXDir;
				pNewSpeed.y = m_iLimSpeed * iYDir;
			}
		}
	}

	SetSpeed(pNewSpeed);

	return NONE;
}

void Controller::Draw(HDC hDC) {

	if (m_pBitmap != NULL) {

		m_pBitmap->DrawPart(hDC, m_rPos.left, m_rPos.top, m_iCurFrame * GetWidth(), 
			0, GetWidth(), GetHeight());
	}
}