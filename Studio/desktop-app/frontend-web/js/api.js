/* Developer: محمد محمود الحموز | Dhad Studio */
/* ====================================================
   API Client - Dhad Studio Frontend
   Fixed: Race condition, token management, stability
   ==================================================== */

const API_BASE = (window.location.hostname === 'localhost' || 
                  window.location.hostname === '127.0.0.1' ||
                  window.location.hostname === '[::1]')
  ? (window.location.port === '3000' ? '' : 'http://localhost:3000')
  : '';

const TEACHER_ROUTES = ['teacher-dashboard', 'classrooms', 'students', 'reports'];
const STUDENT_ROUTES = ['student-dashboard', 'profile', 'achievements'];
const SHARED_ROUTES = ['assessments', 'challenges', 'roadmap', 'leaderboard'];

function escapeHtml(str) {
  if (str == null) return '';
  return String(str)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#039;');
}

function getLoginPath() {
  return window.location.pathname.includes('/pages/') ? '../login.html' : 'login.html';
}

function getDashboardPath(role) {
  const base = window.location.pathname.includes('/pages/') ? '' : 'pages/';
  if (role === 'ADMIN' || role === 'TEACHER') return base + 'teacher-dashboard.html';
  return base + 'student-dashboard.html';
}

// ── Global State (shared across ALL instances) ─────────────────────────────
let _globalRefreshPromise = null;
let _globalRedirecting = false;
let _refreshTokenVersion = 0; // Incremented on each successful refresh

class ApiClient {
  constructor() {
    this.baseURL = API_BASE;
    this._instanceId = Math.random().toString(36).substring(2, 8);
    this._abortControllers = new Map();
  }

  getToken() {
    // httpOnly cookies are sent automatically — no JS access needed
    // Fallback: check if user data exists (means we're logged in)
    return this.getUser() ? 'cookie-based' : null;
  }

  getRefreshToken() {
    // httpOnly cookie — not accessible via JS
    return null;
  }

  setToken(token) {
    // DO NOT store JWT in localStorage (XSS vulnerability)
    // Server sets httpOnly cookies automatically
  }

  setRefreshToken(token) {
    // DO NOT store refresh token in localStorage
    // Server sets httpOnly cookie automatically
  }

  clearTokens() {
    localStorage.removeItem('dhad_user');
    localStorage.removeItem('dhad_role');
    localStorage.removeItem('dhad_last_refresh');
    // Server should clear httpOnly cookies via /auth/logout endpoint
  }

  getUser() {
    const user = localStorage.getItem('dhad_user');
    return user ? JSON.parse(user) : null;
  }

  setUser(user) {
    // Strip sensitive fields — only keep what the UI needs
    const safe = user ? {
      name: user.name,
      role: user.role,
      displayName: user.displayName,
    } : null;
    localStorage.setItem('dhad_user', JSON.stringify(safe));
    if (safe && safe.role) {
      localStorage.setItem('dhad_role', safe.role);
    }
  }

  getRole() {
    return localStorage.getItem('dhad_role') || (this.getUser() && this.getUser().role) || null;
  }

  isLoggedIn() {
    return !!this.getUser();
  }

  // ── Global Redirect (prevents multiple redirects) ───────────────────────
  _redirectToLogin(message) {
    if (_globalRedirecting) return;
    _globalRedirecting = true;

    // Cancel all pending requests
    this._abortAllRequests();

    // Only clear tokens if no recent refresh happened
    const lastRefresh = parseInt(localStorage.getItem('dhad_last_refresh') || '0');
    const now = Date.now();
    if (now - lastRefresh > 5000) {
      this.clearTokens();
    }

    if (typeof toast !== 'undefined' && toast) {
      toast.error(message || 'انتهت صلاحية الجلسة، جارِ تحويلك لتسجيل الدخول...');
    }
    setTimeout(function() {
      window.location.href = getLoginPath();
    }, 300);
  }

  _enforceRoleGuard() {
    const role = this.getRole();
    if (!role) return;
    const page = window.location.pathname.split('/').pop().replace('.html', '');
    if (SHARED_ROUTES.some(r => page.startsWith(r))) return false;
    const isOnTeacherPage = TEACHER_ROUTES.some(r => page.startsWith(r));
    const isOnStudentPage = STUDENT_ROUTES.some(r => page.startsWith(r));
    if (isOnTeacherPage && role === 'STUDENT') {
      window.location.href = getDashboardPath('STUDENT');
      return true;
    }
    if (isOnStudentPage && (role === 'TEACHER' || role === 'ADMIN')) {
      window.location.href = getDashboardPath(role);
      return true;
    }
    return false;
  }

  // ── Request Cancellation ─────────────────────────────────────────────────
  _abortAllRequests() {
    this._abortControllers.forEach((controller) => {
      try { controller.abort(); } catch (e) {}
    });
    this._abortControllers.clear();
  }

  _createAbortController(endpoint) {
    const controller = new AbortController();
    this._abortControllers.set(endpoint, controller);
    return controller;
  }

  _removeAbortController(endpoint) {
    this._abortControllers.delete(endpoint);
  }

  // ── CSRF Token Helper ─────────────────────────────────────────────────────
  _csrfToken = '';
  
  async _ensureCsrfToken() {
    if (this._csrfToken) return this._csrfToken;
    try {
      const resp = await fetch(`${this.baseURL}/api/v1/csrf-token`, { credentials: 'include' });
      const data = await resp.json();
      if (data.csrfToken) {
        this._csrfToken = data.csrfToken;
      }
    } catch (e) {
      // Fallback: try reading from cookie
    }
    if (!this._csrfToken) {
      const match = document.cookie.match(/(?:^|; )_csrf_dhad=([^;]*)/);
      this._csrfToken = match ? decodeURIComponent(match[1]) : '';
    }
    return this._csrfToken;
  }

  _getCsrfToken() {
    return this._csrfToken || '';
  }

  // ── Main Request Method ──────────────────────────────────────────────────
  async request(endpoint, options = {}) {
    const url = `${this.baseURL}${endpoint}`;
    const controller = this._createAbortController(endpoint);
    const method = (options.method || 'GET').toUpperCase();

    const headers = {
      'Content-Type': 'application/json',
      ...options.headers,
    };

    // Add CSRF token for state-changing methods
    if (method !== 'GET' && method !== 'HEAD') {
      await this._ensureCsrfToken();
      const csrfToken = this._getCsrfToken();
      if (csrfToken) {
        headers['X-CSRF-Token'] = csrfToken;
      }
    }

    const fetchOptions = {
      ...options,
      headers,
      signal: controller.signal,
      credentials: 'include', // Send httpOnly cookies with every request
    };

    const doFetch = async () => {
      let response;
      try {
        response = await fetch(url, fetchOptions);
      } catch (fetchError) {
        if (fetchError.name === 'AbortError') {
          return null;
        }
        throw fetchError;
      }

      if (response === null) return null;

      // ── Token Refresh on 401 ────────────────────────────────────────────
      if (response.status === 401 && !endpoint.includes('/auth/')) {
        try {
          await this.refreshAccessToken();
          // Cookies are sent automatically — no need to set Authorization header
          const retryController = new AbortController();
          this._abortControllers.set('retry-' + Date.now(), retryController);
          const retryResponse = await fetch(url, {
            ...fetchOptions,
            signal: retryController.signal,
          });
          
          if (retryResponse.status === 401) {
            this._redirectToLogin('انتهت صلاحية الجلسة، جارِ تحويلك لتسجيل الدخول...');
            throw new Error('انتهت صلاحية الجلسة');
          }
          const retryData = await retryResponse.json();
          if (!retryResponse.ok) {
            throw new Error(retryData.error || retryData.message || 'حدث خطأ');
          }
          return retryData;
        } catch (refreshErr) {
          if (refreshErr.message === 'انتهت صلاحية الجلسة') throw refreshErr;
          if (refreshErr.name === 'AbortError') return null;
          this._redirectToLogin('انتهت صلاحية الجلسة، جارِ تحويلك لتسجيل الدخول...');
          throw new Error('انتهت صلاحية الجلسة');
        }
      }

      const data = await response.json();
      if (!response.ok) {
        throw new Error(data.error || data.message || 'حدث خطأ');
      }
      return data;
    };

    try {
      return await doFetch();
    } catch (error) {
      if (error.name === 'AbortError') return null;
      if (error.message === 'Failed to fetch') {
        throw new Error('لا يمكن الاتصال بالخادم');
      }
      throw error;
    } finally {
      this._removeAbortController(endpoint);
    }
  }

  // ── Auth ──────────────────────────────────────────────

  async login(username, password) {
    const data = await this.request('/api/v1/auth/login', {
      method: 'POST',
      body: JSON.stringify({ username, password }),
    });
    if (data) {
      // Tokens are stored in httpOnly cookies by the server
      // Only store user profile in localStorage for UI purposes
      this.setUser(data.profile);
    }
    return data;
  }

  async register(name, email, password, role = 'STUDENT') {
    const data = await this.request('/api/v1/auth/register', {
      method: 'POST',
      body: JSON.stringify({ name, email, password, role }),
    });
    if (data) {
      // Server returns user object, not profile
      // Store user data for UI purposes
      if (data.user) {
        this.setUser(data.user);
      }
    }
    return data;
  }

  // ── Token Refresh (Global Deduplication) ─────────────────────────────────
  async refreshAccessToken() {
    if (_globalRefreshPromise) return _globalRefreshPromise;

    _globalRefreshPromise = (async () => {
      // Refresh token is in httpOnly cookie — sent automatically
      const url = `${this.baseURL}/api/v1/auth/refresh`;
      const response = await fetch(url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        credentials: 'include', // Send refresh token cookie
      });

      const data = await response.json();
      if (!response.ok) throw new Error(data.error || 'فشل تحديث الجلسة');

      // New tokens are set as httpOnly cookies by the server
      if (data.profile) this.setUser(data.profile);
          return data;
    })();

    try {
      return await _globalRefreshPromise;
    } finally {
      _globalRefreshPromise = null;
    }
  }

  async logout() {
    var user = this.getUser();
    var name = user ? (user.name || '') : '';
    
    // Show goodbye overlay
    var overlay = document.createElement('div');
    overlay.className = 'goodbye-overlay';
    overlay.innerHTML = '<div class="goodbye-content"><div class="goodbye-icon"><svg width="64" height="64" viewBox="0 0 24 24"><use href="#icon-coffee"/></svg></div><div class="goodbye-text">مع السلامة ' + escapeHtml(name.split(' ')[0]) + '!</div><div class="goodbye-subtext">نراك قريباً إن شاء الله</div></div>';
    document.body.appendChild(overlay);
    
    try {
      const headers = { 'Content-Type': 'application/json' };
      // CSRF token for logout
      if (this._csrfToken) headers['X-CSRF-Token'] = this._csrfToken;
      await fetch(`${this.baseURL}/api/v1/auth/logout`, {
        method: 'POST',
        headers: headers,
        credentials: 'include',
      });
    } catch (e) {}
    
    await new Promise(function(r) { setTimeout(r, 1500); });
    this.clearTokens();
    window.location.href = getLoginPath();
  }

  // ── Student Profile ───────────────────────────────────

  async getProfile() {
    return this.request('/api/v1/student/profile');
  }

  async getRoadmap() {
    return this.request('/api/v1/student/roadmap');
  }

  async getAchievements() {
    return this.request('/api/v1/student/achievements');
  }

  // ── Challenges ────────────────────────────────────────

  async getChallenges(difficulty, courseId) {
    const params = [];
    if (difficulty && difficulty !== 'all') params.push(`difficulty=${difficulty}`);
    if (courseId) params.push(`courseId=${encodeURIComponent(courseId)}`);
    const query = params.length ? '?' + params.join('&') : '';
    return this.request(`/api/v1/challenges${query}`);
  }

  async getDailyChallenge() {
    return this.request('/api/v1/challenges/daily');
  }

  async submitSolution(payload) {
    return this.request('/api/v1/student/challenge/submit', {
      method: 'POST',
      body: JSON.stringify(payload),
    });
  }

  // ── Additional Courses ────────────────────────────────

  async getCourses() {
    return this.request('/api/v1/student/courses');
  }

  async enrollCourse(courseId) {
    return this.request(`/api/v1/student/courses/${courseId}/enroll`, {
      method: 'POST',
    });
  }

  async getCourseRoadmap(courseId) {
    return this.request(`/api/v1/student/courses/${courseId}/roadmap`);
  }

  // ── Telemetry ─────────────────────────────────────────

  async submitTelemetry(payload) {
    return this.request('/api/v1/submissions/bulk-report', {
      method: 'POST',
      body: JSON.stringify(payload),
    });
  }

  // ── Leaderboard ───────────────────────────────────────

  async getLeaderboard(limit = 50) {
    return this.request(`/api/v1/analytics/leaderboard?limit=${limit}`);
  }

  async getStudentLeaderboard(limit = 50) {
    return this.request(`/api/v1/student/leaderboard?limit=${limit}`);
  }

  async getClassroomLeaderboard(classroomId) {
    return this.request(`/api/v1/analytics/leaderboard?classroomId=${classroomId}`);
  }

  // ── Teacher - Analytics ───────────────────────────────

  async getTeacherDashboard() {
    return this.request('/api/v1/analytics/dashboard');
  }

  async getClassrooms() {
    return this.request('/api/v1/analytics/classrooms');
  }

  async joinClassroom(inviteCode) {
    return this.request('/api/v1/student/join-classroom', {
      method: 'POST',
      body: JSON.stringify({ inviteCode })
    });
  }

  async getMyClassroom() {
    return this.request('/api/v1/student/my-classroom');
  }

  async getClassroomProgress(classroomId) {
    return this.request(`/api/v1/analytics/progress?classroomId=${classroomId}`);
  }

  async getAttendance(classroomId) {
    return this.request(`/api/v1/analytics/attendance-history?classroomId=${classroomId}`);
  }

  async markAttendance(data) {
    return this.request('/api/v1/analytics/attendance', {
      method: 'POST',
      body: JSON.stringify(data),
    });
  }

  async exportClassroomReport(classroomId) {
    return this.request(`/api/v1/analytics/classroom/${classroomId}/export`);
  }

  // ── Teacher - Assessments ─────────────────────────────

  async getAssessments(classroomId) {
    if (classroomId) {
      return this.request(`/api/v1/assessments/classroom/${classroomId}`);
    }
    return this.request('/api/v1/analytics/assessments');
  }

  async createAssessment(data) {
    return this.request('/api/v1/assessments/create', {
      method: 'POST',
      body: JSON.stringify(data),
    });
  }

  async getAssessmentResults(id) {
    return this.request(`/api/v1/assessments/${id}/results`);
  }

  // ── Student - Profile ─────────────────────────────

  async getStudentProfile() {
    return this.request('/api/v1/student/profile');
  }

  // ── Student - Assessments ─────────────────────────────

  async getStudentAssessments() {
    return this.request('/api/v1/student/assessments');
  }

  async getActiveAssessment() {
    return this.request('/api/v1/student/assessment/active');
  }

  async startAssessmentAttempt(assessmentId) {
    return this.request('/api/v1/student/assessment/start', {
      method: 'POST',
      body: JSON.stringify({ assessmentId }),
    });
  }

  async completeAssessment(assessmentId, mcAnswersOrScore) {
    var body = { assessmentId };
    if (Array.isArray(mcAnswersOrScore)) {
      body.mcAnswers = mcAnswersOrScore;
    }
    return this.request('/api/v1/student/assessment/complete', {
      method: 'POST',
      body: JSON.stringify(body),
    });
  }

  async submitAssessmentAnswer(data) {
    return this.request('/api/v1/student/assessment/submit', {
      method: 'POST',
      body: JSON.stringify(data),
    });
  }

  // ── Student Management (Teacher) ──────────────────────

  async getTeacherStudents(classroomId) {
    const query = classroomId ? `?classroomId=${classroomId}` : '';
    return this.request(`/api/v1/teacher/students${query}`);
  }

  async createStudent(data) {
    return this.request('/api/v1/teacher/students', {
      method: 'POST',
      body: JSON.stringify(data),
    });
  }

  async bulkCreateStudents(students, classroomId) {
    return this.request('/api/v1/teacher/students/bulk', {
      method: 'POST',
      body: JSON.stringify({ students, classroomId }),
    });
  }

  async deleteStudent(id) {
    return this.request(`/api/v1/teacher/students/${id}`, {
      method: 'DELETE',
    });
  }

  async assignStudentToClassroom(studentId, classroomId) {
    return this.request('/api/v1/teacher/students/assign', {
      method: 'POST',
      body: JSON.stringify({ studentId, classroomId }),
    });
  }

  // ── Invitations (Teacher) ─────────────────────────────

  async importStudents(students, classroomId) {
    return this.request('/api/v1/teacher/invitations/import', {
      method: 'POST',
      body: JSON.stringify({ students, classroomId }),
    });
  }

  async getInvitations() {
    return this.request('/api/v1/teacher/invitations');
  }

  // ── Student Login Flow ────────────────────────────────

  async getPublicClassrooms() {
    return this.request('/api/v1/auth/classrooms');
  }

  async getClassroomStudents(classroomId) {
    return this.request(`/api/v1/auth/classrooms/${classroomId}/students`);
  }

  // ── OTP Activation ────────────────────────────────────

  async requestOTP(email) {
    return this.request('/api/v1/auth/request-otp', {
      method: 'POST',
      body: JSON.stringify({ email }),
    });
  }

  async verifyOTP(email, otp, password) {
    const data = await this.request('/api/v1/auth/verify-otp', {
      method: 'POST',
      body: JSON.stringify({ email, otp, password }),
    });
    if (data && data.accessToken) {
      this.setToken(data.accessToken);
      if (data.refreshToken) this.setRefreshToken(data.refreshToken);
      this.setUser(data.profile);
    }
    return data;
  }

  // ── Password ──────────────────────────────────────────

  async changePassword(currentPassword, newPassword) {
    return this.request('/api/v1/auth/change-password', {
      method: 'PUT',
      body: JSON.stringify({ currentPassword, newPassword }),
    });
  }

  // ── Workspace ─────────────────────────────────────────

  async saveWorkspace(challengeId, code, language) {
    return this.request('/api/v1/workspace/save', {
      method: 'POST',
      body: JSON.stringify({ challengeId, code, language }),
    });
  }

  async loadWorkspace(challengeId) {
    return this.request(`/api/v1/workspace/load/${challengeId}`);
  }

  async listWorkspaces() {
    return this.request('/api/v1/workspace/list');
  }

  // ── Security (Admin) ──────────────────────────────────

  async getSecurityDashboard() {
    return this.request('/api/v1/security/dashboard');
  }

  async getAuditLog(params = {}) {
    const query = new URLSearchParams(params).toString();
    return this.request(`/api/v1/security/audit?${query}`);
  }

  async unblockIP(ip) {
    return this.request('/api/v1/security/unblock-ip', {
      method: 'POST',
      body: JSON.stringify({ ip }),
    });
  }

  // ── Health ────────────────────────────────────────────

  async getHealth() {
    return this.request('/health');
  }
}

// ── Page Unload Cleanup ────────────────────────────────────────────────────
window.addEventListener('beforeunload', function() {
  if (typeof api !== 'undefined' && api._abortControllers) {
    api._abortControllers.forEach((controller) => {
      try { controller.abort(); } catch (e) {}
    });
  }
});

// ── Visibility Change (pause requests when tab hidden) ─────────────────────
document.addEventListener('visibilitychange', function() {
  if (document.hidden && typeof api !== 'undefined' && api._abortControllers) {
    // Don't abort - just let them complete naturally
  }
});

const api = new ApiClient();
