if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
}

var _role = api.getRole();
var _isTeacher = (_role === 'TEACHER' || _role === 'ADMIN');

Sidebar.init('leaderboard');

function loadLeaderboard() {
  var p = _isTeacher ? api.getLeaderboard() : api.getStudentLeaderboard();
  p.then(function(res) {
    var data = res.leaderboard || [];
    var container = document.getElementById('leaderboard');
    if (data.length === 0) {
      container.innerHTML = '<div class="empty-state"><div class="icon"></div><p>\u0644\u0627 \u064a\u0648\u062c\u062f \u0637\u0644\u0627\u0628 \u0628\u0639\u062f</p></div>';
      return;
    }
    container.innerHTML = data.map(function(item, i) {
      return '<div class="leaderboard-item"><span class="leaderboard-rank ' + (i < 3 ? 'rank-' + (i+1) : '') + '">' + (i + 1) + '</span><div class="avatar" style="width:36px;height:36px;font-size:14px">' + (item.studentName || item.name || 'U')[0].toUpperCase() + '</div><span class="leaderboard-name">' + escapeHtml(item.studentName || item.name || '\u0627\u0644\u0645\u0633\u062a\u062e\u062f\u0645') + '</span><span style="color:var(--text-muted);font-size:13px">\u0627\u0644\u0645\u0633\u062a\u0648\u0649 ' + (item.currentLevel || 1) + '</span><span class="leaderboard-xp">' + (item.totalXP || item.xp || 0) + ' XP</span></div>';
    }).join('');
  }).catch(function() { toast.error('\u0641\u0634\u0644 \u062a\u062d\u0645\u064a\u0644 \u0644\u0648\u062d\u0629 \u0627\u0644\u0645\u062a\u0635\u062f\u0631\u064a\u0646'); });
}

loadLeaderboard();