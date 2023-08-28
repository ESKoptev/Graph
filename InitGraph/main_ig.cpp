#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <TCHAR.h>
#include "graph.h"




#define MAXIT 20

int fern(HDC hdc)
{
	double x, y, newx, newy;
	int   rand_num;
	while (!isKey(VK_ESCAPE))
	{
		x = (double)rand() / 50000.0;
		y = (double)rand() / 50000.0;
		for (int i = 0; i < MAXIT; i++)
		{
			rand_num = rand();
			if (rand_num < 0.01 * RAND_MAX)
			{
				newx = 0;
				newy = 0.16 * y;
				x = newx;
				y = newy;
			}
			else
				if (rand_num < 0.86 * RAND_MAX)
				{
					newx = (0.85 * x) + (0.04 * y);
					newy = (-0.04 * x) + (0.85 * y) + 1.6;
					x = newx;
					y = newy;
				}
				else
					if (rand_num < 0.93 * RAND_MAX)
					{
						newx = (0.2 * x) - (0.26 * y);
						newy = (0.23 * x) + (0.22 * y) + 1.6;
						x = newx;
						y = newy;
					}
					else
					{
						newx = (-0.15 * x) + (0.28 * y);
						newy = (0.26 * x) + (0.24 * y) + 0.44;
						x = newx;
						y = newy;
					}
		}  /* for loop */
		if ((x >= -5.0) && (x <= 5.0) && (y >= 0.0) && (y <= 10.0))
			SetPixel(hdc, 683 + int(x*76.8), 768 - int(y*76.8), RGB(255, 255, 255));
	}  /* while (!isKey(VK_ESCAPE)) */
	return 0;
}

typedef struct tagPnt
{
	double x, y;
} PNT;

#define N_ITER (17)
int dragon(HDC hdc)
{
	const int Npnts = int(pow(2, N_ITER));
	static PNT pnts[4*1024*1024 * 3] = { 0 };
	static PNT pnts1[4*1024*1024 * 3] = { 0 };
	pnts[0].x = 150;
	pnts[0].y = 250;

	pnts[1].x = 300;
	pnts[1].y = 100;

	pnts[2].x = 450;
	pnts[2].y = 250;

	int lim = 2;

	for (int cnt = 0; cnt < N_ITER; ++cnt)
	{
		for (int cnt1 = 0; cnt1 <= lim; ++cnt1)
		{
			pnts1[lim - cnt1].x = -0.5 * pnts[cnt1].x - 0.5 * pnts[cnt1].y + 500;
			pnts1[lim - cnt1].y = 0.5 * pnts[cnt1].x - 0.5 * pnts[cnt1].y + 150;
		}
		for (int cnt1 = 0; cnt1 <= lim; ++cnt1)
		{
			pnts1[lim + cnt1].x = 0.5 * pnts[cnt1].x - 0.5 * pnts[cnt1].y + 350;
			pnts1[lim + cnt1].y = 0.5 * pnts[cnt1].x + 0.5 * pnts[cnt1].y - 100;
		}
		memcpy_s(pnts, 3 * Npnts*sizeof(PNT), pnts1, 3 * Npnts*sizeof(PNT));
		lim *= 2;
	} 
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, RGB(255, 255, 255));

	LPPOINT pntsl;
	pntsl = (LPPOINT)malloc((2*Npnts+1)*sizeof(POINT));
	for (int cnt = 0; cnt < 2 * Npnts + 1; ++cnt)
	{
		pntsl[cnt].x = long(pnts[cnt].x);
		pntsl[cnt].y = long(pnts[cnt].y);
	}
	Polyline(hdc, pntsl, 2 * Npnts + 1);
/*	for (int cnt = 0; cnt < 2*Npnts; ++cnt)
	{
//		SetPixel(hdc, pnts[cnt].x, pnts[cnt].y, RGB(255, 255, 255));
		MoveToEx(hdc, int(pnts[cnt].x), int(pnts[cnt].y), NULL);
		LineTo(hdc, int(pnts[cnt + 1].x), int(pnts[cnt + 1].y));

	}*/
	free(pntsl);
	return 0;
}
int WINAPI _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{

	HDC hdc = InitGraph(0, 0, 640, 480, FULLSCREEN);

	fern(hdc);

	while (!isKey(VK_ESCAPE)) {}

	dragon(hdc);

	while (isKey(VK_ESCAPE)) {}

	while (!isKey(VK_ESCAPE)) {}

	CloseGraph();

	return 0;
}

