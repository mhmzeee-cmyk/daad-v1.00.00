#!/bin/bash
# F3-01 focused auth cycle (cookies in jars, HttpOnly lines start with #)
B=http://127.0.0.1:3200
J=/tmp/dhad-phase4-5/server/AC.txt; rm -f $J
pass=0; fail=0
chk() { if [ "$2" = "$3" ]; then echo "PASS $1 ($2)"; pass=$((pass+1)); else echo "FAIL $1 got=$2 want=$3"; fail=$((fail+1)); fi; }
SID=cmtq8ta6m00001gjk0whpl4ju
R=$(curl -s -c $J -o /dev/null -w "%{http_code}" -X POST $B/api/v1/auth/register -H 'Content-Type: application/json' -d "{\"name\":\"P45\",\"email\":\"p45@example.com\",\"password\":\"TestPass123!\",\"role\":\"STUDENT\",\"schoolId\":\"$SID\"}"); chk "register" $R 201
R=$(curl -s -c $J -o /dev/null -w "%{http_code}" -X POST $B/api/v1/auth/login -H 'Content-Type: application/json' -d '{"username":"p45@example.com","password":"Wrong!"}'); chk "invalid-password" $R 401
R=$(curl -s -c $J -o /dev/null -w "%{http_code}" -X POST $B/api/v1/auth/login -H 'Content-Type: application/json' -d '{"username":"p45@example.com","password":"TestPass123!"}'); chk "login" $R 200
R=$(curl -s -c $J -b $J -o /dev/null -w "%{http_code}" $B/api/v1/student/profile); chk "protected-with-cookie" $R 200
CSRF=$(curl -s -c $J -b $J $B/api/v1/csrf-token | python3 -c "import sys,json;print(json.load(sys.stdin).get('csrfToken',''))")
R=$(curl -s -c $J -b $J -o /tmp/dhad-phase4-5/logs/r1.json -w "%{http_code}" -X POST $B/api/v1/auth/refresh -H "X-CSRF-Token: $CSRF"); chk "first-refresh" $R 200
R=$(curl -s -c $J -b $J -o /tmp/dhad-phase4-5/logs/r2.json -w "%{http_code}" -X POST $B/api/v1/auth/refresh -H "X-CSRF-Token: $CSRF"); chk "second-refresh" $R 200
R=$(curl -s -c $J -b $J -o /dev/null -w "%{http_code}" $B/api/v1/student/profile); chk "protected-after-rotation" $R 200
R=$(curl -s -c $J -b $J -o /dev/null -w "%{http_code}" -X POST $B/api/v1/auth/logout -H "X-CSRF-Token: $CSRF"); chk "logout" $R 200
OLDJ=/tmp/dhad-phase4-5/server/OLD.txt; cp $J $OLDJ 2>/dev/null
R=$(curl -s -b $OLDJ -o /dev/null -w "%{http_code}" -X POST $B/api/v1/auth/refresh -H "X-CSRF-Token: $CSRF"); chk "revoked-refresh-rejected" $R 401
R=$(curl -s -o /dev/null -w "%{http_code}" $B/api/v1/student/profile); chk "missing-cookie" $R 401
echo "=== pass=$pass fail=$fail ==="
