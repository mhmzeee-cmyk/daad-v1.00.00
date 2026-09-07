/* Developer: محمد محمود الحموز | Dhad Studio */
/* ====================================================
   Dhad Studio - Theme Switcher
   ==================================================== */

var ThemeSwitcher = (function() {
  var STORAGE_KEY = 'dhad_theme';
  var DEFAULT_THEME = 'light';
  var transitionTimer = null;

  var THEMES = {
    terminal: { name: 'تيرمينال أخضر', desc: 'أسود مع أخضر فلوري' },
    purple: { name: 'بنفسجي كلاسيك', desc: 'كحلي مع بنفسجي' },
    dark: { name: 'داكن أزرق', desc: 'رمادي غامق مع أزرق' },
    light: { name: 'فاتح', desc: 'أبيض مع أزرق' },
    ocean: { name: 'محيط أزرق', desc: 'أزرق غامق مع سماوي' }
  };

  function getSaved() {
    try { return localStorage.getItem(STORAGE_KEY) || DEFAULT_THEME; }
    catch(e) { return DEFAULT_THEME; }
  }

  function setTheme(id) {
    if (!THEMES[id]) return;
    
    // Clear any existing transition timer
    if (transitionTimer) {
      clearTimeout(transitionTimer);
      transitionTimer = null;
    }
    
    // Add transition class for smooth animation
    document.documentElement.classList.add('theme-transition');
    
    // Apply the new theme
    document.documentElement.setAttribute('data-theme', id);
    
    // Save to localStorage
    try { localStorage.setItem(STORAGE_KEY, id); } catch(e) {}
    
    // Remove transition class after animation completes
    transitionTimer = setTimeout(function() {
      document.documentElement.classList.remove('theme-transition');
      transitionTimer = null;
    }, 500);
  }

  function getCurrent() {
    return document.documentElement.getAttribute('data-theme') || getSaved();
  }

  function init() {
    setTheme(getSaved());
  }

  function renderCard(current) {
    var html = '<div class="themes-grid">';
    var keys = Object.keys(THEMES);
    for (var i = 0; i < keys.length; i++) {
      var k = keys[i];
      var t = THEMES[k];
      var active = k === current ? ' theme-card-active' : '';
      html += '<div class="theme-card' + active + '" onclick="ThemeSwitcher.apply(\'' + k + '\')">' +
        '<div class="theme-preview" data-preview="' + k + '">' +
          '<div class="preview-bar"></div>' +
          '<div class="preview-body">' +
            '<div class="preview-sidebar"></div>' +
            '<div class="preview-content">' +
              '<div class="preview-line line1"></div>' +
              '<div class="preview-line line2"></div>' +
              '<div class="preview-line line3"></div>' +
            '</div>' +
          '</div>' +
        '</div>' +
        '<div class="theme-info">' +
          '<span class="theme-name">' + t.name + '</span>' +
          '<span class="theme-desc">' + t.desc + '</span>' +
        '</div>' +
        '<div class="theme-check">' + (k === current ? '&#10003;' : '') + '</div>' +
      '</div>';
    }
    html += '</div>';
    return html;
  }

  function apply(id) {
    // Add visual feedback
    var cards = document.querySelectorAll('.theme-card');
    for (var i = 0; i < cards.length; i++) {
      cards[i].classList.remove('theme-card-active');
      cards[i].style.transform = 'scale(0.95)';
      var check = cards[i].querySelector('.theme-check');
      if (check) check.innerHTML = '';
    }
    
    // Apply new theme with animation
    setTheme(id);
    
    // Animate the selected card
    setTimeout(function() {
      var idx = Object.keys(THEMES).indexOf(id);
      if (idx >= 0 && cards[idx]) {
        cards[idx].classList.add('theme-card-active');
        cards[idx].style.transform = 'scale(1.02)';
        var check = cards[idx].querySelector('.theme-check');
        if (check) check.innerHTML = '&#10003;';
        
        // Reset scale after animation
        setTimeout(function() {
          cards[idx].style.transform = '';
        }, 200);
      }
    }, 50);
  }

  return {
    THEMES: THEMES,
    init: init,
    getCurrent: getCurrent,
    apply: apply,
    renderCard: renderCard
  };
})();

/* Auto-init on load */
ThemeSwitcher.init();
