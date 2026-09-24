import os, signal, time
tags = ('http.server 8100', 'server/app/src/index')
me = os.getpid()
parent = os.getppid()
killed = []
for pid in filter(str.isdigit, os.listdir('/proc')):
    p = int(pid)
    if p in (me, parent):
        continue
    try:
        with open(f'/proc/{p}/cmdline', 'rb') as f:
            cmd = f.read().replace(b'\0', b' ').decode()
    except Exception:
        continue
    if any(t in cmd for t in tags):
        try:
            os.kill(p, signal.SIGTERM)
            killed.append((p, cmd[:70]))
        except Exception as e:
            print('fail', p, e)
print('killed:', killed)
