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
        toast.error('يرجى ملء جميع الحقول');
        return;
      }
      if (newPw.length < 8) {
        toast.error('كلمة المرور الجديدة يجب أن تكون 8 أحرف على الأقل');
        return;
      }
      if (newPw !== confirmPw) {
        toast.error('كلمتا المرور غير متطابقتين');
        return;
      }
      if (newPw === currentPw) {
        toast.error('كلمة المرور الجديدة يجب أن تختلف عن الحالية');
        return;
      }

      btn.disabled = true;
      btn.textContent = 'جاري التغيير...';

      api.changePassword(currentPw, newPw).then(function(data) {
        if (data.success) {
          toast.success('تم تغيير كلمة المرور بنجاح');
          document.getElementById('currentPassword').value = '';
          document.getElementById('newPassword').value = '';
          document.getElementById('confirmPassword').value = '';
        } else {
          toast.error(data.message || 'فشل تغيير كلمة المرور');
        }
      }).catch(function() {
        toast.error('خطأ في الاتصال بالخادم');
      }).finally(function() {
        btn.disabled = false;
        btn.textContent = 'تغيير كلمة المرور';
      });
    });
  }
})();
