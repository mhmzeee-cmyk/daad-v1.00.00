if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('classrooms');

var _role = api.getRole();
if (_role !== 'ADMIN') {
  var btn = document.getElementById('btnAddClassroom');
  if (btn) btn.style.display = 'none';
}

var classrooms = [];

function loadClassrooms() {
  api.getClassrooms().then(function(res) {
    classrooms = Array.isArray(res) ? res : (res.classrooms || []);
    renderClassrooms();
  }).catch(function() { toast.error('فشل تحميل الفصول'); });
}

function renderClassrooms() {
  var grid = document.getElementById('classroomsGrid');
  if (classrooms.length === 0) {
    var emptyHtml = '<div class="empty-state" style="grid-column:1/-1"><div class="icon"></div><p>ليس لديك أي فصل بعد</p>';
    if (_role === 'ADMIN') {
      emptyHtml += '<button class="btn btn-primary btn-sm" style="margin-top:12px" onclick="showModal()">إنشاء فصل</button>';
    }
    emptyHtml += '</div>';
    grid.innerHTML = emptyHtml;
    return;
  }
  grid.innerHTML = classrooms.map(function(c) {
    return '<div class="classroom-card" data-id="' + escapeHtml(String(c.id || '')) + '" data-name="' + escapeHtml(c.name || '') + '" onclick="showDetail(this.dataset.id, this.dataset.name)">' +
      '<h4>' + escapeHtml(c.name || '') + '</h4>' +
      '<span class="classroom-code">' + escapeHtml(c.code || '') + '</span>' +
      '<div class="classroom-meta" style="margin-top:12px">' +
      '<span> ' + escapeHtml(String(c.studentCount || 0)) + ' طالب</span>' +
      '<span> ' + escapeHtml(String(c.averageXP || 0)) + ' XP</span>' +
      '<span> ' + escapeHtml(c.createdAt ? new Date(c.createdAt).toLocaleDateString('ar-SA') : '') + '</span>' +
      '</div><div style="margin-top:12px"><span class="badge badge-info">' + escapeHtml(c.level || 'BEGINNER') + '</span></div></div>';
  }).join('');
}

function showModal() {
  document.getElementById('createModal').classList.add('active');
}

function closeModal() {
  document.getElementById('createModal').classList.remove('active');
}

document.getElementById('createForm').addEventListener('submit', function(e) {
  e.preventDefault();
  api.request('/api/v1/analytics/classrooms', {
    method: 'POST',
    body: JSON.stringify({
      name: document.getElementById('className').value,
      subject: document.getElementById('classSubject').value,
      level: document.getElementById('classLevel').value
    })
  }).then(function() {
    toast.success('تم إنشاء الفصل بنجاح');
    closeModal();
    loadClassrooms();
  }).catch(function(err) { toast.error(err.message); });
});

function showDetail(id, name) {
  document.getElementById('detailTitle').textContent = name;
  var content = document.getElementById('detailContent');
  content.innerHTML = '<div class="loading"><div class="spinner"></div></div>';
  document.getElementById('detailModal').classList.add('active');

  api.getClassroomProgress(id).then(function(res) {
    var data = res.progress || res || {};
    content.innerHTML = '<div style="margin-bottom:16px"><h4>تقدم الطلاب</h4>' +
      '<div class="progress-bar" style="margin-top:8px"><div class="progress-fill green" style="width:' + escapeHtml(String(data.averageProgress || 0)) + '%"></div></div>' +
      '<p style="color:var(--text-muted);font-size:13px;margin-top:4px">' + escapeHtml(String(data.averageProgress || 0)) + '% متوسط التقدم</p></div>' +
      '<div style="margin-bottom:16px"><h4>إحصائيات</h4>' +
      '<p> عدد الطلاب: ' + escapeHtml(String(data.studentCount || 0)) + '</p>' +
      '<p> متوسط النقاط: ' + escapeHtml(String(data.averageXP || 0)) + '</p>' +
      '<p> التحديات المكتملة: ' + escapeHtml(String(data.completedChallenges || 0)) + '</p></div>';
  }).catch(function() {
    content.innerHTML = '<p style="color:var(--text-muted)">لا يمكن تحميل التفاصيل</p>';
  });
}

function closeDetailModal() {
  document.getElementById('detailModal').classList.remove('active');
}

loadClassrooms();