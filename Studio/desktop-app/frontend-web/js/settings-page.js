(function() {
  var user = api.getUser();
  if (!user) { window.location.href = '../login.html'; return; }

  Sidebar.init('settings');

  var current = ThemeSwitcher.getCurrent();
  document.getElementById('themesContainer').innerHTML = ThemeSwitcher.renderCard(current);

  document.getElementById('settingsName').value = user.name || '';
  document.getElementById('settingsEmail').value = user.email || '';
  var roleMap = { TEACHER: 'معلم', ADMIN: 'مدير', STUDENT: 'طالب' };
  document.getElementById('settingsRole').value = roleMap[user.role] || user.role;

  // Password change
  var btn = document.getElementById('changePasswordBtn');
  if (btn) {
    btn.addEventListener('click', function() {
      var currentPw = document.getElementById('currentPassword').value.trim();
      var newPw = document.getElementById('newPassword').value.trim();
      var confirmPw = document.getElementById('confirmPassword').value.trim();

      if (!currentPw || !newPw || !confirmPw) {
        Toast.show('يرجى ملء جميع الحقول', 'error');
        return;
      }
      if (newPw.length < 8) {
        Toast.show('كلمة المرور الجديدة يجب أن تكون 8 أحرف على الأقل', 'error');
        return;
      }
      if (newPw !== confirmPw) {
        Toast.show('كلمتا المرور غير متطابقتين', 'error');
        return;
      }
      if (newPw === currentPw) {
        Toast.show('كلمة المرور الجديدة يجب أن تختلف عن الحالية', 'error');
        return;
      }

      btn.disabled = true;
      btn.textContent = 'جاري التغيير...';

      api.changePassword(currentPw, newPw).then(function(data) {
        if (data.success) {
          Toast.show('تم تغيير كلمة المرور بنجاح', 'success');
          document.getElementById('currentPassword').value = '';
          document.getElementById('newPassword').value = '';
          document.getElementById('confirmPassword').value = '';
        } else {
          Toast.show(data.message || 'فشل تغيير كلمة المرور', 'error');
        }
      }).catch(function() {
        Toast.show('خطأ في الاتصال بالخادم', 'error');
      }).finally(function() {
        btn.disabled = false;
        btn.textContent = 'تغيير كلمة المرور';
      });
    });
  }
})();
