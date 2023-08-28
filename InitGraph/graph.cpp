#include "graph.h"

HWND hWnd;

HANDLE  hThread;

BOOL keys[256] = { 0 };
BOOL isInit = FALSE;

HDC hdc;

HBRUSH BlackBrush;

struct IGdata
{
	HINSTANCE hInstance;
	int x;
	int y;
	int width;
	int height;
	int Mode;
};

DWORD WINAPI MyThreadFunction(LPVOID lpParam);

HDC InitGraph(int x, int y, int width, int height, int Mode)
{
	static IGdata CurrIGdata;
	CurrIGdata.hInstance = GetModuleHandle(NULL);
	CurrIGdata.x = x;
	CurrIGdata.y = y;
	CurrIGdata.width = width;
	CurrIGdata.height = height;
	CurrIGdata.Mode = Mode;
	DWORD	dwThreadId;
	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		MyThreadFunction,       // thread function name
		&CurrIGdata,          // argument to thread function 
		0,                      // use default creation flags 
		&dwThreadId);   // returns the thread identifier
	DWORD ExitCode = 0;
	GetExitCodeThread(hThread, &ExitCode);
	if (ExitCode != STILL_ACTIVE) return NULL;
	while (!isInit) {}
	hdc = GetDC(hWnd);
	return hdc;
}

int CloseGraph(void)
{
	ReleaseDC(hWnd, hdc);
	DeleteObject(BlackBrush);
	SendMessage(hWnd, WM_CLOSE, 0, 0);
	WaitForSingleObject(
		hThread,
		INFINITE
	);
	return 0;
}

BOOL KeyPressed()
{
	for (int count = 0; count < 256; ++count)
		if (keys[count]) return TRUE;
	return FALSE;
}

BOOL isKey(unsigned char Key)
{
	return keys[Key];
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	IGdata* ThreadIGdata;
	// èìÿ áóäóùåãî êëàññà
	LPCTSTR lpzClass = TEXT("InitGraph Window Class");

	ThreadIGdata = (IGdata*)lpParam;

	// ðåãèñòðàöèÿ êëàññà
	if (!RegMyWindowClass(ThreadIGdata->hInstance, lpzClass))
		return 1;

	hWnd = CreateWindow(lpzClass, TEXT("Graph Window"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, ThreadIGdata->x, ThreadIGdata->y, ThreadIGdata->width, ThreadIGdata->height, NULL, NULL,
		ThreadIGdata->hInstance, NULL);

	// åñëè îêíî íå ñîçäàíî, îïèñàòåëü áóäåò ðàâåí 0
	if (!hWnd) return 2;

	if (ThreadIGdata->Mode == FULLSCREEN)
	{
		DEVMODE dmScreenSettings;			// Ðåæèì ðàáîòû

		memset(&dmScreenSettings, 0, sizeof(DEVMODE));	// Î÷èñòêà äëÿ õðàíåíèÿ óñòàíîâîê
		dmScreenSettings.dmSize = sizeof(DEVMODE);		// Ðàçìåð ñòðóêòóðû Devmode
		dmScreenSettings.dmPelsWidth = 640;			// Øèðèíà ýêðàíà
		dmScreenSettings.dmPelsHeight = 480;			// Âûñîòà ýêðàíà
		dmScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;	// Ðåæèì Ïèêñåëà
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings);
		//		LONG Res = ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		//		TCHAR TempStr[256];
		//		_tprintf_s(TempStr, TEXT("Ðåçóëüòàò = %i"), Res);
		//		MessageBox(NULL, TempStr, TEXT("Îøèáêà"), MB_OK);
				// Ïåðåêëþ÷åíèå â ïîëíûé ýêðàí

		SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
			dmScreenSettings.dmPelsWidth, dmScreenSettings.dmPelsHeight, SWP_SHOWWINDOW | SWP_FRAMECHANGED);

		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		SetFocus(hWnd);
		isInit = TRUE;
	}

	// öèêë ñîîáùåíèé ïðèëîæåíèÿ
	MSG msg = { 0 };    // ñòðóêòóðà ñîîáùåíèÿ
	int iGetOk = 0;   // ïåðåìåííàÿ ñîñòîÿíèÿ
	while (true) // öèêë ñîîáùåíèé
	{
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{

		} //while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		if ((iGetOk = GetMessage(&msg, NULL, 0, 0)) == -1) return 3;  // åñëè GetMessage âåðíóë îøèáêó - âûõîä
		else if (iGetOk == 0) return msg.wParam;
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	} //while (true) êîíåö öèêëà ñîîáùåíèé
	return 0;
}

////////////////////////////////////////////////////////////////////////// 
// ôóíêöèÿ ðåãèñòðàöèè êëàññà îêîí
ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
	WNDCLASS wcWindowClass = { 0 };
	// àäðåñ ô-öèè îáðàáîòêè ñîîáùåíèé
	wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
	// ñòèëü îêíà
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	// äèñêðèïòîð ýêçåìïëÿðà ïðèëîæåíèÿ
	wcWindowClass.hInstance = hInst;
	// íàçâàíèå êëàññà
	wcWindowClass.lpszClassName = lpzClassName;
	// çàãðóçêà êóðñîðà
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// çàãðóçêà öâåòà îêîí
	BlackBrush = CreateSolidBrush(0);
	wcWindowClass.hbrBackground = BlackBrush;
	return RegisterClass(&wcWindowClass); // ðåãèñòðàöèÿ êëàññà
}

////////////////////////////////////////////////////////////////////////// 
// ôóíêöèÿ îáðàáîòêè ñîîáùåíèé
LRESULT CALLBACK WndProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//	RECT win_rect;
	HDC hdc;
	PAINTSTRUCT ps;
	// âûáîðêà è îáðàáîòêà ñîîáùåíèé
	switch (message)
	{
	case WM_PAINT:
		// ðåàêöèÿ íà ñîîáùåíèå
		hdc = BeginPaint(hWnd, &ps);
//		SelectObject(hdc, GetStockObject(DC_PEN));
//		SetDCPenColor(hdc, RGB(255, 255, 255));
		//		MoveToEx(hdc, 0, 0, NULL);
		//		LineTo(hdc, 1366, 768);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		keys[wParam] = TRUE;
		break;
	case WM_KEYUP:
		keys[wParam] = FALSE;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);  // ðåàêöèÿ íà ñîîáùåíèå
		break;
	default:
		// âñå ñîîáùåíèÿ íå îáðàáîòàííûå Âàìè îáðàáîòàåò ñàìà Windows
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
