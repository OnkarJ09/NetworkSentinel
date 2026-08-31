with open(r'C:\home\pcp\projects\IoT\ESP\NetworkSentinel\SentinelWeb.cpp', 'r') as f:
    content = f.read()
idx = content.find('broadcastWiFi();')
if idx >= 0:
    print('FOUND', idx)
    print(repr(content[idx:idx+100]))
else:
    print('NOT FOUND')