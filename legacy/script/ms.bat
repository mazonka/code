@set PATHO=%PATH%
@set PATH=%APP%\c7\bin;%APP%\c7\binz;%PATH%
@set INCLUDE=%APP%\c7\include
@set LIB=%APP%\c7\lib
@cl -GX -nologo %*
@set PATH=%PATHO%
