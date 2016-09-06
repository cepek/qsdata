set DIRECTORY=..\..\build-qsdata-Desktop_Qt_5_7_0_MSVC2015_64bit-Release\qsdata\release
set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
set WINDEPLOYQT=C:\Qt\5.7\msvc2015_64\bin\windeployqt.exe
set PARAMETERS=--no-translations

%WINDEPLOYQT% %PARAMETERS% %DIRECTORY%\qsdata.exe
