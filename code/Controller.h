#pragma once
#include"Painter.h"
#include<windows.h>

// Action when outside boundary
enum Action {
	
	BOUNCE,
	DIE	
};

// Action when move to next frame
enum ConAction {

	NONE,
	KILL
};

class Controller {

private:

	Painter			*m_pBitmap;
	int				m_iZ;
	RECT		    m_rPos;
	RECT			m_rBound;
	RECT		    m_rColli;
	POINT			m_pSpeed;
	Action			m_aAction;

	int				m_iTotFrame;
	int				m_iCurFrame;
	int				m_iFrameDelay;
	int				m_iFrameTrigger;
	int				m_iAimFrame;
	int				m_iFrameDeta;
	
	BOOL			m_bRound;
	BOOL			m_bDie;
	int				m_iColor;
	int				m_iBlood;
	int				m_iLimSpeed;
	int				m_iDisplay;

public:

	Controller();
	Controller(Painter *pBitmap, int iZ, POINT pPos, RECT rBound,
		POINT pSpeed = { 0, 0 }, int iTotFrame = 1, int iCurFrame = 0,
		int iFrameDelay = 0, int iFrameTrigger = 0, int iAimFrame = -1, 
		int m_iFrameDeta = 1, Action aAction = BOUNCE, BOOL bRound = FALSE);
	virtual ~Controller(); // The guy is lazy to achieve polymorphic

	// Update Frame
	virtual ConAction Update();
	
	// Draw 
	void Draw(HDC hDC);
	
	// Function for checking collision
	virtual void SetColli();
	BOOL TestColli(Controller *pCon);
	
	// Function for updating frame
	void UpdateFrame();
	int GetLowestDir();

	// Update blood
	void UpdateBlood();
	
	// Get Function
	int GetWidth() { return m_pBitmap->GetWidth() / m_iTotFrame; }
	int GetHeight() { return m_pBitmap->GetHeight(); }
	int GetType() { return m_pBitmap->GetType(); }

	Painter *GetBitmap() { return m_pBitmap; }
	int GetZ() { return m_iZ; }
	RECT GetPos() { return m_rPos; }
	RECT GetBound() { return m_rBound; }
	RECT GetColli() { return m_rColli; }
	POINT GetSpeed() { return m_pSpeed; }
	Action GetAction() { return m_aAction; }

	int GetTotFrame() { return m_iTotFrame; }
	int GetCurFrame() { return m_iCurFrame; }
	int GetFrameDelay() { return m_iFrameDelay; }
	int GetFrameTrigger() { return m_iFrameTrigger; }
	int GetAimFrame() { return m_iAimFrame; }
	int GetFrameDeta() { return m_iFrameDeta; }

	BOOL GetRound() { return m_bRound; }
	BOOL GetDie() { return m_bDie; }
	int GetColor() { return m_iColor; }
	int GetBlood();
	int GetLimSpeed() { return m_iLimSpeed; }
	int GetDisplay() { return m_iDisplay; }

	// Set Function
	void SetBitmap(Painter *pBitmap) { m_pBitmap = pBitmap; }
	void SetZ(int iZ) { m_iZ = iZ; }
	void SetPos(RECT rPos) { m_rPos = rPos; }
	void SetPos(POINT pPos) { OffsetRect(&m_rPos, pPos.x - m_rPos.left, pPos.y - m_rPos.top); }
	void SetSpeed(POINT pSpeed) { m_pSpeed = pSpeed; }
	void SetSpeed(int iX, int iY) { m_pSpeed.x = iX, m_pSpeed.y = iY; }
	void SetAction(Action aAction) { m_aAction = aAction; }

	void SetTotFrame(int iTotFrame);
	void SetCurFrame(int iCurFrame) { m_iCurFrame = iCurFrame; }
	void SetFrameDelay(int iFrameDelay) { m_iFrameDelay = iFrameDelay; }
	void SetFrameTrigger(int iFrameTrigger) { m_iFrameTrigger = iFrameTrigger; }
	void SetAimFrame(int iAimFrame) { m_iAimFrame = iAimFrame > m_iTotFrame ? -1 : iAimFrame; }
	void SetFrameDeta(int iFrameDeta) { m_iFrameDeta = iFrameDeta >= 0 ? 1 : -1; }
	
	void SetRound(BOOL bRound) { m_bRound = bRound; }
	void SetDie(BOOL bDie) { m_bDie = bDie; }
	void SetColor(int iColor) { m_iColor = iColor; }
	void SetBlood(int iBlood);
	void SetLimSpeed(int iLimSpeed) { m_iLimSpeed = iLimSpeed; }
	void SetDisplay(int iDisplay) { m_iDisplay = iDisplay; }
};