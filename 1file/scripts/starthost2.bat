:: netsh wlan set hostednetwork mode=allow ssid="om" key="12345678" 
netsh wlan start hostednetwork
pause
:: netsh wlan show hostednetwork security
