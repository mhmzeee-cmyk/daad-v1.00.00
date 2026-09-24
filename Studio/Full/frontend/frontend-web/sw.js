const CACHE_NAME = 'dhad-v13';

const APP_SHELL = [
  '/',
  '/index.html',
  '/login.html',
  '/css/style.css',
  '/css/themes.css',
  '/css/animations.css',
  '/css/decorations.css',
  '/js/api.js',
  '/js/dhad-lexer.js',
  '/js/dhad-ast.js',
  '/js/dhad-parser.js',
  '/js/dhad-codegen.js',
  '/js/dhad.js',
  '/js/auth.js',
  '/js/dhad-libraries.js',
  '/js/sidebar.js',
  '/js/theme-switcher.js',
  '/js/toast.js',
  '/js/smart-evaluator.js',
  '/js/challenges-page.js',
  '/js/student-dashboard.js',
  '/js/mobile-nav.js',
  '/pages/dhad-editor.html',
  '/pages/challenges.html',
  '/pages/assessments.html',
  '/pages/student-dashboard.html',
  '/pages/teacher-dashboard.html',
  '/pages/leaderboard.html',
  '/pages/profile.html',
  '/pages/settings.html',
  '/pages/achievements.html',
  '/pages/additional-courses.html',
  '/pages/classrooms.html',
  '/pages/course-roadmap.html',
  '/pages/roadmap.html',
  '/pages/students.html',
  '/pages/reports.html',
  '/pages/web-editor.html',
  '/pages/terms.html',
  '/pages/privacy.html'
];

self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then((cache) => cache.addAll(APP_SHELL))
      .then(() => self.skipWaiting())
  );
});

self.addEventListener('activate', (event) => {
  event.waitUntil(
    caches.keys().then((keys) =>
      Promise.all(
        keys
          .filter((key) => key !== CACHE_NAME)
          .map((key) => caches.delete(key))
      )
    ).then(() => self.clients.claim())
  );
});

self.addEventListener('fetch', (event) => {
  const { request } = event;
  const url = new URL(request.url);

  if (url.pathname.startsWith('/api/')) {
    event.respondWith(fetch(request).catch(() => {
      return new Response(
        JSON.stringify({ error: 'Offline', message: 'No network available' }),
        { status: 503, headers: { 'Content-Type': 'application/json' } }
      );
    }));
    return;
  }

  // Network-first for HTML, JS, CSS — always get latest versions
  if (request.destination === 'document' ||
      url.pathname.endsWith('.js') ||
      url.pathname.endsWith('.css')) {
    event.respondWith(networkFirst(request));
    return;
  }

  event.respondWith(cacheFirst(request));
});

function networkFirst(request) {
  return fetch(request).then((response) => {
    if (response.ok) {
      const clone = response.clone();
      caches.open(CACHE_NAME).then((cache) => cache.put(request, clone));
    }
    return response;
  }).catch(() => {
    return caches.match(request).then((cached) => {
      if (cached) return cached;
      if (request.destination === 'document') {
        return caches.match('/index.html');
      }
      return new Response('Offline', { status: 503, statusText: 'Offline' });
    });
  });
}

function cacheFirst(request) {
  return caches.match(request).then((cached) => {
    if (cached) return cached;

    return fetch(request).then((response) => {
      if (response.ok) {
        const clone = response.clone();
        caches.open(CACHE_NAME).then((cache) => cache.put(request, clone));
      }
      return response;
    }).catch(() => {
      if (request.destination === 'document') {
        return caches.match('/index.html');
      }
      return new Response('Offline', { status: 503, statusText: 'Offline' });
    });
  });
}
