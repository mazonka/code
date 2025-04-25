yt-dlp.exe -S "height:480,abr:96" --write-subs -R 1 %*
::yt-dlp.exe -S "height:480" %*
::yt-dlp.exe -S "height:480" -f "bv*" %*
:: only audio
::yt-dlp.exe -S "height:480,abr:96" -R 1 -x %*
