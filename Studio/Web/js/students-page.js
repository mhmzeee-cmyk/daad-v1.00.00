if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('students');

var _sRole = api.getRole();
if (_sRole === 'STUDENT') {
  var els = document.querySelectorAll('[data-admin-only]');
  for (var i = 0; i < els.length; i++) els[i].style.display = 'none';
}
if (_sRole !== 'ADMIN') {
  var addBtn = document.getElementById('btnAddStudent');
  var bulkBtn = document.getElementById('btnBulkAdd');
  if (addBtn) addBtn.style.display = 'none';
  if (bulkBtn) bulkBtn.style.display = 'none';
}

var allStudents = [];
var allClassrooms = [];

function loadClassrooms() {
  return api.getClassrooms().then(function(data) {
    allClassrooms = Array.isArray(data) ? data : (data.classrooms || []);
    var selects = ['filterClassroom', 'studentClassroom', 'bulkClassroom'];
    selects.forEach(function(id) {
      var select = document.getElementById(id);
      if (!select) return;
      var defaultText = id === 'filterClassroom' ? 'جميع الفصول' : 'اختر الفصل';
      select.innerHTML = '<option value="">' + defaultText + '</option>';
      allClassrooms.forEach(function(c) {
        var opt = document.createElement('option');
        opt.value = c.id;
        opt.textContent = c.name;
        select.appendChild(opt);
      });
    });
  }).catch(function() {});
}

function loadStudents() {
  var classroomId = document.getElementById('filterClassroom').value;
  return api.getTeacherStudents(classroomId).then(function(data) {
    allStudents = data.students || data || [];
    renderStudents(allStudents);
  }).catch(function() { toast.error('فشل تحميل الطلاب'); });
}

function renderStudents(students) {
  var container = document.getElementById('studentsList');
  document.getElementById('studentCount').textContent = students.length + ' طالب';
  if (students.length === 0) {
    var emptyHtml = '<div class="empty-state"><div class="icon"></div><p>لا يوجد طلاب بعد</p>';
    if (_sRole === 'ADMIN') {
      emptyHtml += '<div style="display:flex;gap:8px;justify-content:center;margin-top:12px">' +
        '<button class="btn btn-primary btn-sm" onclick="showAddStudent()">+ طالب واحد</button>' +
        '<button class="btn btn-success btn-sm" onclick="showBulkAdd()">+ إضافة جماعية</button></div>';
    }
    emptyHtml += '</div>';
    container.innerHTML = emptyHtml;
    return;
  }
  container.innerHTML = students.map(function(s) {
    var canDelete = _sRole === 'ADMIN';
    var safeName = escapeHtml(s.name || '');
    var safeUsername = escapeHtml(s.username || (s.name || '').replace(/\s+/g, '').toLowerCase());
    var safeClassroom = escapeHtml((s.classroom && s.classroom.name) || '—');
    var firstChar = s.name ? escapeHtml(s.name[0]) : '?';
    return '<div class="student-card student-clickable" onclick="showStudentActivity(\'' + s.id + '\', \'' + safeName.replace(/'/g, "\\'") + '\')">' +
      '<div class="avatar-sm">' + firstChar + '</div>' +
      '<div class="student-info"><div class="student-name">' + safeName + '</div>' +
      '<div class="student-username">' + safeUsername + '</div></div>' +
      '<div style="text-align:center"><span style="font-size:12px;color:var(--text-muted)">الفصل</span>' +
      '<div style="font-size:13px">' + safeClassroom + '</div></div>' +
      '<div style="text-align:center;min-width:60px"><span class="student-xp">' + (s.xp || 0) + ' XP</span></div>' +
      '<div style="text-align:center;min-width:60px"><span style="font-size:13px">مستوى ' + (s.level || 1) + '</span></div>' +
      '<div style="text-align:center;min-width:40px"><span style="font-size:13px">' + (s.streak || 0) + '</span></div>' +
      (canDelete ? '<div><button class="btn btn-danger btn-sm" style="padding:6px 10px;font-size:12px" onclick="event.stopPropagation();deleteStudent(\'' + s.id + '\', \'' + safeName.replace(/'/g, "\\'") + '\')">حذف</button></div>' : '') +
      '</div>';
  }).join('');
}

function filterStudents() {
  var query = document.getElementById('searchInput').value.toLowerCase();
  var filtered = allStudents.filter(function(s) { return s.name.toLowerCase().indexOf(query) !== -1; });
  renderStudents(filtered);
}

function showAddStudent() {
  document.getElementById('addStudentModal').classList.add('active');
}

function showBulkAdd() {
  document.getElementById('bulkAddModal').classList.add('active');
}

function closeModal(id) {
  document.getElementById(id).classList.remove('active');
}

document.getElementById('addStudentForm').addEventListener('submit', function(e) {
  e.preventDefault();
  var name = document.getElementById('studentName').value;
  var classroomId = document.getElementById('studentClassroom').value;
  api.createStudent({ name: name, classroomId: classroomId }).then(function(result) {
    toast.success('تم إضافة الطالب بنجاح');
    showCredentials([{
      name: result.student.name,
      username: result.credentials.username,
      password: result.credentials.password
    }]);
    closeModal('addStudentModal');
    document.getElementById('addStudentForm').reset();
    loadStudents();
  }).catch(function(err) { toast.error(err.message); });
});

document.getElementById('bulkAddForm').addEventListener('submit', function(e) {
  e.preventDefault();
  var classroomId = document.getElementById('bulkClassroom').value;
  var namesText = document.getElementById('bulkNames').value;
  var names = namesText.split('\n').map(function(n) { return n.trim(); }).filter(function(n) { return n.length > 0; });
  if (names.length === 0) { toast.error('أدخل أسماء الطلاب'); return; }
  api.bulkCreateStudents(names, classroomId).then(function(result) {
    toast.success('تم إنشاء ' + result.results.successful.length + ' حساب بنجاح');
    showCredentials(result.results.successful.map(function(s) {
      return { name: s.name, username: s.username, password: result.credentials.password };
    }));
    closeModal('bulkAddModal');
    document.getElementById('bulkNames').value = '';
    loadStudents();
  }).catch(function(err) { toast.error(err.message); });
});

function showCredentials(students) {
  var container = document.getElementById('credentialsContent');
  container.innerHTML = '<p style="margin-bottom:16px;color:var(--text-muted)">شارك هذه البيانات مع الطلاب لتسجيل الدخول:</p>' +
    '<div class="credentials-box"><h4>بيانات الدخول</h4>' +
    '<table style="width:100%;border-collapse:collapse"><thead><tr>' +
    '<th style="text-align:right;padding:8px;border-bottom:1px solid var(--border)">الاسم</th>' +
    '<th style="text-align:right;padding:8px;border-bottom:1px solid var(--border)">اسم المستخدم</th>' +
    '<th style="text-align:right;padding:8px;border-bottom:1px solid var(--border)">كلمة المرور</th>' +
    '</tr></thead><tbody>' +
    students.map(function(s) {
      return '<tr><td style="padding:8px;border-bottom:1px solid var(--border)">' + escapeHtml(s.name) + '</td>' +
        '<td style="padding:8px;border-bottom:1px solid var(--border)"><code>' + escapeHtml(s.username) + '</code></td>' +
        '<td style="padding:8px;border-bottom:1px solid var(--border)"><code>' + escapeHtml(s.password) + '</code></td></tr>';
    }).join('') +
    '</tbody></table></div>';
  document.getElementById('credentialsModal').classList.add('active');
}

function deleteStudent(id, name) {
  if (!confirm('هل أنت متأكد من حذف الطالب "' + name + '"؟')) return;
  api.deleteStudent(id).then(function() {
    toast.success('تم حذف الطالب "' + name + '"');
    loadStudents();
  }).catch(function(err) { toast.error(err.message); });
}

function showExportModal() {
  document.getElementById('exportModal').classList.add('active');
}

function exportReport() {
  var classroomId = document.getElementById('filterClassroom').value;
  var checked = document.querySelectorAll('#exportFields input:checked');
  var fields = Array.from(checked).map(function(cb) { return cb.value; });
  if (fields.length === 0) { toast.error('اختر حقل واحد على الأقل'); return; }
  var fieldLabels = {
    name: 'الاسم', username: 'اسم المستخدم', email: 'البريد الإلكتروني', classroom: 'الفصل',
    xp: 'إجمالي النقاط', level: 'المستوى', streak: 'التسلسل', longestStreak: 'أطول سلسلة',
    lastActive: 'آخر نشاط', registeredAt: 'تاريخ التسجيل',
    submissions: 'إجمالي المحاولات', passed: 'المحاولات الناجحة', passedRate: 'نسبة النجاح',
    attendance: 'الحضور %', attendanceDays: 'أيام الحضور', absenceDays: 'أيام الغياب',
    achievements: 'الإنجازات', avgScore: 'متوسط الدرجات'
  };
  api.request('/api/v1/teacher/students/report/export?fields=' + fields.join(',') + (classroomId ? '&classroomId=' + classroomId : ''))
    .then(function(res) {
      if (!res.rows || res.rows.length === 0) { toast.error('لا توجد بيانات للتصدير'); return; }
      var html = '<html xmlns:o="urn:schemas-microsoft-com:office:office" xmlns:x="urn:schemas-microsoft-com:office:excel" xmlns="http://www.w3.org/TR/REC-html40"><head><meta charset="UTF-8"><!--[if gte mso 9]><xml><x:ExcelWorkbook><x:ExcelWorksheets><x:ExcelWorksheet><x:Name>كشف الطلاب</x:Name><x:WorksheetOptions><x:DisplayGridlines/></x:WorksheetOptions></x:ExcelWorksheet></x:ExcelWorksheets></x:ExcelWorkbook></xml><![endif]--></head><body><table dir="rtl" border="1" style="border-collapse:collapse;font-family:tahoma;font-size:12px">';
      html += '<thead><tr style="background:#4472c4;color:#fff">';
      fields.forEach(function(f) {
        html += '<th style="padding:6px 10px">' + (fieldLabels[f] || f) + '</th>';
      });
      html += '</tr></thead><tbody>';
      var keyMap = {
        name: 'الاسم', username: 'اسم المستخدم', email: 'البريد الإلكتروني', classroom: 'الفصل',
        xp: 'إجمالي النقاط', level: 'المستوى', streak: 'التسلسل', longestStreak: 'أطول سلسلة',
        lastActive: 'آخر نشاط', registeredAt: 'تاريخ التسجيل',
        submissions: 'إجمالي المحاولات', passed: 'المحاولات الناجحة', passedRate: 'نسبة النجاح',
        attendance: 'الحضور %', attendanceDays: 'أيام الحضور', absenceDays: 'أيام الغياب',
        achievements: 'الإنجازات', avgScore: 'متوسط الدرجات'
      };
      res.rows.forEach(function(row) {
        html += '<tr>';
        fields.forEach(function(f) {
          var key = keyMap[f];
          var val = (row[key] != null ? row[key] : '').toString();
          html += '<td style="padding:4px 8px;text-align:center">' + val.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;') + '</td>';
        });
        html += '</tr>';
      });
      html += '</tbody></table></body></html>';
      var blob = new Blob(['\uFEFF' + html], { type: 'application/vnd.ms-excel;charset=utf-8' });
      var link = document.createElement('a');
      link.href = URL.createObjectURL(blob);
      link.download = 'كشف_الطلاب_' + new Date().toISOString().split('T')[0] + '.xls';
      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
      URL.revokeObjectURL(link.href);
      toast.success('تم تحميل الكشف بنجاح');
      closeModal('exportModal');
    }).catch(function() { toast.error('فشل تصدير التقرير'); });
}

function showStudentActivity(id, name) {
  document.getElementById('activityStudentName').textContent = 'نشاط: ' + name;
  document.getElementById('activityModal').classList.add('active');
  var container = document.getElementById('activityContent');
  container.innerHTML = '<div class="loading"><div class="spinner"></div></div>';
  api.request('/api/v1/teacher/students/' + id + '/activity')
    .then(function(res) {
      if (!res.student) { container.innerHTML = '<p>لا توجد بيانات</p>'; return; }
      var s = res.student;
      var html = '<div style="display:grid;grid-template-columns:1fr 1fr;gap:12px;margin-top:16px">';
      html += '<div class="card" style="text-align:center;padding:16px"><div style="font-size:24px;font-weight:700;color:var(--accent)">' + (s.xp||0) + '</div><div style="font-size:13px;color:var(--text-muted)">إجمالي النقاط</div></div>';
      html += '<div class="card" style="text-align:center;padding:16px"><div style="font-size:24px;font-weight:700;color:var(--primary)">' + (s.level||1) + '</div><div style="font-size:13px;color:var(--text-muted)">المستوى</div></div>';
      html += '<div class="card" style="text-align:center;padding:16px"><div style="font-size:24px;font-weight:700;color:var(--secondary)">' + (s.submissions.total||0) + '</div><div style="font-size:13px;color:var(--text-muted)">إجمالي المحاولات</div></div>';
      html += '<div class="card" style="text-align:center;padding:16px"><div style="font-size:24px;font-weight:700;color:' + (s.submissions.passRate >= 70 ? 'var(--secondary)' : s.submissions.passRate >= 40 ? '#f59e0b' : '#ef4444') + '">' + (s.submissions.passRate||0) + '%</div><div style="font-size:13px;color:var(--text-muted)">نسبة النجاح</div></div>';
      html += '<div class="card" style="text-align:center;padding:16px"><div style="font-size:24px;font-weight:700;color:var(--info, #3b82f6)">' + (s.attendance||0) + '%</div><div style="font-size:13px;color:var(--text-muted)">الحضور</div></div>';
      html += '<div class="card" style="text-align:center;padding:16px"><div style="font-size:24px;font-weight:700">' + (s.streak||0) + '</div><div style="font-size:13px;color:var(--text-muted)">التسلسل الحالي</div></div>';
      html += '</div>';
      if (s.areas && s.areas.courses && s.areas.courses.length > 0) {
        html += '<h4 style="margin-top:16px">المجالات التي اشتغل فيها الطالب:</h4><div style="display:flex;flex-wrap:wrap;gap:6px;margin-top:8px">';
        s.areas.courses.forEach(function(c) {
          html += '<span class="badge badge-primary" style="padding:4px 10px">' + escapeHtml(c.title) + '</span>';
        });
        html += '</div>';
      }
      if (s.areas && s.areas.challengeTypes && s.areas.challengeTypes.length > 0) {
        html += '<h4 style="margin-top:16px">أنواع التحديات:</h4><div style="display:flex;flex-wrap:wrap;gap:6px;margin-top:8px">';
        s.areas.challengeTypes.forEach(function(t) {
          html += '<span class="badge" style="padding:4px 10px;background:var(--bg-input)">' + escapeHtml(t) + '</span>';
        });
        html += '</div>';
      }
      container.innerHTML = html;
    }).catch(function() {
      container.innerHTML = '<p style="color:var(--text-muted)">فشل تحميل بيانات النشاط</p>';
    });
}

loadClassrooms().then(function() { loadStudents(); });
