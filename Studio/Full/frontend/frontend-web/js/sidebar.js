/* Developer: محمد محمود الحموز | Dhad Studio */
// ═══════════════════════════════════════════════════════════════════════════════
// Sidebar Navigation - Single source of truth for all pages
// ═══════════════════════════════════════════════════════════════════════════════

var Sidebar = (function() {
  var currentPage = window.location.pathname.split('/').pop().replace('.html', '');

  // Pages that are main dashboards (no back button needed)
  var mainPages = ['student-dashboard', 'teacher-dashboard', 'login', 'register', 'index', 'welcome'];

  // Auto-inject back button into page-header
  function injectBackButton() {
    if (mainPages.indexOf(currentPage) !== -1) return;

    var pageHeader = document.querySelector('.page-header');
    if (!pageHeader) return;

    // Check if back button already exists
    if (pageHeader.querySelector('.back-btn')) return;

    var backBtn = document.createElement('a');
    backBtn.href = '#';
    backBtn.className = 'back-btn';
    backBtn.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 18 15 12 9 6"/></svg> رجوع';
    backBtn.onclick = function(e) {
      e.preventDefault();
      window.history.go(-1);
    };

    // Wrap h1 in page-header with page-header-left if needed
    var h1 = pageHeader.querySelector('h1');
    if (h1 && !h1.parentElement.classList.contains('page-header-left')) {
      var wrapper = document.createElement('div');
      wrapper.className = 'page-header-left';
      pageHeader.insertBefore(wrapper, h1);
      wrapper.appendChild(backBtn);
      wrapper.appendChild(h1);
    } else if (h1) {
      pageHeader.insertBefore(backBtn, h1);
    } else {
      pageHeader.insertBefore(backBtn, pageHeader.firstChild);
    }
  }

  var icons = {
    'grid': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="7" height="7"/><rect x="14" y="3" width="7" height="7"/><rect x="14" y="14" width="7" height="7"/><rect x="3" y="14" width="7" height="7"/></svg>',
    'home': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 9l9-7 9 7v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z"/><polyline points="9 22 9 12 15 12 15 22"/></svg>',
    'users': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"/><circle cx="9" cy="7" r="4"/><path d="M23 21v-2a4 4 0 0 0-3-3.87"/><path d="M16 3.13a4 4 0 0 1 0 7.75"/></svg>',
    'file': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg>',
    'star': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"/></svg>',
    'chart': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="18" y1="20" x2="18" y2="10"/><line x1="12" y1="20" x2="12" y2="4"/><line x1="6" y1="20" x2="6" y2="14"/></svg>',
    'book': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 19.5A2.5 2.5 0 0 1 6.5 17H20"/><path d="M6.5 2H20v20H6.5A2.5 2.5 0 0 1 4 19.5v-15A2.5 2.5 0 0 1 6.5 2z"/></svg>',
    'target': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><circle cx="12" cy="12" r="6"/><circle cx="12" cy="12" r="2"/></svg>',
    'graduation': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M22 10v6M2 10l10-5 10 5-10 5z"/><path d="M6 12v5c3 3 9 3 12 0v-5"/></svg>',
    'map': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="1 6 1 22 8 18 16 22 23 18 23 2 16 6 8 2 1 6"/><line x1="8" y1="2" x2="8" y2="18"/><line x1="16" y1="6" x2="16" y2="22"/></svg>',
    'user': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"/><circle cx="12" cy="7" r="4"/></svg>',
    'award': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="8" r="7"/><polyline points="8.21 13.89 7 23 12 20 17 23 15.79 13.88"/></svg>',
    'settings': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>',
    'logout': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4"/><polyline points="16 17 21 12 16 7"/><line x1="21" y1="12" x2="9" y2="12"/></svg>',
    'code': '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="4 17 10 11 4 5"/><line x1="12" y1="19" x2="20" y2="19"/></svg>'
  };

  var teacherLinks = [
    { file: 'teacher-dashboard.html', label: 'لوحة التحكم', icon: 'grid' },
    { file: 'classrooms.html', label: 'الفصول الدراسية', icon: 'home' },
    { file: 'students.html', label: 'الطلاب', icon: 'users' },
    { file: 'assessments.html', label: 'الامتحانات', icon: 'file' },
    { file: 'leaderboard.html', label: 'لوحة المتصدرين', icon: 'star' },
    { file: 'reports.html', label: 'التقارير', icon: 'chart' },
    { file: '../book.html', label: 'الكتاب', icon: 'book' },
    { file: 'dhad-editor.html', label: 'محرر ض', icon: 'code' },
  ];

  var studentLinks = [
    { file: 'student-dashboard.html', label: 'لوحة التحكم', icon: 'grid' },
    { file: 'challenges.html', label: 'التحديات', icon: 'target' },
    { file: 'additional-courses.html', label: 'الدورات الإضافية', icon: 'graduation' },
    { file: 'assessments.html', label: 'الاختبارات', icon: 'file' },
    { file: 'leaderboard.html', label: 'لوحة المتصدرين', icon: 'star' },
    { file: 'roadmap.html', label: 'خارطة الطريق', icon: 'map' },
    { file: '../book.html', label: 'الكتاب', icon: 'book' },
    { file: 'dhad-editor.html', label: 'محرر ض', icon: 'code' },
    { file: 'profile.html', label: 'الملف الشخصي', icon: 'user' },
    { file: 'achievements.html', label: 'الإنجازات', icon: 'award' },
  ];

  function render(role, activePage) {
    var isTeacher = (role === 'TEACHER' || role === 'ADMIN');
    var links = isTeacher ? teacherLinks : studentLinks;
    var versionLabel = isTeacher ? 'v2.0.0 - لوحة المعلم' : 'v2.0.0';

    var html = '<div class="sidebar-logo"><h2>ض ستوديو</h2><span class="version">' + versionLabel + '</span></div>';
    html += '<nav class="sidebar-nav">';

    links.forEach(function(link) {
      var pageName = link.file.replace('.html', '').replace('../', '');
      var isActive = (activePage === pageName || currentPage === pageName) ? ' class="active"' : '';
      html += '<a href="' + link.file + '"' + isActive + '>' + icons[link.icon] + ' ' + link.label + '</a>';
    });

    html += '<a href="settings.html">' + icons.settings + ' الإعدادات</a>';
    html += '<a href="#" onclick="api.logout()">' + icons.logout + ' تسجيل الخروج</a>';
    html += '</nav>';

    return html;
  }

  function init(activePage) {
    var el = document.getElementById('sidebar');
    if (!el) return;

    var user = null;
    try {
      var raw = localStorage.getItem('dhad_user');
      user = raw ? JSON.parse(raw) : null;
    } catch(e) {}
    var role = (user && user.role) || 'STUDENT';

    el.innerHTML = render(role, activePage);

    // Inject back button for sub-pages
    injectBackButton();
  }

  return { render: render, init: init };
})();
