@echo off

REM add a UTF-8 header to show the chars as they should
del log.htm
echo ^<html^>^<head^> >> log.htm
echo ^<meta charset="UTF-8"/^> >> log.htm
echo ^<style^>pre{background-color:#EEE;}^</style^> >> log.htm
echo </head^>^<body^>^<pre^> >> log.htm
type log.txt >> log.htm
echo ^</pre^>^</body^>^</html^> >> log.htm