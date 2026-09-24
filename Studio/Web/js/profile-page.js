if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('profile');

var user = api.getUser();
if (user) {
  document.getElementById('avatar').textContent = (user.name || 'U')[0].toUpperCase();
  document.getElementById('profileName').textContent = user.name || '\u0645\u0633\u062a\u062e\u062f\u0645';
  document.getElementById('profileEmail').textContent = user.email || '';
  document.getElementById('profileRole').textContent = user.role === 'TEACHER' ? '\u0645\u0639\u0644\u0645' : user.role === 'ADMIN' ? '\u0645\u062f\u064a\u0631' : '\u0637\u0627\u0644\u0628';
  document.getElementById('joinDate').textContent = new Date().toLocaleDateString('ar-SA');
}

function loadProfile() {
  api.getProfile().then(function(res) {
    var profile = res.profile || res;
    document.getElementById('totalXP').textContent = profile.totalXP || 0;
    document.getElementById('currentLevel').textContent = profile.currentLevel || 1;
    document.getElementById('currentStreak').textContent = profile.currentStreak || 0;
    document.getElementById('completedChallenges').textContent = profile.totalChallenges || 0;
    var xpInLevel = (profile.totalXP || 0) - (profile.currentLevelXP || 0);
    var xpNeeded = (profile.nextLevelXP || 100) - (profile.currentLevelXP || 0);
    var progress = profile.levelProgress || 0;
    document.getElementById('xpProgress').textContent = xpInLevel + ' / ' + xpNeeded + ' XP';
    document.getElementById('xpProgressBar').style.width = progress + '%';
  }).catch(function() { toast.error('\u0641\u0634\u0644 \u062a\u062d\u0645\u064a\u0644 \u0627\u0644\u0645\u0644\u0641 \u0627\u0644\u0634\u062e\u0635\u064a'); });
}

loadProfile();