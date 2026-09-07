if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('additional-courses');

var COURSE_ICONS = {
  'التشفير': '🔐',
  'الأمن السيبراني': '🛡️',
  'برمجة صفحات الويب': '🌐'
};

function loadCourses() {
  api.getCourses().then(function(res) {
    var courses = res.courses || [];
    renderCourses(courses);
  }).catch(function() {
    document.getElementById('coursesGrid').innerHTML =
      '<div class="empty-state" style="grid-column:1/-1"><p>فشل تحميل الدورات</p></div>';
  });
}

function renderCourses(courses) {
  var grid = document.getElementById('coursesGrid');
  if (courses.length === 0) {
    grid.innerHTML = '<div class="empty-state" style="grid-column:1/-1"><p>لا توجد دورات إضافية حالياً</p></div>';
    return;
  }

  grid.innerHTML = courses.map(function(c) {
    var icon = COURSE_ICONS[c.title] || '📖';
    var statusBadge = '';
    var btnHtml = '';
    var lockOverlay = '';

    if (c.status === 'ENROLLED') {
      statusBadge = '<span class="course-badge badge-enrolled">مسجّل</span>';
      btnHtml = '<button class="btn btn-primary btn-sm" onclick="goToCourse(\'' + c.id + '\')">ابدأ التعلم</button>';
    } else if (c.status === 'UNLOCKED') {
      statusBadge = '<span class="course-badge badge-unlocked">متاح</span>';
      btnHtml = '<button class="btn btn-success btn-sm btn-enroll" onclick="enroll(\'' + c.id + '\')">سجّل الآن</button>';
    } else {
      statusBadge = '<span class="course-badge badge-locked">مقفل</span>';
      lockOverlay = '<div class="lock-overlay"><div class="lock-text">🔒 يُفتح بعد المستوى ' + c.unlockTier + '</div></div>';
    }

    return '<div class="course-card' + (c.status === 'LOCKED' ? ' locked' : '') + '" style="position:relative">' +
      lockOverlay +
      '<div class="course-icon">' + icon + '</div>' +
      '<div class="course-title">' + escapeHtml(c.title) + '</div>' +
      '<div class="course-desc">' + escapeHtml(c.description || '') + '</div>' +
      '<div class="course-meta">' +
        statusBadge +
        '<span class="course-badge" style="background:rgba(168,85,247,0.1);color:#a855f7;border:1px solid rgba(168,85,247,0.3)">' + escapeHtml(c.category) + '</span>' +
      '</div>' +
      '<div class="course-stats">' +
        '<span class="course-stat">📚 ' + c.lessonCount + ' دروس</span>' +
        '<span class="course-stat">🎯 ' + (c.lessonCount * 3) + ' تحدٍ</span>' +
      '</div>' +
      btnHtml +
    '</div>';
  }).join('');
}

function enroll(courseId) {
  api.enrollCourse(courseId).then(function(res) {
    toast.success('تم التسجيل بنجاح!');
    loadCourses();
  }).catch(function(err) {
    toast.error(err.message || 'فشل التسجيل');
  });
}

function goToCourse(courseId) {
  window.location.href = 'course-roadmap.html?course=' + courseId;
}

loadCourses();

if('serviceWorker' in navigator){navigator.serviceWorker.register('/sw.js').catch(function(){})}
