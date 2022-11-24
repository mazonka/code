g++ -g %1 -o %~n1_gc
cl %1 -EHsc -Fe%~n1_ms

@set PATH=%APP%\run;%APP%\cyg\bin;%APP%\run2;%windir%;%windir%\system32
call bc.bat %1
@move %~n1.exe %~n1_bc.exe

@set PATH=%APP%\run;%APP%\cyg\bin;%APP%\run2;%windir%;%windir%\system32
call dm.bat %1
@move %~n1.exe %~n1_dm.exe

@del *.map *.obj *.tds
