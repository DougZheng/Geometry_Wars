#include"Painter.h"
#include<cstdlib>

using namespace Gdiplus;

// Painter constructor, create a transparent bitmap
Painter::Painter(HDC hDC, int iWidth, int iHeight, int iType, COLORREF cColor) {

	m_iWidth = iWidth, m_iHeight = iHeight; m_iType = iType;
	m_hBitmap = CreateCompatibleBitmap(hDC, iWidth, iHeight);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBRUSH hBrush = CreateSolidBrush(cColor);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	RECT rRect = { 0, 0, m_iWidth, m_iHeight };
	FillRect(hMemDC, &rRect, hBrush);
	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hMemDC);
	DeleteObject(hBrush);
}

void Painter::InitBackGround(HDC hDC) {

	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	Graphics g(hMemDC);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	Color colors[] = { Color(0, 0, 255) }; int iCnt = 1;
	for (int i = 0; i < 400; ++i) {

		int x = rand() % SCREENWIDTH;
		int y = rand() % SCREENHEIGHT;
		int w = rand() % 3 + 1, h = w;
		GraphicsPath gPath;
		gPath.AddEllipse(x, y, w, h);
		PathGradientBrush pGB(&gPath);
		pGB.SetSurroundColors(colors, &iCnt);
		g.FillEllipse(&pGB, x, y, w, h);
	}
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

Painter::~Painter() {

	if (m_hBitmap != NULL) {

		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

void Painter::InitCircle(HDC hDC, Color cColor) {

	int w = GetWidth(), h = GetHeight();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	Graphics g(hMemDC);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	GraphicsPath gPath;
	gPath.AddEllipse(0, 0, w, h);
	Color colors[] = { cColor }; int iCnt = 1;
	PathGradientBrush pGB(&gPath);
	pGB.SetSurroundColors(colors, &iCnt);
	g.FillEllipse(&pGB, 0, 0, w, h);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::InitSquare0(HDC hDC, Color cColor) {

	int w = GetWidth() / 8, h = GetHeight();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	HBITMAP hRot = CreateCompatibleBitmap(hDC, w, h);
	HDC hRotDC = CreateCompatibleDC(hDC);
	HBITMAP hOldRot = (HBITMAP)SelectObject(hRotDC, hRot);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255));
	RECT rRect = { 0, 0, w, h };
	Point pPoints[8][4] = { { { w / 2, 0 },{ 0, h / 2 },{ w / 2, h },{ w, h / 2 } },
	{ { w / 2, 0 },{ 14 * w / 100, 58 * h / 100 },{ w / 2, h },{ 86 * w / 100, 42 * h / 100 } },
	{ { w / 2, 0 },{ 23 * w / 100, 61 * h / 100 },{ w / 2, h },{ 77* w / 100, 39 * h / 100 } },
	{ { w / 2, 0 },{ 35 * w / 100, 62 * h / 100 },{ w / 2, h },{ 65 * w / 100, 38 * h / 100 } },
	{ { w / 2, 0 },{ 46 * w / 100, 63 * h / 100 },{ w / 2, h },{ 54 * w / 100, 37 * h / 100 } },
	{ { w / 2, 0 },{ 39 * w / 100, 37 * h / 100 },{ w / 2, h },{ 61 * w / 100, 63 * h / 100 } },
	{ { w / 2, 0 },{ 26 * w / 100, 39 * h / 100 },{ w / 2, h },{ 74 * w / 100, 61 * h / 100 } },
	{ { w / 2, 0 },{ 13 * w / 100, 42 * h / 100 },{ w / 2, h },{ 87 * w / 100, 58 * h / 100 } } };
	Color colors[] = { cColor }; int iCnt = 1;
	for (int i = 0; i < 8; ++i) {

		FillRect(hRotDC, &rRect, hBrush);
		Graphics g(hRotDC);
		g.SetSmoothingMode(SmoothingModeHighSpeed);
		GraphicsPath gPath;
		gPath.AddPolygon(pPoints[i], 4);
		PathGradientBrush pGB(&gPath);
		pGB.SetSurroundColors(colors, &iCnt);
		g.FillPolygon(&pGB, pPoints[i], 4);
		BitBlt(hMemDC, i * w, 0, w, h, hRotDC, 0, 0, SRCCOPY);
	}
	SelectObject(hRotDC, hOldRot);
	DeleteDC(hRotDC);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::InitSquare1(HDC hDC, Color cColor) {

	int w = GetWidth() / 8, h = GetHeight();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	HDC hRotDC = CreateCompatibleDC(hDC);
	HBITMAP hRot = CreateCompatibleBitmap(hDC, w, h);
	HBITMAP hOldRot = (HBITMAP)SelectObject(hRotDC, hRot);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255));
	RECT rRect = { 0, 0, w, h };
	PointF pRotCen = { w / 2.0f, h * 52 / 100.0f };
	Point pPoints[] = { { w * 15 / 100, h * 15 / 100 },{ w * 85 / 100, h * 15 / 100 },{ w * 85 / 100, h * 85 / 100 },{ w * 15 / 100, h * 85 / 100 } };
	for (int i = 0; i < 4; ++i) pPoints[i].X -= w / 2, pPoints[i].Y -= h / 2;
	Color colors[] = { cColor }; int iCnt = 1;
	for (int i = 0; i < 8; ++i) {

		FillRect(hRotDC, &rRect, hBrush);
		Graphics g(hRotDC);
		g.SetSmoothingMode(SmoothingModeHighSpeed);
		g.TranslateTransform(pRotCen.X, pRotCen.Y);
		for (int j = 0; j < i; ++j) g.RotateTransform(11.125);
		GraphicsPath gPath;
		gPath.AddPolygon(pPoints, 4);
		PathGradientBrush pGB(&gPath);
		pGB.SetSurroundColors(colors, &iCnt);
		g.FillPolygon(&pGB, pPoints, 4);
		g.TranslateTransform(-pRotCen.X, -pRotCen.Y);
		BitBlt(hMemDC, i * w, 0, w, h, hRotDC, 0, 0, SRCCOPY);
	}
	SelectObject(hRotDC, hOldRot);
	DeleteDC(hRotDC);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::InitTriangle0(HDC hDC, Color cColor) {

	int w = GetWidth() / 8, h = GetHeight();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	HDC hRotDC = CreateCompatibleDC(hDC);
	HBITMAP hRot = CreateCompatibleBitmap(hDC, w, h);
	HBITMAP hOldRot = (HBITMAP)SelectObject(hRotDC, hRot);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255));
	RECT rRect = { 0, 0, w, h };
	PointF pRotCen = { w / 2.0f, h * 52 / 100.0f };
	Point pPoints[] = { { w / 2, h * 1 / 100 },{ w * 5 / 100, h * 78 / 100 },{ w * 95 / 100, h * 78 / 100 } };
	for (int i = 0; i < 3; ++i) pPoints[i].X -= w / 2, pPoints[i].Y -= h * 52 / 100;
	Color colors[] = { cColor }; int iCnt = 1;
	for (int i = 0; i < 8; ++i) {

		FillRect(hRotDC, &rRect, hBrush);
		Graphics g(hRotDC);
		g.SetSmoothingMode(SmoothingModeHighSpeed);
		g.TranslateTransform(pRotCen.X, pRotCen.Y);
		for (int j = 0; j < i; ++j) g.RotateTransform(15.0f);
		GraphicsPath gPath;
		gPath.AddPolygon(pPoints, 3);
		PathGradientBrush pGB(&gPath);
		pGB.SetSurroundColors(colors, &iCnt);
		g.FillPolygon(&pGB, pPoints, 3);
		g.TranslateTransform(-pRotCen.X, -pRotCen.Y);
		BitBlt(hMemDC, i * w, 0, w, h, hRotDC, 0, 0, SRCCOPY);
	}
	SelectObject(hRotDC, hOldRot);
	DeleteDC(hRotDC);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::InitTriangle1(HDC hDC, Color cColor) {

	int w = GetWidth() / 8, h = GetHeight();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	HBITMAP hRot = CreateCompatibleBitmap(hDC, w, h);
	HDC hRotDC = CreateCompatibleDC(hDC);
	HBITMAP hOldRot = (HBITMAP)SelectObject(hRotDC, hRot);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255));
	RECT rRect = { 0, 0, w, h };
	Point pPoints[8][3] = { { { 2 * w / 100, 14 * h / 100 },{ 98 * w / 100, 14 * h / 100 },{ 50 * w / 100, 97 * h / 100 } },
	{ { 11 * w / 100, 21 * h / 100 },{ 89 * w / 100, 7 * h / 100 },{ 50 * w / 100, 97 * h / 100 } },
	{ { 22 * w / 100, 24 * h / 100 },{ 78 * w / 100, 4 * h / 100 },{ 50 * w / 100, 97 * h / 100 } },
	{ { 33 * w / 100, 26 * h / 100 },{ 67 * w / 100, 2 * h / 100 },{ 50 * w / 100, 97 * h / 100 } },
	{ { 45 * w / 100, 27 * h / 100 },{ 55 * w / 100, 1 * h / 100 },{ 50 * w / 100, 97 * h / 100 } },
	{ { 62 * w / 100, 27 * h / 100 },{ 38 * w / 100, 1 * h / 100 },{ 50 * w / 100, 97 * h / 100 } },
	{ { 76 * w / 100, 25 * h / 100 },{ 24 * w / 100, 3 * h / 100 },{ 50 * w / 100, 97 * h / 100 } },
	{ { 88 * w / 100, 22 * h / 100 },{ 12 * w / 100, 6 * h / 100 },{ 50 * w / 100, 97 * h / 100 } } };
	Color colors[] = { cColor }; int iCnt = 1;
	for (int i = 0; i < 8; ++i) {

		FillRect(hRotDC, &rRect, hBrush);
		Graphics g(hRotDC);
		g.SetSmoothingMode(SmoothingModeHighSpeed);
		GraphicsPath gPath;
		gPath.AddPolygon(pPoints[i], 3);
		PathGradientBrush pGB(&gPath);
		pGB.SetSurroundColors(colors, &iCnt);
		g.FillPolygon(&pGB, pPoints[i], 3);
		BitBlt(hMemDC, i * w, 0, w, h, hRotDC, 0, 0, SRCCOPY);
	}
	SelectObject(hRotDC, hOldRot);
	DeleteDC(hRotDC);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::InitPlayer(HDC hDC, Color cColor) {

	float w = (float)GetWidth() / 8.0f, h = (float)GetHeight() * 1.15f;
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	HDC hRotDC = CreateCompatibleDC(hDC);
	HBITMAP hRot = CreateCompatibleBitmap(hDC, (int)w, (int)h);
	HBITMAP hOldRot = (HBITMAP)SelectObject(hRotDC, hRot);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255));
	RECT rRect = { 0, 0, (int)w, (int)h };
	PointF pRotCen = { w / 2, h * 9 / 20 };
	PointF pPoints[] = { { 15 * w / 50, 7 * h / 50 },{ 12 * w / 50, 8 * h / 50 },{ 4 * w / 50, 19 * h / 50 },
	{ 4 * w / 50, 23 * h / 50 }, { 22 * w / 50, 39 * h / 50 },{ 28 * w / 50, 39 * h / 50 },{ 46 * w / 50, 23 * h / 50 },
	{ 46 * w / 50, 19 * h / 50 },{ 38 * w / 50, 8 * h / 50 }, { 35 * w / 50, 7 * h / 50 },{ 34 * w / 50, 11 * h / 50 },
	{ 37 * w / 50, 15 * h / 50 },{ 25 * w / 50, 21 * h / 50 },{ 13 * w / 50, 15 * h / 50 }, { 16 * w / 50, 11 * h / 50 } };
	PointF pPoints2[] = { { 24 * w / 50, 28 * h / 50 },{ 11 * w / 50, 20 * h / 50 },{ 10 * w / 50, 22 * h / 50 },{ 24 * w / 50, 33 * h / 50 },
	{ 26 * w / 50, 33 * h / 50 },{ 40 * w / 50, 22 * h / 50 },{ 39 * w / 50, 20 * h / 50 },{ 26 * w / 50, 28 * h / 50 } };
	for (int i = 0; i < 15; ++i) pPoints[i].X -= w / 2, pPoints[i].Y -= h * 9 / 20;
	for (int i = 0; i < 8; ++i) pPoints2[i].X -= w / 2, pPoints2[i].Y -= h * 9 / 20;
	Color colors[] = { cColor }; int iCnt = 1;
	for (int i = 0; i < 8; ++i) {

		FillRect(hRotDC, &rRect, hBrush);
		Graphics g(hRotDC);
		g.SetSmoothingMode(SmoothingModeHighSpeed);
		g.TranslateTransform(pRotCen.X, pRotCen.Y);
		for (int j = 0; j < i; ++j) g.RotateTransform(45.0f);
		GraphicsPath gPath;
		gPath.AddPolygon(pPoints, 15);
		PathGradientBrush pGB(&gPath);
		pGB.SetSurroundColors(colors, &iCnt);
		g.FillPolygon(&pGB, pPoints, 15);
		SolidBrush sBr(Color(255, 0, 255));
		g.FillPolygon(&sBr, pPoints2, 8);
		g.TranslateTransform(-pRotCen.X, -pRotCen.Y);
		BitBlt(hMemDC, i * (int)w, 0, (int)w, (int)h, hRotDC, 0, 0, SRCCOPY);
	}
	SelectObject(hRotDC, hOldRot);
	DeleteDC(hRotDC);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::InitBullet(HDC hDC, Gdiplus::Color cColor) {

	int w = GetWidth() / 16, h = GetHeight();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	HDC hRotDC = CreateCompatibleDC(hDC);
	HBITMAP hRot = CreateCompatibleBitmap(hDC, w, h);
	HBITMAP hOldRot = (HBITMAP)SelectObject(hRotDC, hRot);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255));
	RECT rRect = { 0, 0, w, h };
	PointF pRotCen = { w / 2.0f, h / 2.0f };
	Point pPoints[] = { { w * 11 / 50, h * 6 / 50 },{ w * 18 / 50, h * 6 / 50 },
	{ w * 22 / 50, h * 42 / 50 },{ w * 6 / 50, h * 42 / 50 } };
	Point pPoints2[] = { { w * 32 / 50, h * 6 / 50 },{ w * 39 / 50, h * 6 / 50 },
	{ w * 43 / 50, h * 42 / 50 },{ w * 27 / 50, h * 42 / 50 } };
	for (int i = 0; i < 4; ++i) pPoints[i].X -= w / 2, pPoints[i].Y -= h / 2;
	for (int i = 0; i < 4; ++i) pPoints2[i].X -= w / 2, pPoints2[i].Y -= h / 2;
	Color colors[] = { cColor }; int iCnt = 1;
	for (int i = 0; i < 16; ++i) {

		FillRect(hRotDC, &rRect, hBrush);
		Graphics g(hRotDC);
		g.SetSmoothingMode(SmoothingModeHighSpeed);
		g.TranslateTransform(pRotCen.X, pRotCen.Y);
		for (int j = 0; j < i; ++j) g.RotateTransform(22.5f);
		GraphicsPath gPath;
		gPath.AddClosedCurve(pPoints, 4);
		gPath.AddClosedCurve(pPoints2, 4);
		PathGradientBrush pGB(&gPath);
		pGB.SetSurroundColors(colors, &iCnt);
		g.FillClosedCurve(&pGB, pPoints, 4);
		g.FillClosedCurve(&pGB, pPoints2, 4);
		g.TranslateTransform(-pRotCen.X, -pRotCen.Y);
		BitBlt(hMemDC, i * w, 0, w, h, hRotDC, 0, 0, SRCCOPY);
	}
	SelectObject(hRotDC, hOldRot);
	DeleteDC(hRotDC);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::InitBuff(HDC hDC, Color cColor, Color cColor2) {

	int w = GetWidth(), h = GetHeight();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	Graphics g(hMemDC);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	Point pPoints[] = { { w * 18 / 50, h * 3 / 50 },{ w * 3 / 50, h * 24 / 50 },{ w * 11 / 50, h * 22 / 50 },
	{ w * 13 / 50, h * 43 / 50 }, { w * 23 / 50, h * 43 / 50 },{ w * 25 / 50, h * 22 / 50 },{ w * 33 / 50, h * 24 / 50 } };
	Point pPoints2[] = { { w * 31 / 50, h * 9 / 50 },{ w * 16 / 50, h * 30 / 50 },{ w * 24 / 50, h * 28 / 50 },
	{ w * 26 / 50, h * 49 / 50 },{ w * 36 / 50, h * 49 / 50 },{ w * 38 / 50, h * 28 / 50 },{ w * 46 / 50, h * 30 / 50 } };
	Point pCen(w * 18 / 50, h * 13 / 50);
	Point pCen2(w * 31 / 50, h * 19 / 50);
	Color colors[] = { cColor };
	Color colors2[] = { cColor2 }; int iCnt = 1;
	GraphicsPath gPath, gPath2;
	gPath.AddClosedCurve(pPoints, 7);
	gPath2.AddClosedCurve(pPoints2, 7);
	PathGradientBrush pGB(&gPath), pGB2(&gPath2);
	pGB.SetCenterPoint(pCen);
	pGB.SetSurroundColors(colors, &iCnt);
	pGB2.SetCenterPoint(pCen2);
	pGB2.SetSurroundColors(colors2, &iCnt);
	g.FillClosedCurve(&pGB, pPoints, 7);
	g.FillClosedCurve(&pGB2, pPoints2, 7);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Painter::Draw(HDC hDC, int iX, int iY, BOOL bTrans, COLORREF cColor) {

	if (m_hBitmap != NULL) {

		DrawPart(hDC, iX, iY, 0, 0, GetWidth(), GetHeight(), bTrans, cColor);
	}
}

// Draw some part, BOOL bTrans: transparent
void Painter::DrawPart(HDC hDC, int iDX, int iDY, int iSX, int iSY, 
	int iW, int iH, BOOL bTrans, COLORREF cColor) {

	if (m_hBitmap != NULL) {

		HDC hMemDC = CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
		if (bTrans) TransparentBlt(hDC, iDX, iDY, iW, iH, hMemDC, iSX, iSY, iW, iH, cColor);
		else BitBlt(hDC, iDX, iDY, iW, iH, hMemDC, iSX, iSY, SRCCOPY);
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
}