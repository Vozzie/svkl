Option Explicit

Const vbForReading = 1

Select Case True
Case WScript.Arguments.Named.Exists("i") : Install
Case WScript.Arguments.Named.Exists("u") : Uninstall
Case WScript.Arguments.Named.Exists("r") : Run
Case Else WScript.Echo "Invalid arguments" & vbCrLf & vbCrLf & "Usage: svkl.vbs /i | /u | /r" & vbCrLf & vbTab & "(i)nstall | (u)ninstall | (r)un" & vbCrLf
End Select

Function FormatDate(ByVal dNow)
	Dim sMonth, sDay, sHour, sMinute, sSecond
	sMonth = CStr(Month(dNow))
	sDay = CStr(Day(dNow))
	sHour = CStr(Hour(dNow))
	sMinute = CStr(Minute(dNow))
	sSecond = CStr(Second(dNow))
	FormatDate = Year(dNow) & _
		IIf(Len(sMonth) = 1, "0" & sMonth, sMonth) & _
		IIf(Len(sDay) = 1, "0" & sDay, sDay) & _
		IIf(Len(sHour) = 1, "0" & sHour, sHour) & _
		IIf(Len(sMinute) = 1, "0" & sMinute, sMinute) & _
		IIf(Len(sSecond) = 1, "0" & sSecond, sSecond) 
End Function

Sub Install()
	CreateObject("WScript.Shell").RegWrite "HKCU\Software\Microsoft\Windows\CurrentVersion\Run\svkl", "WSCRIPT.EXE """ & WScript.ScriptFullName & """ /r", "REG_SZ"
End Sub

Function IIf(ByVal expression, ByVal trueVal, ByVal falseVal)
	If expression Then : IIf = trueVal : Else : IIf = falseVal : End If
End Function

Sub Run()
	Dim oFso, oWS, oStream
	Dim sPath, sWinDir, sHtml
	Dim dNow
	Set oFso = CreateObject("Scripting.FileSystemObject")
	Set oWS = CreateObject("WScript.Shell")
	sPath = Left(WScript.ScriptFullName, InStrRev(WScript.ScriptFullName, "\")) 
	oWS.CurrentDirectory = sPath
	If oFso.FileExists("log.txt") Then
		dNow = Now()
		Set oStream = oFso.OpenTextFile("log.txt", vbForReading)
		sHtml = oStream.ReadAll()
		oStream.Close
		Set oStream = oFso.CreateTextFile(FormatDate(dNow) & ".htm")
		oStream.Write _
		"<html>" & _
			"<head>" & _
				"<title>SVKL - " & CStr(dNow) & "</title>" & _
				"<meta charset=""UTF-8""/>" & _
				"<style>" & _
					"body{background-color:InfoBackground;color:Navy;}" & _
					"pre{background-color:#EEE;border-style:dotted;border-width:2px;padding:4px;}" & _
					"pre:empty{border-width:0px;padding:0;}" & _
				"</style>" & _
			"</head>" & _
			"<body><pre>" & sHtml & "</pre></body>" & _
		"</html>"
		oStream.Close
		oFso.DeleteFile "log.txt"
	End if
	sWinDir = oWS.ExpandEnvironmentStrings("%WINDIR%")
	If Right(sWinDir, 1) <> "\" Then sWinDir = sWinDir & "\"
	If oFso.FolderExists(sWinDir & "SysWOW64") Then
		Call oWS.Exec(sWinDir & "System32\rundll32.exe """ & sPath & "x64\svkl.dll"",Start ""log.txt""")
		Call oWS.Exec(sWinDir & "SysWOW64\rundll32.exe """ & sPath & "x86\svkl.dll"",Start ""log.txt""")
	Else
		Call oWS.Exec(sWinDir & "System32\rundll32.exe """ & sPath & "x86\svkl.dll"",Start ""log.txt""")
	End If
End Sub

Sub Uninstall()
	CreateObject("WScript.Shell").RegDelete "HKCU\Software\Microsoft\Windows\CurrentVersion\Run\svkl"
End Sub
