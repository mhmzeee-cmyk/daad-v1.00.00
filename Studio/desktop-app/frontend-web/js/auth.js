/* Developer: محمد محمود الحموز | Dhad Studio */
/* ====================================================
   Auth Module - Login/Register
   ==================================================== */

function initAuth() {
  if (api.isLoggedIn()) {
    const user = api.getUser();
    if (user) {
      const redirect = user.role === 'TEACHER' || user.role === 'ADMIN'
        ? 'pages/teacher-dashboard.html'
        : 'pages/student-dashboard.html';
      window.location.href = redirect;
    }
  }
}

function initLoginPage() {
  const form = document.getElementById('loginForm');
  if (!form) return;

  form.addEventListener('submit', async (e) => {
    e.preventDefault();
    const email = document.getElementById('email').value;
    const password = document.getElementById('password').value;
    const errorEl = document.getElementById('error');
    const btn = form.querySelector('button[type="submit"]');

    errorEl.style.display = 'none';
    btn.disabled = true;
    btn.textContent = 'جاري تسجيل الدخول...';

    try {
      const data = await api.login(email, password);
      const redirect = data.role === 'TEACHER' || data.role === 'ADMIN'
        ? 'pages/teacher-dashboard.html'
        : 'pages/student-dashboard.html';
      window.location.href = redirect;
    } catch (err) {
      errorEl.textContent = err.message;
      errorEl.style.display = 'block';
      btn.disabled = false;
      btn.textContent = 'تسجيل الدخول';
    }
  });
}

function initRegisterPage() {
  const form = document.getElementById('registerForm');
  if (!form) return;

  form.addEventListener('submit', async (e) => {
    e.preventDefault();
    const name = document.getElementById('name').value;
    const email = document.getElementById('email').value;
    const password = document.getElementById('password').value;
    const role = document.getElementById('role').value;
    const errorEl = document.getElementById('error');
    const btn = form.querySelector('button[type="submit"]');

    errorEl.style.display = 'none';
    btn.disabled = true;
    btn.textContent = 'جاري إنشاء الحساب...';

    try {
      await api.register(name, email, password, role);
      const redirect = role === 'TEACHER' ? 'pages/teacher-dashboard.html' : 'pages/student-dashboard.html';
      window.location.href = redirect;
    } catch (err) {
      errorEl.textContent = err.message;
      errorEl.style.display = 'block';
      btn.disabled = false;
      btn.textContent = 'إنشاء حساب';
    }
  });
}
