#include"Painter.h"
#include"Controller.h"
#include"Geometry_Wars.h"
#include<ctime>
#include<vector>

using namespace Gdiplus;

// Handle Key
#define PRESS_ENTER (GetAsyncKeyState(VK_RETURN) < 0)
#define PRESS_STOP (GetAsyncKeyState('P') < 0)

#define MOVL (GetAsyncKeyState('A') < 0)
#define MOVR (GetAsyncKeyState('D') < 0)
#define MOVU (GetAsyncKeyState('W') < 0)
#define MOVD (GetAsyncKeyState('S') < 0)
#define MOVUL (MOVL && MOVU)
#define MOVUR (MOVR && MOVU)
#define MOVDL (MOVL && MOVD)
#define MOVDR (MOVR && MOVD)

#define SHOTL (GetAsyncKeyState(VK_LEFT) < 0)
#define SHOTR (GetAsyncKeyState(VK_RIGHT) < 0)
#define SHOTU (GetAsyncKeyState(VK_UP) < 0)
#define SHOTD (GetAsyncKeyState(VK_DOWN) < 0)
#define SHOTUL (SHOTL && SHOTU)
#define SHOTUR (SHOTR && SHOTU)
#define SHOTDL (SHOTL && SHOTD)
#define SHOTDR (SHOTR && SHOTD)

// Bitmap size
const int		g_iPlayerSize      = 40;
const int		g_iBulletSize	   = 13;
const int		g_iEffectSize	   = 6;
const int		g_iBuffSize		   = 25;
const int		g_iBloodSizeW	   = 4;
const int		g_iBloodSizeH	   = 10;

// Blood Size
const int		g_iMinEnemySize    = 25;
const int		g_iMaxEnemyBlood   = 25;
const int	    g_iMaxPlayerBlood  = 50;

// Others
const int		g_iEnemyNum		   = 5;
const int		g_iColorNum		   = 9;
const int		g_iMaxLev		   = 15;
const int		g_iBuffLast		   = 500;

// Sixteen Directions
enum DirPos {

	FU, FX1, FUR, FX2, // U
	FR, FX3, FDR, FX4, // R
	FD, FX5, FDL, FX6, // D
	FL, FX7, FUL, FX8  // L
};

// Const Array
const int		g_iTotFrame[g_iEnemyNum]	= { 1, 8, 8, 8, 8 };
const int		g_iShotDelay[g_iMaxLev]	    = { 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 5 };
const int		g_iXoyVel[g_iMaxLev]	    = { 7, 7, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 11, 12 };
const int		g_iEnemyAreaNum[17]			= { 1, 4, 4, 8, 4, 4, 8, 2, 2, 4, 2, 2, 4, 6, 6, 8, 8 };
const int		g_iDirVel[16][2]			= { {  0, -2 },{  1, -2 },{  2, -2 },{  2, -1 },
												{  2,  0 },{  2,  1 },{  2,  2 },{  1,  2 },
												{  0,  2 },{ -1,  2 },{ -2,  2 },{ -2,  1 },
												{ -2,  0 },{ -2, -1 },{ -2, -2 },{ -1, -2 } };

// Enemy Number
int				g_iCurEnemyNum	   = 0;
int				g_iLimEnemyNum	   = 25;
int				g_iCurKillNum	   = 0;
int				g_iTotKillNum      = 0;

// Player Blood
int				g_iPlayerBlood     = 50;
int				g_iBloodLosing     = 0;

// Player Score
int				g_iScore		   = 0;

// Game Process
BOOL			g_bSplash		   = TRUE;
BOOL			g_bGameOver		   = FALSE;
BOOL			g_bStop			   = FALSE;

// Bullet Property
int				g_iBulletTrigger   = 0;
int				g_iFasterBullet    = 0;
int				g_iFasterShoot     = 0;
int			    g_iDoubleBulletCnt = 0;
int				g_iMultiBulletCnt  = 0;
BOOL			g_bShootFlag       = TRUE;
BOOL			g_bMultiBullet     = FALSE;
BOOL			g_bDoubleBullet    = FALSE;

// Create Buff
POINT			g_rCreateBuffPos   = { 0, 0 };
BOOL			g_pGotBuff		   = FALSE;

// Others
RECT			g_rBound		   = { 0, 0, SCREENWIDTH, SCREENHEIGHT };
int				g_iEnemySize[g_iMinEnemySize + g_iMaxEnemyBlood + 1];

// Game Engine
GameEngine		*g_pGame;

// GDI & GDI+
ULONG_PTR		g_pGdiToken;
HDC				g_hTmpDC;
HBITMAP			g_hTmpBitmap;

// Painter 
Painter			*g_pBackGround;
Painter			*g_pEnemyArr[g_iEnemyNum][g_iMinEnemySize + g_iMaxEnemyBlood + 1];
Painter			*g_pPlayer;
Painter			*g_pBullet;
Painter			*g_pEffect[g_iEnemyNum + 1];
Painter			*g_pBuff;
Painter			*g_pBlood;

// GDI(+) Objects
Controller		*g_pCEnemy[g_iEnemyNum];
Controller		*g_pCPlayer;
Controller		*g_pCBullet;
Controller		*g_pCBuff;
Controller		*g_pCBlood[g_iMaxPlayerBlood + 1];
Controller		g_cPreShot;

// Color Reference
Color cColorRef[g_iColorNum] = {

	Color(	0,	 0, 250), // Blue
	Color(250,	 0,   0), // Red
	Color(	0, 250,   0), // Green
	Color(250,	 0, 250), // Purple
	Color(250, 128,   0), // Orange
	Color(158, 158, 158), // Silver
	Color(250, 200,  55), // Golden
	Color( 38,	38, 250), // Wathet
	Color(250,  38,  38)  // Pink
};

// Enemy Appear Aear
enum AearType {
	
	RANDOM,									   // random
	INNERROUND4_0, INNERROUND4_1, INNERROUND8, // inner, number of 4 or 8
	OUTERROUND4_0, OUTERROUND4_1, OUTERROUND8, // outer, number of 4 or 8
	CORNER2_0, CORNER2_1, CORNER4,             // corner, number of 2 or 4
	EDGE2_0, EDGE2_1, EDGE4,				   // edge, number of 2 or 4
	EDGE6_0, EDGE6_1, EDGE8_0, EDGE8_1		   // edge, number of 6 or 8
};
std::vector<POINT> g_iEnemyArea[17];

// Initilize EnemyAear
void InitEnemyAear() {

	int iW = SCREENWIDTH, iH = SCREENHEIGHT;
	
	g_iEnemyArea[RANDOM].push_back(POINT{ 0, 0 }); // Random, Not to be executed

	g_iEnemyArea[INNERROUND4_0].push_back(POINT{ iW / 2 - 30, iH / 2 - 30 });
	g_iEnemyArea[INNERROUND4_0].push_back(POINT{ iW / 2 + 30, iH / 2 - 30 });
	g_iEnemyArea[INNERROUND4_0].push_back(POINT{ iW / 2 - 30, iH / 2 + 30 });
	g_iEnemyArea[INNERROUND4_0].push_back(POINT{ iW / 2 + 30, iH / 2 + 30 });

	g_iEnemyArea[INNERROUND4_1].push_back(POINT{ iW / 2, iH / 2 - 30 });
	g_iEnemyArea[INNERROUND4_1].push_back(POINT{ iW / 2, iH / 2 + 30 });
	g_iEnemyArea[INNERROUND4_1].push_back(POINT{ iW / 2 - 30, iH / 2 });
	g_iEnemyArea[INNERROUND4_1].push_back(POINT{ iW / 2 + 30, iH / 2 });

	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2 - 40, iH / 2 });
	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2 + 40, iH / 2 });
	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2 - 25, iH / 2 - 25 });
	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2 + 25, iH / 2 - 25 });
	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2 - 25, iH / 2 + 25 });
	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2 + 25, iH / 2 + 25 });
	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2, iH / 2 - 40 });
	g_iEnemyArea[INNERROUND8].push_back(POINT{ iW / 2, iH / 2 + 40 });

	g_iEnemyArea[OUTERROUND4_0].push_back(POINT{ iW * 7 / 20, iH * 3 / 10 });
	g_iEnemyArea[OUTERROUND4_0].push_back(POINT{ iW * 7 / 20, iH * 7 / 10 });
	g_iEnemyArea[OUTERROUND4_0].push_back(POINT{ iW * 13 / 20, iH * 3 / 10 });
	g_iEnemyArea[OUTERROUND4_0].push_back(POINT{ iW * 13 / 20, iH * 7 / 10 });

	g_iEnemyArea[OUTERROUND4_1].push_back(POINT{ iW / 2, iH / 4 });
	g_iEnemyArea[OUTERROUND4_1].push_back(POINT{ iW / 2, iH * 3 / 4 });
	g_iEnemyArea[OUTERROUND4_1].push_back(POINT{ iW * 3 / 10, iH / 2 });
	g_iEnemyArea[OUTERROUND4_1].push_back(POINT{ iW * 7 / 10, iH / 2 });

	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW * 7 / 20, iH / 2 });
	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW * 13 / 20, iH / 2 });
	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW / 2, iH / 4 });
	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW / 2, iH * 3 / 4 });
	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW * 2 / 5, iH * 7 / 20 });
	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW * 3 / 5, iH * 7 / 20 });
	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW * 2 / 5, iH * 13 / 20 });
	g_iEnemyArea[OUTERROUND8].push_back(POINT{ iW * 3 / 5, iH * 13 / 20 });

	g_iEnemyArea[CORNER2_0].push_back(POINT{ 50, 50 });
	g_iEnemyArea[CORNER2_0].push_back(POINT{ iW - 50, iH - 50 });

	g_iEnemyArea[CORNER2_1].push_back(POINT{ iW - 50, 50 });
	g_iEnemyArea[CORNER2_1].push_back(POINT{ 50, iH - 50 });

	g_iEnemyArea[CORNER4].push_back(POINT{ 80, 80 });
	g_iEnemyArea[CORNER4].push_back(POINT{ iW - 80, iH - 80 });
	g_iEnemyArea[CORNER4].push_back(POINT{ iW - 80, 80 });
	g_iEnemyArea[CORNER4].push_back(POINT{ 80, iH - 80 });

	g_iEnemyArea[EDGE2_0].push_back(POINT{ iW / 2, 50 });
	g_iEnemyArea[EDGE2_0].push_back(POINT{ iW / 2, iH - 50 });

	g_iEnemyArea[EDGE2_1].push_back(POINT{ 50, iH / 2 });
	g_iEnemyArea[EDGE2_1].push_back(POINT{ iW - 50, iH / 2 });

	g_iEnemyArea[EDGE4].push_back(POINT{ iW / 2, 80 });
	g_iEnemyArea[EDGE4].push_back(POINT{ iW / 2, iH - 80 });
	g_iEnemyArea[EDGE4].push_back(POINT{ 80, iH / 2 });
	g_iEnemyArea[EDGE4].push_back(POINT{ iW - 80, iH / 2 });

	g_iEnemyArea[EDGE6_0].push_back(POINT{ 80, iH / 7 });
	g_iEnemyArea[EDGE6_0].push_back(POINT{ 80, iH * 2 / 7 });
	g_iEnemyArea[EDGE6_0].push_back(POINT{ 80, iH * 3 / 7 });
	g_iEnemyArea[EDGE6_0].push_back(POINT{ 80, iH * 4 / 7 });
	g_iEnemyArea[EDGE6_0].push_back(POINT{ 80, iH * 5 / 7 });
	g_iEnemyArea[EDGE6_0].push_back(POINT{ 80, iH * 6 / 7 });

	g_iEnemyArea[EDGE6_1].push_back(POINT{ iW - 80, iH / 7 });
	g_iEnemyArea[EDGE6_1].push_back(POINT{ iW - 80, iH * 2 / 7 });
	g_iEnemyArea[EDGE6_1].push_back(POINT{ iW - 80, iH * 3 / 7 });
	g_iEnemyArea[EDGE6_1].push_back(POINT{ iW - 80, iH * 4 / 7 });
	g_iEnemyArea[EDGE6_1].push_back(POINT{ iW - 80, iH * 5 / 7 });
	g_iEnemyArea[EDGE6_1].push_back(POINT{ iW - 80, iH * 6 / 7 });

	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW / 9, 80 });
	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW * 2 / 9, 80 });
	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW / 3, 80 });
	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW * 4 / 9, 80 });
	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW * 5 / 9, 80 });
	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW * 2 / 3, 80 });
	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW * 7 / 9, 80 });
	g_iEnemyArea[EDGE8_0].push_back(POINT{ iW * 8 / 9, 80 });

	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW / 9, iH - 80 });
	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW * 2 / 9, iH - 80 });
	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW / 3, iH - 80 });
	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW * 4 / 9, iH - 80 });
	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW * 5 / 9, iH - 80 });
	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW * 2 / 3, iH - 80 });
	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW * 7 / 9, iH - 80 });
	g_iEnemyArea[EDGE8_1].push_back(POINT{ iW * 8 / 9, iH - 80 });

	// More ...
}

// Game Initialize
BOOL GameInitialize(HWND hWindow) {

	// GameEngine Initialize
	g_pGame = new GameEngine(hWindow);
	if (g_pGame == NULL) return FALSE;
	g_pGame->SetFrameDelay(20); // 50 frames per second
	g_pGame->SetSleep(FALSE);
	
	// GDI+ Initialize
	GdiplusStartupInput m_gdiplusStartupInput;
	GdiplusStartup(&g_pGdiToken, &m_gdiplusStartupInput, NULL);
	srand((unsigned)time(NULL));

	// Windows Initialize
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD size = { 125, 37 };
	SetConsoleScreenBufferSize(hOut, size);
	SMALL_RECT rc = { 0, 0, 124, 36 };
	SetConsoleWindowInfo(hOut, TRUE, &rc);
	CONSOLE_CURSOR_INFO cci;
	SetConsoleTitle("Geometry Wars");
	GetConsoleCursorInfo(hOut, &cci);
	cci.bVisible = 0;
	SetConsoleCursorInfo(hOut, &cci);
	CloseHandle(hOut);

	return TRUE;
}

// Game Start
void GameStart(HWND hWindow) {
	
	// GDI & GDI+ Double-Buffering
	HDC hDC = GetDC(hWindow);
	g_hTmpDC = CreateCompatibleDC(hDC);
	g_hTmpBitmap = CreateCompatibleBitmap(hDC, SCREENWIDTH, SCREENHEIGHT);
	SelectObject(g_hTmpDC, g_hTmpBitmap);

	// Array Initialize
	InitEnemyAear();
	for (int i = 0; i <= g_iMinEnemySize + g_iMaxEnemyBlood; ++i) g_iEnemySize[i] = 5 + i;
	
	// Bitmap Intialize
	g_pBackGround = new Painter(hDC, SCREENWIDTH, SCREENHEIGHT, BACKGROUND, RGB(0, 0, 0));
	g_pPlayer = new Painter(hDC, g_iPlayerSize * 8, g_iPlayerSize, PLAYER);
	g_pBullet = new Painter(hDC, g_iBulletSize * 16, g_iBulletSize, BULLET);
	g_pBuff = new Painter(hDC, g_iBuffSize, g_iBuffSize, BUFF);
	g_pBlood = new Painter(hDC, g_iBloodSizeW * 8, g_iBloodSizeH, BLOOD);
	g_pEffect[SILVER] = new Painter(hDC, g_iEffectSize, g_iEffectSize, SILVER);

	for (int i = 0; i < g_iEnemyNum; ++i) {

		for (int j = 0; j <= g_iMinEnemySize + g_iMaxEnemyBlood; ++j) {

			g_pEnemyArr[i][j] = new Painter(hDC, g_iEnemySize[j] * g_iTotFrame[i], g_iEnemySize[j], ENEMY);

			switch (i) {
			// Five types
			case 0: g_pEnemyArr[i][j]->InitCircle(hDC, cColorRef[BLUE]); break;
			case 1: g_pEnemyArr[i][j]->InitSquare0(hDC, cColorRef[RED]); break;
			case 2: g_pEnemyArr[i][j]->InitSquare1(hDC, cColorRef[GREEN]); break;
			case 3:	g_pEnemyArr[i][j]->InitTriangle0(hDC, cColorRef[PURPLE]); break;
			case 4: g_pEnemyArr[i][j]->InitTriangle1(hDC, cColorRef[ORANGE]); break;
			default: break;
			}
		}
		g_pEffect[i] = new Painter(hDC, g_iEffectSize, g_iEffectSize, EFFECT);
		g_pEffect[i]->InitCircle(hDC, cColorRef[i]);
	}

	g_pEffect[SILVER]->InitCircle(hDC, cColorRef[SILVER]);
	g_pBackGround->InitBackGround(hDC);
	g_pPlayer->InitPlayer(hDC, cColorRef[SILVER]);
	g_pBullet->InitBullet(hDC, cColorRef[GOLDEN]);
	g_pBuff->InitBuff(hDC, cColorRef[WATHET], cColorRef[PINK]);
	g_pBlood->InitSquare1(hDC, cColorRef[PINK]);

	// Create Player
	g_pCPlayer = new Controller(g_pPlayer, PLAYER, 
		POINT{ SCREENWIDTH / 2 - g_iPlayerSize / 2, SCREENHEIGHT / 2 - g_iPlayerSize / 2 }, g_rBound,
		POINT{ 0, 0 }, 8, 0, 2, 0, 0);
	g_pCPlayer->SetColor(SILVER);
	g_pGame->AddController(g_pCPlayer);

	GamePaint(hDC);
}

// Game Over
void GameOver() {

	g_bSplash = TRUE;
	g_pCPlayer->SetSpeed(POINT{ 0, 0 });
	g_pCPlayer->SetPos(POINT{ SCREENWIDTH / 2 - g_iPlayerSize / 2, SCREENHEIGHT / 2 - g_iPlayerSize / 2 });
	g_pCPlayer->SetAimFrame(0);

	Sleep(100);
}

// New Game
void NewGame() {

	g_bSplash = FALSE;
	g_bGameOver = FALSE;

	for (int i = 0; i <= g_iMaxPlayerBlood; ++i) {

		g_pCBlood[i] = new Controller(g_pBlood, BLOOD, POINT{ 10 + i * g_iBloodSizeW, 10 }, g_rBound, 
			POINT{ 0, 0 }, 8, 0, 0, 0, 0);
		g_pGame->AddEffect(g_pCBlood[i]);
	}

	g_pGame->SetEnemyDie();

	g_iCurEnemyNum = 0;
	g_iLimEnemyNum = 25;
	g_iCurKillNum = 0;
	g_iTotKillNum = 0;
	g_iPlayerBlood = 50;
	g_iBloodLosing = 0;
	g_iScore = 0;
	g_iBulletTrigger = 0;
	g_iFasterBullet = 0;
	g_iFasterShoot = 0;
	g_iDoubleBulletCnt = 0;
	g_iMultiBulletCnt = 0;

	g_bShootFlag = TRUE;
	g_bMultiBullet = FALSE;
	g_bDoubleBullet = FALSE;

	g_rCreateBuffPos = { 0, 0 };
	g_pGotBuff = FALSE;
	g_cPreShot.SetBitmap(NULL);
}

// Game End
void GameEnd() {

	// Game
	g_pGame->ClearController();
	delete g_pGame;

	// GDI & GDI+
	DeleteObject(g_hTmpBitmap);
	DeleteDC(g_hTmpDC);
	GdiplusShutdown(g_pGdiToken);
	
	// GDI(+) Object
	delete g_pBackGround;
	delete g_pPlayer;
	delete g_pBullet;
	delete g_pBuff;
	delete g_pBlood;
	delete g_pEffect[SILVER];

	for (int i = 0; i < g_iEnemyNum; ++i) {

		for (int j = 0; j <= g_iMinEnemySize + g_iMaxEnemyBlood; ++j) {

			delete g_pEnemyArr[i][j];
		}
		delete g_pEffect[i];
	}
}

// Game Paint
void GamePaint(HDC hDC) {

	g_pBackGround->Draw(hDC, 0, 0, FALSE); // BackGround : Not transparent
	g_pGame->DrawController(hDC); // Draw others objects

	if (g_bSplash) { // Splash, player are not allowed to move

		TCHAR str[32] = "Press <Enter> to Begin";
		RECT  rect = { SCREENWIDTH / 2 - 80, SCREENHEIGHT / 2 + 35, SCREENWIDTH / 2 + 80, SCREENHEIGHT / 2 + 55 };
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(255, 255, 255));
		DrawText(hDC, str, -1, &rect, DT_SINGLELINE | DT_CENTER);
		return;
	}
	if (g_bGameOver) { // GameOver, waiting for new game

		TCHAR str[32] = "Press <Enter> to Restart";
		RECT  rect = { SCREENWIDTH / 2 - 80, SCREENHEIGHT / 2 + 35, SCREENWIDTH / 2 + 80, SCREENHEIGHT / 2 + 55 };
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(255, 255, 255));
		DrawText(hDC, str, -1, &rect, DT_SINGLELINE | DT_CENTER);
	}

	// Print the scores
	TCHAR str[32] = "Score :  ";
	wsprintf(str + 9, "%d", g_iScore);
	RECT  rect = { SCREENWIDTH - 130, 10, SCREENWIDTH, 30 };
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 255));
	DrawText(hDC, str, -1, &rect, DT_SINGLELINE | DT_LEFT);
}

// Game Cycle
void GameCycle() {

	// Game Stop
	if (g_bStop) return;

	// GDI(+) Paint
	g_pGame->UpdateController();
	HWND hWindow = g_pGame->GetWindow();
	HDC hDC = GetDC(hWindow);
	GamePaint(g_hTmpDC);
	BitBlt(hDC, 0, 0, SCREENWIDTH, SCREENHEIGHT, g_hTmpDC, 0, 0, SRCCOPY);
	ReleaseDC(hWindow, hDC);
	
	if (g_bSplash) { // Splash mode, randomly create enemy

		if (g_iCurEnemyNum < 10 && rand() % 30 == 0) {

			++g_iCurEnemyNum;
			int id = rand() % g_iEnemyNum;
			if (id == BLUE && (g_iCurEnemyNum & 1)) id = rand() % g_iEnemyNum; // Blue Enemy not too much

			int x = rand() % (SCREENWIDTH - 100) + 50, y = rand() % (SCREENHEIGHT - 100) + 50;
			g_pCEnemy[id] = new Controller(g_pEnemyArr[id][g_iMinEnemySize], ENEMY, POINT{ x, y }, g_rBound,
				POINT{ 0, 0 }, g_iTotFrame[id], 0, 2);

			g_pCEnemy[id]->SetColor(id);
			g_pCEnemy[id]->SetBlood(0);

			g_pGame->AddController(g_pCEnemy[id]);
		}
		return;
	}

	if (g_bGameOver) { // GameOver mode, randomly create effect

		if (rand() % 15 == 0) {

			int id = rand() % g_iEnemyNum, ib = rand() % (g_iMaxEnemyBlood + 1);
			int x = rand() % (SCREENWIDTH - 100) + 50, y = rand() % (SCREENHEIGHT - 100) + 50;
			g_pCEnemy[id] = new Controller(g_pEnemyArr[id][g_iMinEnemySize + ib], ENEMY, POINT{ x, y }, g_rBound,
				POINT{ 0, 0 }, g_iTotFrame[id], 0, 2);

			g_pGame->CreateEffect(g_pCEnemy[id], id, 125 + ib * 4, 30 + ib);
		}
		return;
	}

	// Trigger
	if (g_iBulletTrigger >= g_iShotDelay[g_iFasterShoot]) {

		// ShootDelay
		g_bShootFlag = TRUE;
	}
	else {

		++g_iBulletTrigger;
	}

	if (g_iDoubleBulletCnt >= g_iBuffLast) {

		// DoubleBullet
		g_bDoubleBullet = FALSE;
	}
	else if (g_bDoubleBullet) {

		++g_iDoubleBulletCnt;
	}

	if (g_iMultiBulletCnt >= g_iBuffLast) {

		// MultiBullet
		g_bMultiBullet = FALSE;
	}
	else if (g_bMultiBullet) {

		++g_iMultiBulletCnt;
	}

	if (g_iBloodLosing > 0) { // Blood Losing

		while (g_iBloodLosing > 0 && g_iPlayerBlood >= 0) {

			g_pCBlood[g_iPlayerBlood]->SetDie(TRUE);
			g_pGame->CreateEffect(g_pCBlood[g_iPlayerBlood], RED, 30, 10);

			--g_iPlayerBlood;
			--g_iBloodLosing;
		}
		if (g_iBloodLosing > 0) { // Blood drop to zero

			g_pGame->SetEnemyDie();
			g_pCPlayer->SetSpeed(POINT{ 0, 0 });
			g_pGame->CreateEffect(g_pCPlayer, SILVER, 400, 100);

			g_bGameOver = TRUE;
			return;
		}
	}
	
	if (g_iCurKillNum) { // Create Buff

		g_iTotKillNum += g_iCurKillNum;

		if (rand() % 20 <= g_iCurKillNum) {

			int dW = g_pBuff->GetWidth() / 2, dH = g_pBuff->GetHeight() / 2;
			g_pCBuff = new Controller(g_pBuff, BUFF, POINT{ g_rCreateBuffPos.x - dW, g_rCreateBuffPos.y - dH }, 
				RECT{0, 0, SCREENWIDTH, SCREENHEIGHT}, POINT{ 0, 0 });

			g_pGame->AddController(g_pCBuff);
		}

		g_iCurKillNum = 0;
	}

	if (g_pGotBuff) { // Got Buff

		int r = rand() % 4; // Just randomly improve

		if (r == 0) {

			g_iFasterBullet = min(g_iFasterBullet + 1, g_iMaxLev - 1);
		}
		else if (r == 1) {

			g_bDoubleBullet = TRUE, g_iDoubleBulletCnt = 0;
		}
		else if (r == 2) {

			g_bMultiBullet = TRUE, g_iMultiBulletCnt = 0;
		}
		else if (r == 3) {

			g_iFasterShoot = min(g_iFasterShoot + 1, g_iMaxLev - 1);
		}
		else; // None

		g_pGotBuff = FALSE;
	}

	// Enemy Appear Mode1, with an area array
	if (g_iCurEnemyNum < min(g_iScore / 1000 + 10, g_iLimEnemyNum) && rand() % 50 == 0) {

		int id = rand() % g_iEnemyNum, iType = rand() % 16 + 1;
		int ib = rand() % min(g_iMaxEnemyBlood / 5 + 1, g_iScore / 5000 + 1);

		for (int i = 0; i < g_iEnemyAreaNum[iType]; ++i) {

			int x = g_iEnemyArea[iType][i].x, y = g_iEnemyArea[iType][i].y;
			g_pCEnemy[id] = new Controller(g_pEnemyArr[id][g_iMinEnemySize + ib], ENEMY, POINT{ x, y }, g_rBound,
				POINT{ 0, 0 }, g_iTotFrame[id], 0, 2);

			g_pCEnemy[id]->SetColor(id);
			g_pCEnemy[id]->SetBlood(ib);
			g_pGame->AddController(g_pCEnemy[id]);
		}

		g_iCurEnemyNum += g_iEnemyAreaNum[iType];
	}
	// Enemy Appear Mode2, randomly appear
	else if (g_iCurEnemyNum < min(g_iScore / 400 + 10, g_iLimEnemyNum) && rand() % 30 == 0) {

		int id = rand() % g_iEnemyNum, ib = rand() % min(g_iScore / 1000 + 1, g_iMaxEnemyBlood + 1);
		int x = rand() % (SCREENWIDTH - 80) + 40, y = rand() % (SCREENHEIGHT - 80) + 40;
		g_pCEnemy[id] = new Controller(g_pEnemyArr[id][g_iMinEnemySize + ib], ENEMY, POINT{ x, y }, g_rBound,
			POINT{ 0, 0 }, g_iTotFrame[id], 0, 2);

		g_pCEnemy[id]->SetColor(id);
		g_pCEnemy[id]->SetBlood(ib);

		g_pGame->AddController(g_pCEnemy[id]);
		++g_iCurEnemyNum;
	}
}

// Player Shot
void CreateBullet(DirPos dDir, Controller *player) {

	if (!g_bShootFlag) return;
	
	// Create Bullet
	RECT rPos = player->GetPos();
	int w = player->GetWidth() * 11 / 30, h = player->GetHeight() * 11 / 30;
	Controller *p = new Controller(g_pBullet, 0, POINT{rPos.left + w, rPos.top + h }, g_rBound,
		POINT{ g_iDirVel[dDir][0] * g_iXoyVel[g_iFasterBullet], g_iDirVel[dDir][1] * g_iXoyVel[g_iFasterBullet] }, 
		16, dDir, 0, 0, dDir, 1, DIE);
	g_pGame->AddController(p);
	
	// MultiBullet
	if (g_bMultiBullet) {

		DirPos dDir2 = (DirPos)((dDir + 15) % 16);
		Controller *p2 = new Controller(g_pBullet, BULLET, POINT{ rPos.left + w, rPos.top + h }, g_rBound,
			POINT{ g_iDirVel[dDir2][0] * g_iXoyVel[g_iFasterBullet], g_iDirVel[dDir2][1] * g_iXoyVel[g_iFasterBullet] }, 
			16, dDir2, 0, 0, dDir2, 1, DIE);
		g_pGame->AddController(p2);
		
		DirPos dDir3 = (DirPos)((dDir + 1) % 16);
		Controller *p3 = new Controller(g_pBullet, BULLET, POINT{ rPos.left + w, rPos.top + h }, g_rBound,
			POINT{ g_iDirVel[dDir3][0] * g_iXoyVel[g_iFasterBullet], g_iDirVel[dDir3][1] * g_iXoyVel[g_iFasterBullet] }, 
			16, dDir3, 0, 0, dDir3, 1, DIE);
		g_pGame->AddController(p3);
	}
	
	g_bShootFlag = FALSE;
	g_iBulletTrigger = 0;
	g_cPreShot = *p;
}

void HandleKeys() {

	if (g_bStop && PRESS_STOP) {

		g_bStop = FALSE;
		Sleep(100);
		return;
	}
	else if (!g_bStop && PRESS_STOP) {

		g_bStop = TRUE;
		Sleep(100);
		return;
	}
	if (g_bSplash) {

		if (!PRESS_ENTER) return;
		NewGame();
	}
	else if (g_bGameOver) {

		if (!PRESS_ENTER) return;
		GameOver();
	}

	Controller *p = g_pCPlayer;
	const int VX = 6, VY = 5;
	
	// Move
	if (MOVUL) {

		p->SetSpeed(-VX, -VY);
		p->SetAimFrame(FUL / 2);
	}
	else if (MOVUR) {

		p->SetSpeed(VX, -VY);
		p->SetAimFrame(FUR / 2);
	}
	else if (MOVDL) {

		p->SetSpeed(-VX, VY);
		p->SetAimFrame(FDL / 2);
	}
	else if (MOVDR) {

		p->SetSpeed(VX, VY);
		p->SetAimFrame(FDR / 2);
	}
	else if (MOVU) {

		p->SetSpeed(0, -VY);
		p->SetAimFrame(FU / 2);
	}
	else if (MOVD) {

		p->SetSpeed(0, VY);
		p->SetAimFrame(FD / 2);
	}
	else if (MOVL) {

		p->SetSpeed(-VX, 0);
		p->SetAimFrame(FL / 2);
	}
	else if (MOVR) {

		p->SetSpeed(VX, 0);
		p->SetAimFrame(FR / 2);
	}
	else p->SetSpeed(0, 0);
	
	// Shoot
	if (SHOTUL) {

		CreateBullet(FUL, p);
	}
	else if (SHOTUR) {

		CreateBullet(FUR, p);
	}
	else if (SHOTDL) {

		CreateBullet(FDL, p);
	}
	else if (SHOTDR) {

		CreateBullet(FDR, p);
	}
	else if (SHOTU) {

		CreateBullet(FU, p);
	}
	else if (SHOTD) {

		CreateBullet(FD, p);
	}
	else if (SHOTL) {

		CreateBullet(FL, p);
	}
	else if (SHOTR) {

		CreateBullet(FR, p);
	}
	p->SetFrameDeta(p->GetLowestDir());

	// DoubleBullet
	if (g_bDoubleBullet && !g_bShootFlag && 
		g_iBulletTrigger == g_iShotDelay[0] / 4 && g_cPreShot.GetBitmap() != NULL) {

		Controller *pTmp = new Controller();
		RECT rPos = p->GetPos();
		int w = p->GetWidth() * 11 / 30, h = p->GetHeight() * 11 / 30;

		g_cPreShot.SetPos(POINT{ rPos.left + w, rPos.top + h });
		*pTmp = g_cPreShot;
		g_pGame->AddController(pTmp);
		
		if (g_bMultiBullet) {

			DirPos dDir = (DirPos)g_cPreShot.GetAimFrame();
			DirPos dDir2 = (DirPos)((dDir + 15) % 16), dDir3 = (DirPos)((dDir + 1) % 16);
			
			Controller *pTmp2 = new Controller();
			*pTmp2 = g_cPreShot; pTmp2->SetCurFrame(dDir2); pTmp2->SetAimFrame(dDir2);
			pTmp2->SetSpeed(g_iDirVel[dDir2][0] * g_iXoyVel[g_iFasterBullet], g_iDirVel[dDir2][1] * g_iXoyVel[g_iFasterBullet]);
			g_pGame->AddController(pTmp2);
			
			Controller *pTmp3 = new Controller();
			*pTmp3 = g_cPreShot; pTmp3->SetCurFrame(dDir3); pTmp3->SetAimFrame(dDir3);
			pTmp3->SetSpeed(g_iDirVel[dDir3][0] * g_iXoyVel[g_iFasterBullet], g_iDirVel[dDir3][1] * g_iXoyVel[g_iFasterBullet]);
			g_pGame->AddController(pTmp3);
		}
	}
}