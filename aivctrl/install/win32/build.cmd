@echo off

rem Qt installation folder where find bin (Change to your Qt folder).
set QTDIR=C:\Qt\Qt5.8.0\5.8\mingw53_32

rem The QtUPnP folder (Change to your QtUPnP folder).
set DEVELDIR=C:\Users\%USERNAME%\Documents\Development

rem All files and folders for InstallShield or to launch AIVCtrl
set TARGET=build

rem Product name.
set PRODUCT=aivctrl

rem Compilation results.
set BINARIES=%DEVELDIR%\build-aivctrl-Desktop_Qt_5_8_0_MinGW_32bit-Release

if not exist %TARGET% goto nodel
echo ************* Remove old folder
echo %TARGET%
rmdir %TARGET% /S /Q

:nodel

md %TARGET%

echo ************* Binary files
copy %BINARIES%\%PRODUCT%\release\%PRODUCT%.exe %TARGET%
copy %BINARIES%\upnp\release\QtUPnP.dll %TARGET%
xcopy ..\..\icons\%PRODUCT%48.png %TARGET%

echo ************* Translation
xcopy ..\..\languages\aivctrl_??.qm "%TARGET%\languages" /Y /I

echo ************* Qt Images plugins
md "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qgif.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qicns.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qico.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qjpeg.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qsvg.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qtga.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qtiff.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qwbmp.dll" "%TARGET%\imageformats"
copy "%QTDIR%\plugins\imageformats\qwebp.dll" "%TARGET%\imageformats"

echo ************* Qt Bearer plugins
md "%TARGET%\bearer"
copy "%QTDIR%\plugins\bearer\qgenericbearer.dll" "%TARGET%\bearer"
copy "%QTDIR%\plugins\bearer\qnativewifibearer.dll" "%TARGET%\bearer"

echo ************* Qt platforms
md "%TARGET%\platforms"
copy "%QTDIR%\plugins\platforms\qminimal.dll" "%TARGET%\platforms"
copy "%QTDIR%\plugins\platforms\qoffscreen.dll" "%TARGET%\platforms"
copy "%QTDIR%\plugins\platforms\qwindows.dll" "%TARGET%\platforms"

echo ************* C++ mingw files
copy "%QTDIR%\bin\libgcc_s_dw2-1.dll" %TARGET%
copy "%QTDIR%\bin\libstdc++-6.dll" %TARGET%
copy "%QTDIR%\bin\libwinpthread-1.dll" %TARGET%

echo ************* Qt binary files
copy %QTDIR%\bin\qt5core.dll %TARGET%
copy %QTDIR%\bin\qt5gui.dll %TARGET%
copy %QTDIR%\bin\qt5xml.dll %TARGET%
copy %QTDIR%\bin\qt5network.dll %TARGET%
copy %QTDIR%\bin\qt5widgets.dll %TARGET%
copy %QTDIR%\bin\linguist.exe %TARGET%
pause

:end

