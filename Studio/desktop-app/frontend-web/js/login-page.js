(function() {
  if (typeof api !== 'undefined') {
    api._ensureCsrfToken();
    if (api.isLoggedIn()) {
      var u = api.getUser();
      if (u && (u.role === 'TEACHER' || u.role === 'ADMIN')) {
        window.location.href = 'pages/teacher-dashboard.html';
      } else if (u) {
        window.location.href = 'pages/student-dashboard.html';
      }
    }
  }
})();

function togglePassword(inputId, btn) {
  var input = document.getElementById(inputId);
  var eyeOpen = btn.querySelector('.eye-open');
  var eyeClosed = btn.querySelector('.eye-closed');
  if (input.type === 'password') {
    input.type = 'text';
    eyeOpen.style.display = 'none';
    eyeClosed.style.display = 'block';
  } else {
    input.type = 'password';
    eyeOpen.style.display = 'block';
    eyeClosed.style.display = 'none';
  }
}

function switchTab(tab) {
  hideError();
  if (tab === 'student') {
    document.getElementById('tabStudent').classList.add('active');
    document.getElementById('tabTeacher').classList.remove('active');
    document.getElementById('studentSection').style.display = 'block';
    document.getElementById('teacherSection').style.display = 'none';
  } else {
    document.getElementById('tabTeacher').classList.add('active');
    document.getElementById('tabStudent').classList.remove('active');
    document.getElementById('teacherSection').style.display = 'block';
    document.getElementById('studentSection').style.display = 'none';
  }
}

function showError(msg) {
  var el = document.getElementById('errorBox');
  el.textContent = msg;
  el.style.display = 'block';
}
function hideError() { document.getElementById('errorBox').style.display = 'none'; }

function setLoading(btn, loading) {
  if (loading) {
    btn.disabled = true;
    btn.dataset.originalText = btn.textContent;
    btn.innerHTML = '<span class="loading-spinner"></span> جاري تسجيل الدخول...';
  } else {
    btn.disabled = false;
    btn.textContent = btn.dataset.originalText || 'تسجيل الدخول';
  }
}

function showLoginToast(name, role) {
  var toast = document.createElement('div');
  toast.className = 'login-toast';
  var iconSpan = document.createElement('span');
  iconSpan.className = 'login-toast-icon';
  if (role === 'student') {
    iconSpan.innerHTML = '<svg width="24" height="24" viewBox="0 0 24 24"><use href="#icon-graduation"/></svg>';
  } else {
    iconSpan.innerHTML = '<svg width="24" height="24" viewBox="0 0 24 24"><use href="#icon-users"/></svg>';
  }
  var textSpan = document.createElement('span');
  textSpan.className = 'login-toast-text';
  textSpan.textContent = 'مرحباً ' + (name || '') + '!';
  toast.appendChild(iconSpan);
  toast.appendChild(textSpan);
  document.body.appendChild(toast);
  setTimeout(function() { toast.remove(); }, 4000);
}

async function handleStudentLogin(e) {
  e.preventDefault();
  var email = document.getElementById('studentEmail').value.trim();
  var password = document.getElementById('studentPassword').value;
  var btn = document.getElementById('studentBtn');
  hideError();
  if (!email || !password) { showError('يرجى ملء جميع الحقول'); return; }
  setLoading(btn, true);
  try {
    var data = await api.request('/api/v1/auth/login', {
      method: 'POST',
      body: JSON.stringify({ username: email, password: password })
    });
    api.setUser(data.profile);
    showLoginToast(data.profile ? data.profile.name : '', 'student');
    setTimeout(function() { window.location.href = 'pages/student-dashboard.html'; }, 800);
  } catch (ex) {
    showError(ex.message);
    setLoading(btn, false);
  }
}

async function handleTeacherLogin(e) {
  e.preventDefault();
  var email = document.getElementById('teacherEmail').value.trim();
  var password = document.getElementById('teacherPassword').value;
  var btn = document.getElementById('teacherBtn');
  hideError();
  if (!email || !password) { showError('يرجى ملء جميع الحقول'); return; }
  setLoading(btn, true);
  try {
    var data = await api.request('/api/v1/auth/login', {
      method: 'POST',
      body: JSON.stringify({ username: email, password: password })
    });
    api.setUser(data.profile);
    showLoginToast(data.profile ? data.profile.name : '', 'teacher');
    setTimeout(function() { window.location.href = 'pages/teacher-dashboard.html'; }, 800);
  } catch (ex) {
    showError(ex.message);
    setLoading(btn, false);
  }
}

document.querySelectorAll('.input-field').forEach(function(input) {
  input.addEventListener('focus', function() {
    this.parentElement.style.transform = 'scale(1.02)';
    this.parentElement.style.transition = 'transform 0.3s ease';
  });
  input.addEventListener('blur', function() {
    this.parentElement.style.transform = 'scale(1)';
  });
});

document.querySelectorAll('.btn-submit').forEach(function(btn) {
  btn.addEventListener('click', function(e) {
    var ripple = document.createElement('span');
    ripple.className = 'ripple-circle';
    var rect = this.getBoundingClientRect();
    var size = Math.max(rect.width, rect.height);
    ripple.style.width = ripple.style.height = size + 'px';
    ripple.style.left = (e.clientX - rect.left - size / 2) + 'px';
    ripple.style.top = (e.clientY - rect.top - size / 2) + 'px';
    this.appendChild(ripple);
    setTimeout(function() { ripple.remove(); }, 600);
  });
});
