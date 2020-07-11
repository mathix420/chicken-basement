import time
import requests

IP_ADR = 'http://192.168.5.65'

while 1:
    time.sleep(1)
    try:
        requests.get(IP_ADR + '/H')
    except:
        pass
    time.sleep(1)
    try:
        requests.get(IP_ADR + '/L')
    except:
        pass
