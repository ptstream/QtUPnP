@echo off

rem Qt installation folder.
set QTDIR=C:\Qt\Qt5.12.3\5.12.3\mingw73_%PLATEFORM%

rem The QtUPnP folder.
set DEVELDIR=C:\Users\%USERNAME%\Documents\Development

rem Compilation results.
set BINARIES=%DEVELDIR%\build-aivctrl-Desktop_Qt_5_12_3_MinGW_%PLATEFORM%_bit-Release

call build-Qt5123-Mingw73-internal.cmd