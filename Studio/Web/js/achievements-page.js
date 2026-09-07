if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('achievements');

var loadAchievements = function() {
  api.getAchievements().then(function(data) {
    var container = document.getElementById('achievementsGrid');
    if (!data || data.length === 0) {
      container.innerHTML = '<div class="empty-state" style="grid-column:1/-1"><div class="icon"></div><p>\u0644\u0645 \u062a\u062d\u0635\u0644 \u0639\u0644\u0649 \u0623\u064a \u0625\u0646\u062c\u0627\u0632 \u0628\u0639\u062f</p><p style="font-size:13px;margin-top:8px;color:var(--text-muted)">\u0623\u0643\u0645\u0644 \u0627\u0644\u062a\u062d\u062f\u064a\u0627\u062a \u0644\u0641\u062a\u062d \u0627\u0644\u0625\u0646\u062c\u0627\u0632\u0627\u062a</p></div>';
      return;
    }
    var unlocked = data.filter(function(a) { return a.unlocked; }).length;
    document.getElementById('achievementCount').textContent = unlocked + ' / ' + data.length;
    container.innerHTML = data.map(function(a) {
      return '<div class="achievement-card ' + (a.unlocked ? '' : 'locked') + '"><div class="achievement-icon"></div><div class="achievement-name">' + escapeHtml(a.name) + '</div><p style="font-size:11px;color:var(--text-muted);margin-top:4px">' + escapeHtml(a.description || '') + '</p>' + (a.unlocked ? '<span class="badge badge-success" style="margin-top:8px">\u0645\u0643\u062a\u0645\u0644</span>' : '') + '</div>';
    }).join('');
  }).catch(function() {
    document.getElementById('achievementsGrid').innerHTML = '<div class="empty-state" style="grid-column:1/-1"><div class="icon"></div><p>\u0627\u0644\u0625\u0646\u062c\u0627\u0632\u0627\u062a \u0642\u064a\u062f \u0627\u0644\u0625\u0639\u062f\u0627\u062f</p></div>';
  });
};

loadAchievements();