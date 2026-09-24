if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
}

Sidebar.init('reports');

function loadClassroomsForSelects() {
  api.getClassrooms().then(function(res) {
    var classrooms = Array.isArray(res) ? res : (res.classrooms || []);
    var selects = ['attendanceClassroom', 'progressClassroom'];
    selects.forEach(function(id) {
      var select = document.getElementById(id);
      select.innerHTML = '<option value="">اختر الفصل</option>';
      classrooms.forEach(function(c) {
        var opt = document.createElement('option');
        opt.value = c.id;
        opt.textContent = c.name;
        select.appendChild(opt);
      });
    });
  }).catch(function(err) {
    console.error('Failed to load classrooms:', err);
  });
}

function loadAttendance() {
  var classroomId = document.getElementById('attendanceClassroom').value;
  if (!classroomId) {
    document.getElementById('attendanceReport').innerHTML = '<div class="empty-state"><div class="icon"></div><p>اختر فصلاً لعرض تقرير الحضور</p></div>';
    document.getElementById('markAttendanceBtn').style.display = 'none';
    return;
  }
  document.getElementById('markAttendanceBtn').style.display = 'inline-block';
  var container = document.getElementById('attendanceReport');
  container.innerHTML = '<div class="loading"><div class="spinner"></div></div>';
  api.getAttendance(classroomId).then(function(res) {
    var data = res.attendance || [];
    if (!data || data.length === 0) {
      container.innerHTML = '<div class="empty-state"><p>لا توجد بيانات حضور</p></div>';
      return;
    }
    container.innerHTML = '<div class="table-wrapper"><table><thead><tr><th>التاريخ</th><th>الحاضرون</th><th>الغياب</th><th>النسبة</th></tr></thead><tbody>' +
      data.map(function(d) {
        var badgeClass = d.percentage >= 80 ? 'badge-success' : d.percentage >= 50 ? 'badge-warning' : 'badge-danger';
        return '<tr><td>' + escapeHtml(d.date) + '</td><td>' + d.present + '</td><td>' + d.absent + '</td><td><span class="badge ' + badgeClass + '">' + d.percentage + '%</span></td></tr>';
      }).join('') + '</tbody></table></div>';
  }).catch(function() {
    container.innerHTML = '<p style="color:var(--text-muted)">لا يمكن تحميل البيانات</p>';
  });
}

function showMarkAttendance() {
  var classroomId = document.getElementById('attendanceClassroom').value;
  if (!classroomId) return;
  var panel = document.getElementById('markAttendancePanel');
  panel.style.display = 'block';
  panel.innerHTML = '<div class="loading"><div class="spinner"></div></div>';

  api.request('/api/v1/auth/classrooms/' + classroomId + '/students').then(function(res) {
    var students = res.students || res || [];
    if (students.length === 0) {
      panel.innerHTML = '<p>لا يوجد طلاب في هذا الفصل</p>';
      return;
    }
    var html = '<h4 style="margin-bottom:12px">تسجيل حضور ' + new Date().toLocaleDateString('ar-SA') + '</h4>';
    html += '<div style="display:flex;gap:8px;margin-bottom:12px">';
    html += '<button class="btn btn-primary btn-sm" onclick="markAllPresent()">الكل حاضر</button>';
    html += '<button class="btn btn-secondary btn-sm" onclick="markAllAbsent()">الكل غائب</button>';
    html += '</div>';
    html += '<div id="attendanceStudentsList">';
    students.forEach(function(s) {
      html += '<div style="display:flex;align-items:center;justify-content:space-between;padding:8px 0;border-bottom:1px solid var(--border)">';
      html += '<span>' + escapeHtml(s.name) + '</span>';
      html += '<div style="display:flex;gap:6px">';
      html += '<button class="btn btn-sm btn-primary attendance-btn" data-student="' + s.id + '" data-status="PRESENT" onclick="toggleAttendance(this)">حاضر</button>';
      html += '<button class="btn btn-sm btn-secondary attendance-btn" data-student="' + s.id + '" data-status="ABSENT" onclick="toggleAttendance(this)">غائب</button>';
      html += '</div></div>';
    });
    html += '</div>';
    html += '<button class="btn btn-success" style="margin-top:12px" onclick="submitAttendance(\'' + classroomId + '\')">حفظ الحضور</button>';
    panel.innerHTML = html;
  }).catch(function() {
    panel.innerHTML = '<p style="color:var(--text-muted)">فشل تحميل قائمة الطلاب</p>';
  });
}

var attendanceState = {};

function toggleAttendance(btn) {
  var studentId = btn.getAttribute('data-student');
  var status = btn.getAttribute('data-status');
  attendanceState[studentId] = status;
  var siblings = btn.parentElement.querySelectorAll('.attendance-btn');
  siblings.forEach(function(b) { b.style.opacity = '0.5'; });
  btn.style.opacity = '1';
}

function markAllPresent() {
  document.querySelectorAll('.attendance-btn[data-status="PRESENT"]').forEach(function(b) {
    toggleAttendance(b);
  });
}

function markAllAbsent() {
  document.querySelectorAll('.attendance-btn[data-status="ABSENT"]').forEach(function(b) {
    toggleAttendance(b);
  });
}

function submitAttendance(classroomId) {
  var records = Object.entries(attendanceState).map(function(entry) {
    return { studentId: entry[0], status: entry[1] };
  });
  if (records.length === 0) {
    toast.error('اختر حالة حضور للطلاب أولاً');
    return;
  }
  api.markAttendance({ classroomId: classroomId, records: records }).then(function(res) {
    toast.success('تم تسجيل الحضور لـ ' + res.count + ' طالب');
    document.getElementById('markAttendancePanel').style.display = 'none';
    attendanceState = {};
    loadAttendance();
  }).catch(function() {
    toast.error('فشل تسجيل الحضور');
  });
}

function loadProgress() {
  var classroomId = document.getElementById('progressClassroom').value;
  if (!classroomId) return;
  var container = document.getElementById('progressReport');
  container.innerHTML = '<div class="loading"><div class="spinner"></div></div>';
  api.getClassroomProgress(classroomId).then(function(res) {
    var data = res.progress || res || {};
    container.innerHTML = '<div style="margin-bottom:16px"><div style="display:flex;justify-content:space-between;margin-bottom:8px"><span>متوسط التقدم</span><span>' + escapeHtml(String(data.averageProgress || 0)) + '%</span></div>' +
      '<div class="progress-bar"><div class="progress-fill green" style="width:' + (data.averageProgress || 0) + '%"></div></div></div>' +
      '<div style="margin-bottom:16px"><div style="display:flex;justify-content:space-between;margin-bottom:8px"><span>متوسط النقاط</span><span>' + escapeHtml(String(data.averageXP || 0)) + ' XP</span></div>' +
      '<div class="progress-bar"><div class="progress-fill blue" style="width:' + Math.min((data.averageXP || 0) / 10, 100) + '%"></div></div></div>' +
      '<p style="color:var(--text-muted);font-size:13px"> عدد الطلاب: ' + escapeHtml(String(data.studentCount || 0)) + '</p>';
  }).catch(function() {
    container.innerHTML = '<p style="color:var(--text-muted)">لا يمكن تحميل البيانات</p>';
  });
}

function loadSecurityAlerts() {
  api.request('/api/v1/analytics/security-alerts').then(function(res) {
    var alerts = Array.isArray(res) ? res : (res.alerts || []);
    var container = document.getElementById('securityAlerts');
    if (!alerts || alerts.length === 0) {
      container.innerHTML = '<div class="empty-state"><div class="icon"></div><p>لا توجد تنبيهات أمان</p></div>';
      return;
    }
    container.innerHTML = alerts.map(function(a) {
      return '<div style="display:flex;justify-content:space-between;padding:12px 0;border-bottom:1px solid var(--border)"><div><strong>' + escapeHtml(a.type) + '</strong><p style="color:var(--text-muted);font-size:13px">' + escapeHtml(a.description) + '</p></div><span class="badge badge-danger">' + escapeHtml(a.severity) + '</span></div>';
    }).join('');
  }).catch(function() {});
}

loadClassroomsForSelects();
loadSecurityAlerts();
