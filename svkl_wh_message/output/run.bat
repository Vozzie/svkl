@echo off

REM rundll32.exe svkl.dll,Start clipboard "filename"
REM clipboard = 1 on, 0 off

START C:\Windows\System32\rundll32.exe %CD%\x64\svkl.dll,Start "%CD%\log.txt"
START C:\Windows\SysWoW64\rundll32.exe %CD%\x86\svkl.dll,Start "%CD%\log.txt"

