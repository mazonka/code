call C:\ap12\run\initap.bat

set c16=C:\ap12\c16w
set PATH=%c16%\bin;%PATH%
set INCLUDE=%c16%\include
set LIB=%c16%\lib

set c16sdk=%c16%\sdk
set PATH=%c16sdk%\bin;%PATH%
set INCLUDE=%INCLUDE%;%c16sdk%\include
set LIB=%LIB%;%c16sdk%\lib

set c16mfc=%c16%\mfc
set INCLUDE=%INCLUDE%;%c16mfc%\include
set LIB=%LIB%;%c16mfc%\lib

c:\ap12\far\far