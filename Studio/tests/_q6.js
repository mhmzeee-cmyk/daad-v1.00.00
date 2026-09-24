const http = require('http');

function api(method, path, body, token) {
  return new Promise((resolve) => {
    const data = body ? JSON.stringify(body) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'Test/1.0' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, hostname: 'localhost', port: 3000, path, headers, timeout: 10000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => { try { resolve({ s: res.statusCode, d: JSON.parse(buf) }); } catch(e) { resolve({ s: res.statusCode, d: null }); } });
    });
    r.on('error', () => resolve({ s: 0, d: null }));
    r.on('timeout', () => { r.destroy(); resolve({ s: 0, d: null }); });
    if (data) r.write(data);
    r.end();
  });
}

async function main() {
  // Login as teacher_1_1
  const loginRes = await api('POST', '/api/v1/auth/login', { username: 'teacher_1_1@test.com', password: 'Test1234!' });
  if (!loginRes.d?.accessToken) { console.log('Login failed:', loginRes.s, loginRes.d); return; }
  const token = loginRes.d.accessToken;
  console.log('Logged in as teacher_1_1');

  // Get classrooms
  const classRes = await api('GET', '/api/v1/analytics/classrooms', null, token);
  console.log('\nClassrooms:', classRes.s);
  const classrooms = classRes.d?.classrooms || [];
  console.log('Count:', classrooms.length);
  if (classrooms.length === 0) { console.log('No classrooms!'); return; }

  const classroomId = classrooms[0].id;
  console.log('Using classroom:', classroomId, classrooms[0].name);

  // Get students in classroom
  const studRes = await api('GET', '/api/v1/auth/classrooms/' + classroomId + '/students', null, token);
  console.log('\nStudents:', studRes.s);
  const students = studRes.d?.students || studRes.d || [];
  console.log('Count:', Array.isArray(students) ? students.length : 'N/A');

  // Test attendance history (empty)
  const histRes = await api('GET', '/api/v1/analytics/attendance-history?classroomId=' + classroomId, null, token);
  console.log('\nAttendance history:', histRes.s);
  console.log('Data:', JSON.stringify(histRes.d));

  // Mark attendance for first 3 students
  if (Array.isArray(students) && students.length >= 3) {
    const records = students.slice(0, 3).map((s, i) => ({
      studentId: s.id,
      status: i === 2 ? 'ABSENT' : 'PRESENT'
    }));
    console.log('\nMarking attendance for', records.length, 'students...');
    const markRes = await api('POST', '/api/v1/analytics/attendance', {
      classroomId,
      records
    }, token);
    console.log('Mark result:', markRes.s, JSON.stringify(markRes.d));
  }

  // Check history again
  const histRes2 = await api('GET', '/api/v1/analytics/attendance-history?classroomId=' + classroomId, null, token);
  console.log('\nAttendance history after marking:', histRes2.s);
  console.log('Data:', JSON.stringify(histRes2.d));
}

main();
