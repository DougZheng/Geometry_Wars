#pragma once
#include<windows.h>
#include<vector>
#include"Controller.h"

// GameEngine Help Function
BOOL GameInitialize(HWND hWindow);			      
void GameStart(HWND hWindow);					  
void GameOver();								  
void NewGame();									  
void GameEnd();									  
void GamePaint(HDC hDC);					 	  
void GameCycle();								  
void HandleKeys();								  
BOOL Collision(Controller *pC1, Controller *pC2);

class GameEngine {

private:
	
	HWND						m_hWindow; 		 	  // Handle of window
	int							m_iWidth, m_iHeight;  // W, h of screen
	int						    m_iFrameDelay;		  // Delay per frame
	BOOL						m_bSleep;			  // Not been used
	std::vector<Controller *>	m_vController;		  // Sprite controller
	std::vector<Controller *>	m_vEffect;	          // Special effect

public:
	
	GameEngine(HWND hWindow, int iwidth = SCREENWIDTH, int iHeight = SCREENHEIGHT);
	~GameEngine();

	// Function of sprite controller
	BOOL CheckCollision(Controller *pC);
	void AddController(Controller *pC);
	void DrawController(HDC hDC);
	void UpdateController();
	void ClearController();
	void SetEnemyDie();

	// Function of special effect
	void AddEffect(Controller *pC);
	void CreateEffect(Controller *pBomb, int iColor, int iRange, int iSize);

	// Get Function
	HWND GetWindow() { return m_hWindow; }
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	int GetFrameDelay() { return m_iFrameDelay; }
	BOOL GetSleep() { return m_bSleep; }

	// Set Function
	void SetWindow(HWND hWindow) { m_hWindow = hWindow; }
	void SetWidth(int iWidth) { m_iWidth = iWidth; }
	void SetHeight(int iHeight) { m_iHeight = iHeight; }
	void SetFrameDelay(int iFrameDelay) { m_iFrameDelay = iFrameDelay; }
	void SetSleep(BOOL bSleep) { m_bSleep = bSleep; }
};