make -j 3
copy span.exe %APP%\app\bin\sp.exe

@if exist %APP%\run\sp.exe goto delexe

exit

:delexe
echo Old exe is in %APP%\run removing
rm %APP%\run\sp.exe
