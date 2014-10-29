
#include "debug.h"

#ifdef DEBUG

VOID __OutputLastError(LPTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 54) * sizeof(TCHAR)); 
#if defined(UNICODE) || defined(_UNICODE)
	wsprintf((LPTSTR)lpDisplayBuf, L"%ls failed with error %d (0x%08X): %ls", lpszFunction, dw, dw, lpMsgBuf); 
#else
    sprintf(LPTSTR)lpDisplayBuf, "%s failed with error %d (0x%08X): %s", lpszFunction, dw, dw, lpMsgBuf); 
#endif
	MessageBox(NULL, lpDisplayBuf, TEXT("Error"), MB_ICONERROR);
	OutputDebugString(lpDisplayBuf);
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

#else

void _(void){}

#endif
