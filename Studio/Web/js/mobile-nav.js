/* Developer: محمد محمود الحموز | Dhad Studio */
// Mobile Navigation - Auto-injects into all dashboard pages
(function() {
  var user = JSON.parse(localStorage.getItem('dhad_user') || '{}');
  var role = user.role || 'STUDENT';
  var currentPage = window.location.pathname.split('/').pop().replace('.html', '');
  
  var studentLinks = [
    { id: 'student-dashboard', label: 'لوحة التحكم', file: 'student-dashboard.html' },
    { id: 'challenges', label: 'التحديات', file: 'challenges.html' },
    { id: 'assessments', label: 'الاختبارات', file: 'assessments.html' },
    { id: 'leaderboard', label: 'لوحة المتصدرين', file: 'leaderboard.html' },
    { id: 'roadmap', label: 'خارطة الطريق', file: 'roadmap.html' },
    { id: 'book', label: 'الكتاب', file: '../book.html' },
    { id: 'dhad-editor', label: 'محرر ض', file: 'dhad-editor.html' },
    { id: 'profile', label: 'الملف الشخصي', file: 'profile.html' },
    { id: 'achievements', label: 'الإنجازات', file: 'achievements.html' },
  ];
  
  var teacherLinks = [
    { id: 'teacher-dashboard', label: 'لوحة التحكم', file: 'teacher-dashboard.html' },
    { id: 'classrooms', label: 'الفصول الدراسية', file: 'classrooms.html' },
    { id: 'students', label: 'الطلاب', file: 'students.html' },
    { id: 'assessments', label: 'الامتحانات', file: 'assessments.html' },
    { id: 'leaderboard', label: 'لوحة المتصدرين', file: 'leaderboard.html' },
    { id: 'reports', label: 'التقارير', file: 'reports.html' },
    { id: 'book', label: 'الكتاب', file: '../book.html' },
    { id: 'dhad-editor', label: 'محرر ض', file: 'dhad-editor.html' },
  ];
  
  var links = role === 'TEACHER' || role === 'ADMIN' ? teacherLinks : studentLinks;
  
  var nav = document.createElement('nav');
  nav.className = 'mobile-nav';
  nav.innerHTML = '<span class="mobile-nav-brand thuluth">ض ستوديو</span>' +
    '<button class="mobile-nav-toggle" onclick="toggleMobileNav()">&#9776;</button>';
  
  var overlay = document.createElement('div');
  overlay.className = 'mobile-nav-overlay';
  overlay.onclick = closeMobileNav;
  
  var menu = document.createElement('div');
  menu.className = 'mobile-nav-menu';
  menu.id = 'mobileNavMenu';
  
  var menuHTML = '<button class="mobile-nav-close" onclick="closeMobileNav()">&times;</button>';
  menuHTML += '<div style="margin-bottom:20px;padding-bottom:16px;border-bottom:1px solid var(--border)"><strong>' + (user.name || 'مستخدم').replace(/</g,'&lt;').replace(/>/g,'&gt;') + '</strong><br><small style="color:var(--text-muted)">' + (role === 'TEACHER' ? 'معلم' : 'طالب') + '</small></div>';
  
  links.forEach(function(link) {
    var active = link.id === currentPage ? ' active' : '';
    menuHTML += '<a href="' + link.file + '" class="' + active + '">' + link.label + '</a>';
  });
  
  menuHTML += '<a href="settings.html">الإعدادات</a>';
  menuHTML += '<a href="#" onclick="api.logout();return false" style="color:var(--danger);margin-top:16px;border-top:1px solid var(--border);padding-top:16px">تسجيل الخروج</a>';
  
  menu.innerHTML = menuHTML;
  
  document.body.prepend(menu);
  document.body.prepend(overlay);
  document.body.prepend(nav);
  
  window.toggleMobileNav = function() {
    document.getElementById('mobileNavMenu').classList.toggle('active');
    document.querySelector('.mobile-nav-overlay').classList.toggle('active');
  };
  
  window.closeMobileNav = function() {
    document.getElementById('mobileNavMenu').classList.remove('active');
    document.querySelector('.mobile-nav-overlay').classList.remove('active');
  };

  // Button mouse glow effect (debounced via rAF)
  var _mouseGlowPending = false;
  document.addEventListener('mousemove', function(e) {
    if (_mouseGlowPending) return;
    _mouseGlowPending = true;
    requestAnimationFrame(function() {
      _mouseGlowPending = false;
      var btn = e.target.closest('.btn');
      if (btn) {
        var rect = btn.getBoundingClientRect();
        btn.style.setProperty('--x', ((e.clientX - rect.left) / rect.width * 100) + '%');
        btn.style.setProperty('--y', ((e.clientY - rect.top) / rect.height * 100) + '%');
      }
    });
  });
})();
