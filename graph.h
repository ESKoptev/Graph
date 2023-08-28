#pragma once
#include <Windows.h>
#include <TCHAR.h>

#define FULLSCREEN (1)

HDC InitGraph(int x, int y, int width, int height, int Mode);
int CloseGraph(void);
BOOL KeyPressed(void);
BOOL isKey(unsigned char Key);