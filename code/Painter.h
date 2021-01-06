#pragma once
#include<windows.h>
#include<gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")

#define SCREENWIDTH 1000
#define SCREENHEIGHT 592

// Object color type
enum ColorType {

	BLUE, RED, 
	GREEN, PURPLE,
	ORANGE, SILVER, 
	GOLDEN, WATHET, PINK
};

// Object type, Z order
enum ConType {

	BACKGROUND, EFFECT, 
	ENEMY, BULLET, 
	PLAYER, BUFF, BLOOD
};

class Painter {

private:

	HBITMAP		m_hBitmap;			  // Handle of bitmap
	int		    m_iWidth, m_iHeight;  // W, h of bitmap
	int			m_iType;			  // Object type

public:

	Painter(HDC hDC, int iWidth, int iHeight, int iType, COLORREF cColor = RGB(255, 0, 255));
	~Painter();

	// Draw in bitmap
	void InitBackGround(HDC hDC);
	void InitCircle(HDC hDC, Gdiplus::Color cColor);
	void InitSquare0(HDC hDC, Gdiplus::Color cColor);
	void InitSquare1(HDC hDC, Gdiplus::Color cColor);
	void InitTriangle0(HDC hDC, Gdiplus::Color cColor);
	void InitTriangle1(HDC hDC, Gdiplus::Color cColor);
	void InitPlayer(HDC hDC, Gdiplus::Color cColor);
	void InitBullet(HDC hDC, Gdiplus::Color cColor);
	void InitBuff(HDC hDC, Gdiplus::Color cColor, Gdiplus::Color cColor2);
	
	// Draw or draw some part
	void Draw(HDC hDC, int iX, int iY, BOOL bTrans = TRUE, COLORREF cColor = RGB(255, 0, 255));
	void DrawPart(HDC hDC, int iDX, int iDY, int iSX, int iSY,
		int iW, int iH, BOOL bTrans = TRUE, COLORREF cColor = RGB(255, 0, 255));
	
	// Get Function
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	int GetType() { return m_iType; }

	// Not Set Function
};