// Theme loader (originally in <head> to prevent FOUC)
try{var t=localStorage.getItem('dhad_theme')||'light';document.documentElement.setAttribute('data-theme',t);}catch(e){}

// Auth and role guard
if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('teacher-dashboard');

var _role = api.getRole();
var _user = api.getUser();
if (_user) {
  document.getElementById('userEmail').textContent = _user.email || '';
  document.getElementById('userAvatar').textContent = (_user.name || 'M')[0].toUpperCase();
}
if (_role === 'STUDENT') {
  var _adminBtns = document.querySelectorAll('[data-admin-only]');
  for (var i = 0; i < _adminBtns.length; i++) _adminBtns[i].style.display = 'none';
}

// Welcome banner for first-time login this session
(function() {
  var banner = document.getElementById('welcomeBanner');
  var title = document.getElementById('welcomeTitle');
  var subtitle = document.getElementById('welcomeSubtitle');
  var sessionKey = 'teacher_welcomed_' + (_user ? _user.id : '');
  var isNewSession = !sessionStorage.getItem(sessionKey);
  var lastWelcome = localStorage.getItem('teacher_last_welcome');
  var isNewUser = !lastWelcome;
  
  if (isNewSession) {
    banner.style.display = 'block';
    if (isNewUser) {
      title.textContent = 'مرحباً بك في ض ستوديو!';
      subtitle.textContent = 'هذا هو أولك الأول - تفضل باستكشاف الميزات المتاحة';
    } else {
      var hour = new Date().getHours();
      var greeting = hour < 12 ? 'صباح الخير' : hour < 18 ? 'مساء الخير' : 'مساء الخير';
      var name = (_user && _user.name) ? _user.name.split(' ')[0] : 'المعلم';
      title.textContent = greeting + ' ' + name + '!';
      subtitle.textContent = 'تفضل باستكشاف فصولك وطلابك اليوم';
    }
    sessionStorage.setItem(sessionKey, '1');
    localStorage.setItem('teacher_last_welcome', new Date().toISOString());
  }
})();

function dismissWelcome() {
  var banner = document.getElementById('welcomeBanner');
  banner.style.opacity = '0';
  banner.style.transform = 'translateY(-10px)';
  banner.style.transition = 'all 0.3s ease';
  setTimeout(function() { banner.style.display = 'none'; }, 300);
}

function loadDashboard() {
  api.getClassrooms().then(function(res) {
    var classrooms = Array.isArray(res) ? res : (res.classrooms || []);
    document.getElementById('totalClassrooms').textContent = classrooms.length || 0;
    var container = document.getElementById('classroomsList');
    if (classrooms.length === 0) {
      container.innerHTML = '<div class="empty-state"><div class="icon"></div><p>ليس لديك أي فصل بعد</p>' +
        (_role === 'ADMIN' ? '<button class="btn btn-primary btn-sm" style="margin-top:12px" onclick="showCreateClassroom()">إنشاء فصل</button>' : '') +
        '</div>';
      return;
    }
    container.innerHTML = classrooms.map(function(c) {
      return '<div class="classroom-card" style="margin-bottom:12px"><h4>' + escapeHtml(c.name) + '</h4>' +
        '<span class="classroom-code">' + escapeHtml(c.code || '') + '</span>' +
        '<div class="classroom-meta"><span>' + (c.studentCount || 0) + ' طالب</span>' +
        '<span>' + (c.averageXP || 0) + ' XP</span></div></div>';
    }).join('');
  }).catch(function() { toast.error('فشل تحميل الفصول'); });

  var lbPromise = api.getLeaderboard();
  lbPromise.then(function(lbRes) {
    var leaderboard = lbRes.leaderboard || lbRes || [];
    var container = document.getElementById('topStudents');
    if (leaderboard.length === 0) {
      container.innerHTML = '<div class="empty-state"><p>لا يوجد طلاب بعد</p></div>';
      return;
    }
    container.innerHTML = leaderboard.slice(0, 5).map(function(item, i) {
      var name = item.studentName || item.name || '—';
      var xp = item.totalXP || item.xp || 0;
      return '<div class="leaderboard-item">' +
        '<span class="leaderboard-rank ' + (i < 3 ? 'rank-' + (i+1) : '') + '">' + (i + 1) + '</span>' +
        '<span class="leaderboard-name">' + escapeHtml(name) + '</span>' +
        '<span class="leaderboard-xp">' + xp + ' XP</span></div>';
    }).join('');
  }).catch(function() {
    document.getElementById('topStudents').innerHTML = '<p style="color:var(--text-muted)">لا يمكن تحميل البيانات</p>';
  });

  api.getAssessments().then(function(examsRes) {
    var exams = Array.isArray(examsRes) ? examsRes : (examsRes.assessments || []);
    document.getElementById('totalExams').textContent = exams.length || 0;
    var container = document.getElementById('recentExams');
    if (exams.length === 0) {
      container.innerHTML = '<div class="empty-state"><p>لا يوجد امتحانات بعد</p></div>';
      return;
    }
    container.innerHTML = exams.slice(0, 3).map(function(e) {
      return '<div style="display:flex;justify-content:space-between;padding:8px 0;border-bottom:1px solid var(--border)">' +
        '<span>' + escapeHtml(e.title) + '</span>' +
        '<span class="badge ' + (e.isActive ? 'badge-success' : 'badge-warning') + '">' + (e.isActive ? 'نشط' : 'مغلق') + '</span></div>';
    }).join('');
  }).catch(function() {
    document.getElementById('recentExams').innerHTML = '<p style="color:var(--text-muted)">لا يمكن تحميل البيانات</p>';
  });
}

function showCreateClassroom() {
  document.getElementById('createClassroomModal').classList.add('active');
}

function closeModal() {
  document.getElementById('createClassroomModal').classList.remove('active');
}

document.getElementById('createClassroomForm').addEventListener('submit', function(e) {
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
    loadDashboard();
  }).catch(function(err) { toast.error(err.message); });
});

loadDashboard();
