#pragma once
#ifndef __DEBUG_HEADER__
#define __DEBUG_HEADER__

#ifdef DEBUG

#include <windows.h>

VOID __OutputLastError(LPTSTR lpszFunction);
#define OutputDebug(string) OutputDebugString(string)
#define OutputLastError(method) __OutputLastError((method))
#define ELSE_OutputDebug(string) else OutputDebugString(string)
#define ELSE_OutputLastError(method) else __OutputLastError((method))

#else//!DEBUG

#define OutputDebug(string) 
#define OutputLastError(method)
#define ELSE_OutputDebug(string) 
#define ELSE_OutputLastError(method)

#endif//DEBUG

#endif//__DEBUG_HEADER__
