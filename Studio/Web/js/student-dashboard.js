if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('student-dashboard');

var user = api.getUser();
if (user) {
  document.getElementById('userName').textContent = user.name || 'طالب';
  document.getElementById('userEmail').textContent = user.email || '';
  document.getElementById('userAvatar').textContent = (user.name || 'T')[0].toUpperCase();
}

// Welcome banner
(function() {
  var banner = document.getElementById('welcomeBanner');
  var title = document.getElementById('welcomeTitle');
  var subtitle = document.getElementById('welcomeSubtitle');
  var sessionKey = 'student_welcomed_' + (user ? user.id : '');
  var isNewSession = !sessionStorage.getItem(sessionKey);
  var lastWelcome = localStorage.getItem('student_last_welcome');
  var isNewUser = !lastWelcome;
  
  if (isNewSession) {
    banner.style.display = 'block';
    if (isNewUser) {
      title.textContent = 'مرحباً بك في رحلة التعلم!';
      subtitle.textContent = 'هذا هو يومك الأول - ابدأ بالتحديات لجمع النقاط';
    } else {
      var hour = new Date().getHours();
      var greeting = hour < 12 ? 'صباح النشاط' : hour < 18 ? 'مساء الخير' : 'مساء الهدوء';
      var name = (user && user.name) ? user.name.split(' ')[0] : 'الطالب';
      title.textContent = greeting + ' ' + name + '!';
      subtitle.textContent = 'استمر في التقدم - أنت على الطريق الصحيح';
    }
    sessionStorage.setItem(sessionKey, '1');
    localStorage.setItem('student_last_welcome', new Date().toISOString());
  }
})();

function dismissWelcome() {
  var banner = document.getElementById('welcomeBanner');
  banner.style.opacity = '0';
  banner.style.transform = 'translateY(-10px)';
  banner.style.transition = 'all 0.3s ease';
  setTimeout(function() { banner.style.display = 'none'; }, 300);
}

function renderActionName(action) {
  var map = {
    'CHALLENGE_SUBMIT': 'تقديم تحدي',
    'CHALLENGE_PASS': 'اكتمال تحدي',
    'XP_AWARDED': 'كسب نقطة',
    'xp_awarded': 'كسب نقطة',
    'LEVEL_UP': 'ترقية مستوى',
    'ACHIEVEMENT_AWARDED': 'حصول على علامة',
    'LOGIN': 'تسجيل دخول',
    'login': 'تسجيل دخول',
    'ASSESSMENT_START': 'بدء امتحان',
    'ASSESSMENT_SUBMIT': 'تسليم امتحان',
    'create_assessment': 'إنشاء امتحان',
    'bulk_submission_recorded': 'تسجيل إرسالات',
  };
  return map[action] || action || 'نشاط';
}

function renderActivityTime(dateStr) {
  if (!dateStr) return '';
  var diff = Date.now() - new Date(dateStr).getTime();
  var mins = Math.floor(diff / 60000);
  if (mins < 1) return 'الآن';
  if (mins < 60) return mins + ' دقيقة';
  var hrs = Math.floor(mins / 60);
  if (hrs < 24) return hrs + ' ساعة';
  return Math.floor(hrs / 24) + ' يوم';
}

var LEVEL_NAMES = ['مبتدئ', 'متوسط', 'متقدم', 'خبير', 'محترف', 'عبقري'];

function getLevelName(level) {
  if (level <= LEVEL_NAMES.length) return LEVEL_NAMES[level - 1];
  return 'خبير+' + level;
}

function loadDashboard() {
  api.getProfile().then(function(res) {
    var p = res.profile || res;
    document.getElementById('totalXP').textContent = p.totalXP || 0;
    document.getElementById('currentLevel').textContent = p.currentLevel || 1;
    document.getElementById('currentStreak').textContent = p.currentStreak || 0;

    // ── Journey Card ──
    var level = p.currentLevel || 1;
    var totalXP = p.totalXP || 0;
    var levelProgress = p.levelProgress || 0;
    var currentLevelXP = p.currentLevelXP || 0;
    var nextLevelXP = p.nextLevelXP || 100;
    var xpToNext = p.xpToNextLevel || 0;

    document.getElementById('journeyLevelBadge').textContent = 'المستوى ' + level + ' — ' + getLevelName(level);
    document.getElementById('journeyXpLabel').textContent = totalXP + ' / ' + nextLevelXP + ' XP';
    document.getElementById('journeyProgressBar').style.width = Math.min(levelProgress, 100) + '%';

    if (xpToNext > 0) {
      var needed = Math.ceil(xpToNext / 100);
      document.getElementById('journeyHint').textContent = 'أكمل ' + needed + ' تحدي' + (needed > 1 ? 'ات' : '') + ' للوصول إلى المستوى التالي';
    } else {
      document.getElementById('journeyHint').textContent = 'أنت في أعلى مستوى! استمر في التقدم';
    }

    // ── Quick Stats ──
    document.getElementById('qsChallenges').textContent = p.totalChallenges || 0;
    document.getElementById('qsTodayXP').textContent = '+' + (p.todayXP || 0);
    document.getElementById('qsRank').textContent = p.rank ? '#' + p.rank + ' / ' + (p.totalStudents || '') : '-';
    document.getElementById('qsSuccessRate').textContent = (p.totalChallenges || 0) > 0 ? Math.round(p.totalXP / Math.max(p.totalChallenges, 1)) + '' : '0';

    // ── Streak Week ──
    var streak = p.currentStreak || 0;
    document.getElementById('streakCountBadge').textContent = streak + ' يوم';
    var today = new Date().getDay();
    // Sunday=0 maps to index 0, adjust for Arabic week (Sat=0)
    var arabicDayMap = [6, 0, 1, 2, 3, 4, 5];
    var todayIdx = arabicDayMap[today];

    for (var d = 0; d < 7; d++) {
      var dot = document.getElementById('sd' + d);
      if (!dot) continue;
      if (d === todayIdx) {
        dot.className = 'streak-dot active';
      } else if (d > todayIdx) {
        dot.className = 'streak-dot';
      } else {
        // Days before today: active if streak covers them
        var daysAgo = todayIdx - d;
        dot.className = daysAgo <= streak ? 'streak-dot filled' : 'streak-dot';
      }
    }

    // ── Recent Activity ──
    var container = document.getElementById('recentActivity');
    var activities = p.recentActivities || [];
    if (activities.length === 0) {
      container.innerHTML = '<div class="empty-state"><p>لم تقم بأي نشاط</p></div>';
    } else {
      container.innerHTML = activities.slice(0, 8).map(function(a) {
        var details = a.details || {};
        var extra = details.xpAwarded ? ' - ' + details.xpAwarded + ' XP' : '';
        extra += details.challengeTitle ? ' - ' + escapeHtml(details.challengeTitle) : '';
        return '<div style="display:flex;justify-content:space-between;padding:8px 0;border-bottom:1px solid var(--border)"><span>' + escapeHtml(renderActionName(a.action)) + extra + '</span><span style="color:var(--text-muted);font-size:12px;white-space:nowrap">' + renderActivityTime(a.createdAt) + '</span></div>';
      }).join('');
    }
  }).catch(function() { toast.error('فشل تحميل البيانات'); });

  // ── Leaderboard ──
  api.getStudentLeaderboard().then(function(res) {
    var leaderboard = res.leaderboard || [];
    var container = document.getElementById('leaderboardPreview');
    if (leaderboard.length === 0) { container.innerHTML = '<div class="empty-state"><p>لا يوجد بعد</p></div>'; return; }
    var medalSvgs = [
      '<svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#f59e0b" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="8" r="6"/><path d="M8.21 13.89L7 23l5-3 5 3-1.21-9.12"/></svg>',
      '<svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#94a3b8" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="8" r="6"/><path d="M8.21 13.89L7 23l5-3 5 3-1.21-9.12"/></svg>',
      '<svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#b45309" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="8" r="6"/><path d="M8.21 13.89L7 23l5-3 5 3-1.21-9.12"/></svg>'
    ];
    container.innerHTML = leaderboard.slice(0, 5).map(function(item, i) {
      var rankDisplay = i < 3 ? '<span class="leaderboard-medal">' + medalSvgs[i] + '</span>' : '<span class="leaderboard-rank">' + (i + 1) + '</span>';
      var isMe = user && item.studentName === user.name;
      return '<div class="leaderboard-item' + (isMe ? ' leaderboard-me' : '') + '">' + rankDisplay + '<span class="leaderboard-name">' + escapeHtml(item.studentName) + '</span><span class="leaderboard-xp">' + item.totalXP + ' XP</span></div>';
    }).join('');
  }).catch(function() {
    document.getElementById('leaderboardPreview').innerHTML = '<p style="color:var(--text-muted)">لا يتم تحميل البيانات</p>';
  });

  // ── Daily Challenge ──
  api.getDailyChallenge().then(function(res) {
    var c = res.challenge;
    if (!c) {
      document.getElementById('dailyChallengeContent').innerHTML = '<div class="empty-state"><p>لا يوجد تحدي يومي</p></div>';
      return;
    }
    var diffLabel = 'سهل';
    var diffClass = 'tier-easy';
    if (c.difficulty === 'MEDIUM') { diffLabel = 'متوسط'; diffClass = 'tier-medium'; }
    else if (c.difficulty === 'HARD') { diffLabel = 'صعب'; diffClass = 'tier-hard'; }
    else if (c.difficulty === 'EXPERT') { diffLabel = 'خبير'; diffClass = 'tier-hard'; }

    document.getElementById('dailyChallengeContent').innerHTML =
      '<div style="margin-bottom:8px"><strong style="font-size:14px">' + escapeHtml(c.title) + '</strong></div>' +
      '<div style="display:flex;gap:8px;align-items:center;margin-bottom:12px;flex-wrap:wrap">' +
        '<span class="badge ' + diffClass + '" style="font-size:11px">' + diffLabel + '</span>' +
        '<span style="color:var(--text-muted);font-size:12px;display:flex;align-items:center;gap:4px"><svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"/></svg> ' + (c.points || 100) + ' XP</span>' +
        (c.estimatedTime ? '<span style="color:var(--text-muted);font-size:12px;display:flex;align-items:center;gap:4px"><svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/></svg> ' + c.estimatedTime + ' دقيقة</span>' : '') +
      '</div>' +
      '<a href="challenges.html?id=' + c.id + '" class="btn btn-primary btn-sm" style="width:100%;text-align:center">ابدأ التحدي ←</a>';
  }).catch(function() {
    document.getElementById('dailyChallengeContent').innerHTML = '<div class="empty-state"><p>لا يوجد تحدي يومي</p></div>';
  });

  // ── Assessments ──
  api.getStudentAssessments().then(function(res) {
    var assessments = res.assessments || [];
    var container = document.getElementById('assessmentsList');
    if (assessments.length === 0) { container.innerHTML = '<div class="empty-state"><p>لا توجد امتحانات حالياً</p></div>'; return; }
    container.innerHTML = assessments.map(function(a) {
      var statusClass = a.status === 'active' ? 'badge-success' : a.status === 'upcoming' ? 'badge-warning' : 'badge-info';
      var statusText = a.status === 'active' ? 'نشط الآن' : a.status === 'upcoming' ? 'قادم' : 'مكتمل';
      return '<div style="display:flex;justify-content:space-between;align-items:center;padding:12px 0;border-bottom:1px solid var(--border)"><div><strong>' + escapeHtml(a.title) + '</strong><br><span style="color:var(--text-muted);font-size:13px">' + a.allowedTime + ' دقيقة - ' + a.totalPoints + ' نقطة</span></div><span class="badge ' + statusClass + '">' + statusText + '</span></div>';
    }).join('');
  }).catch(function() {
    document.getElementById('assessmentsList').innerHTML = '<p style="color:var(--text-muted)">لا يتم تحميل الامتحانات</p>';
  });
}

loadDashboard();

// ── Classroom Check ───────────────────────────────────────────────────────
function checkClassroom() {
  api.getMyClassroom().then(function(res) {
    var banner = document.getElementById('joinClassroomBanner');
    var info = document.getElementById('myClassroomInfo');
    if (res.assigned && res.classroom) {
      banner.style.display = 'none';
      info.style.display = 'block';
      document.getElementById('myClassroomName').textContent = res.classroom.name;
      document.getElementById('myClassroomTeacher').textContent = res.classroom.teacher ? ('المعلم: ' + res.classroom.teacher.name) : '';
      document.getElementById('myClassroomCount').textContent = (res.classroom.studentCount || 0) + ' طالب';
    } else {
      banner.style.display = 'block';
      info.style.display = 'none';
    }
  }).catch(function() {
    document.getElementById('joinClassroomBanner').style.display = 'block';
  });
}

function joinClassroom() {
  var input = document.getElementById('inviteCodeInput');
  var code = input.value.trim();
  if (!code || code.length < 4) {
    toast.error('أدخل كود الدعوة');
    return;
  }
  api.joinClassroom(code).then(function(res) {
    toast.success(res.message);
    input.value = '';
    checkClassroom();
  }).catch(function(err) {
    toast.error(err.message || 'فشل الانضمام');
  });
}

checkClassroom();
