if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
  /* role guard redirected */
}

Sidebar.init('challenges');

// ── Read URL params (from roadmap/dashboard navigation) ──
var urlParams = new URLSearchParams(window.location.search);
var initialTier = parseInt(urlParams.get('tier')) || 0;
var initialChallengeId = urlParams.get('id') || '';

var allChallenges = [];
var roadmapData = null;
var currentChallenge = null;
var activeTier = 0;
var challengeMap = {};

var TIER_NAMES = {
  1: 'أساسيات البرمجة',
  2: 'المنطق والتحكم',
  3: 'هياكل البيانات',
  4: 'البرمجة الكائنية',
  5: 'الخوارزميات وإدارة الذاكرة',
  6: 'المصفوفات',
  7: 'البرمجة الكائنية المتقدمة'
};
var DIFF_LABELS = { BEGINNER: 'مبتدئ', INTERMEDIATE: 'متوسط', ADVANCED: 'متقدم', EXPERT: 'خبير' };
var TIER_DIFF = { 1:'مبتدئ', 2:'متوسط', 3:'متقدم', 4:'متمرس', 5:'محترف', 6:'مخضرم', 7:'خبير' };

function loadAll() {
  var p1 = api.getChallenges();
  var p2 = api.getRoadmap ? api.getRoadmap() : Promise.resolve(null);
  return Promise.all([p1, p2]).then(function(results) {
    var raw = results[0].challenges || results[0] || [];
    roadmapData = results[1] ? (results[1].roadmap || results[1]) : null;

    challengeMap = {};
    if (roadmapData && roadmapData.lessons) {
      roadmapData.lessons.forEach(function(l) {
        if (l.challenges) l.challenges.forEach(function(c) {
          challengeMap[c.id] = c;
        });
      });
    }

    allChallenges = Array.isArray(raw) ? raw.map(function(c) {
      var rm = challengeMap[c.id] || {};
      return Object.assign({}, c, {
        passed: rm.passed || false,
        order: rm.order || c.order || 999,
        status: rm.status || 'LOCKED',
        xpReward: rm.xpReward || c.points || 0,
        isPractice: rm.isPractice || false,
        // SECURITY: expectedOutput is NEVER sent from roadmap - only used server-side
      });
    }) : [];

    buildTabs();
    // If URL has ?tier=, select that tier; otherwise default to tier 1
    var startTier = initialTier || 1;
    if (initialTier && getTierStatus(initialTier) === 'LOCKED') startTier = 1;
    selectTier(startTier);
    // If URL has ?id=, auto-select that challenge
    if (initialChallengeId) {
      var target = allChallenges.find(function(c) { return c.id === initialChallengeId; });
      if (target && target.status !== 'LOCKED') {
        setTimeout(function() { selectChallenge(initialChallengeId); }, 200);
      }
    }
  }).catch(function() {
    document.getElementById('challengesGrid').innerHTML =
      '<div class="empty-state" style="grid-column:1/-1"><p>فشل تحميل التحديات</p></div>';
  });
}

function getTierStatus(tier) {
  if (!roadmapData || !roadmapData.lessons) return tier === 1 ? 'UNLOCKED' : 'LOCKED';
  var lesson = roadmapData.lessons.find(function(l) { return l.order === tier; });
  return lesson ? lesson.status : 'LOCKED';
}

function buildTabs() {
  var html = '';
  var tiers = [1,2,3,4,5,6,7];
  tiers.forEach(function(t){
    var status = getTierStatus(t);
    var count = allChallenges.filter(function(c) { return c.tier === t; }).length;
    var passedCount = allChallenges.filter(function(c) { return c.tier === t && c.passed; }).length;
    var disabled = status === 'LOCKED';
    var activeClass = activeTier === t ? ' active' : '';
    var lockedClass = status === 'LOCKED' ? ' locked' : '';
    var check = passedCount === count && count > 0 ? '<span class="tab-check">&#10003;</span>' : '';
    html += '<button class="tab' + activeClass + lockedClass + '" onclick="selectTier(' + t + ')"' + (disabled ? ' disabled' : '') + '>' +
      check + TIER_NAMES[t] + '<span class="tab-count">(' + passedCount + '/' + count + ')</span></button>';
  });

  document.getElementById('tierTabs').innerHTML = html;
}

function selectTier(tier) {
  if (getTierStatus(tier) === 'LOCKED') {
    toast.error('احجز المستوى أولاً (أكمل 80% على الأقل)');
    return;
  }
  activeTier = tier;
  buildTabs();
  renderChallenges();
}

function renderChallenges() {
  var grid = document.getElementById('challengesGrid');
  var tierChallenges = allChallenges.filter(function(c) { return c.tier === activeTier; })
    .sort(function(a, b) { return a.order - b.order; });

  if (tierChallenges.length === 0) {
    grid.innerHTML = '<div class="empty-state" style="grid-column:1/-1"><p>لا توجد تحديات في هذا المستوى</p></div>';
    return;
  }

  grid.innerHTML = tierChallenges.map(function(c) {
    var activeClass = (currentChallenge && currentChallenge.id === c.id) ? ' active' : '';
    var passedClass = c.passed ? ' passed' : '';
    var locked = c.status === 'LOCKED';
    var cursorStyle = locked ? 'cursor:not-allowed;opacity:0.5;' : '';
    var onclick = locked ? '' : 'onclick="selectChallenge(\'' + c.id + '\')" role="button" tabindex="0"';
    var lockIcon = locked ? '<span style="margin-left:4px">&#128274;</span>' : '';
    var checkIcon = c.passed ? '<span style="color:var(--primary);margin-left:4px">&#10003;</span>' : '';
    var practiceBadge = '<span style="background:rgba(16,185,129,0.15);color:var(--primary);padding:2px 8px;border-radius:8px;font-size:11px;display:inline-block;margin-right:6px;border:1px solid var(--primary);font-weight:600">درس</span>';
    var orderNum = '<span style="font-size:11px;color:var(--text-muted);margin-left:6px">#' + c.order + '</span>';

    return '<div class="challenge-card' + activeClass + passedClass + '" ' + onclick + ' style="' + cursorStyle + '">' +
      '<div>' + practiceBadge + orderNum +
        '<span class="challenge-tier-badge diff-' + c.difficulty + '">' + (TIER_DIFF[c.tier] || DIFF_LABELS[c.difficulty] || c.difficulty) + '</span>' +
      '</div>' +
      '<div style="font-size:15px;font-weight:700;margin:8px 0">' + escapeHtml(c.title || '') + lockIcon + checkIcon + '</div>' +
      '<div style="color:var(--text-muted);font-size:13px;line-height:1.6">' + escapeHtml((c.description || 'تحدي في البرمجة').substring(0, 100)) + '...</div>' +
      '<div style="display:flex;justify-content:space-between;align-items:center;margin-top:12px">' +
        '<span style="font-size:13px;font-weight:700;color:var(--primary)">' + (c.xpReward || c.points || 0) + ' XP</span>' +
      '</div>' +
    '</div>';
  }).join('');
}

function selectChallenge(id) {
  currentChallenge = allChallenges.find(function(c) { return c.id === id; });
  if (!currentChallenge) return;
  if (currentChallenge.status === 'LOCKED') {
    toast.error('يجب إكمال التحديات السابقة أولاً');
    return;
  }
  renderChallenges();
  renderSolution();
  // Restore auto-saved code if any
  restoreChallengeCode(id);
}

// Save code on page refresh/close
window.addEventListener('beforeunload', function() {
  if (currentChallenge) {
    var editor = document.getElementById('codeEditor');
    if (editor) {
      localStorage.setItem('dhad_challenge_autosave_' + currentChallenge.id, editor.value);
    }
  }
});

function renderSolution() {
  var c = currentChallenge;
  var panel = document.getElementById('solutionPanel');
  var html = '<div class="solution-title">' +
    (c.isPractice ? '<span style="background:rgba(16,185,129,0.15);color:var(--primary);padding:4px 12px;border-radius:8px;font-size:13px;display:inline-block;margin-left:8px;border:1px solid var(--primary);font-weight:600">درس</span>' :
     '<span style="background:rgba(245,158,11,0.1);color:var(--accent);padding:4px 12px;border-radius:8px;font-size:13px;display:inline-block;margin-left:8px;border:1px solid var(--accent);font-weight:600">درس</span>') +
    escapeHtml(c.title || '') +
    '</div>' +
    '<div class="solution-meta">' +
      '<span class="challenge-tier-badge tier-' + c.tier + '">' + TIER_NAMES[c.tier] + ' - #' + c.order + '</span>' +
      '<span class="challenge-tier-badge diff-' + c.difficulty + '">' + (TIER_DIFF[c.tier] || DIFF_LABELS[c.difficulty] || c.difficulty) + '</span>' +
      '<span class="solution-item" style="font-size:15px;font-weight:700;color:var(--primary)">' + (c.xpReward || c.points || 0) + ' XP</span>' +
    '</div>' +
    '<div class="solution-desc">' + escapeHtml(c.description || '').replace(/\n/g, '<br>') + '</div>' +
    '<div style="margin-top:16px">' +
      '<label class="form-label">الكود <span style="font-size:12px;color:var(--text-muted)">(اضغط للتعديل)</span></label>' +
      '<div class="code-highlight" id="codeHighlight" onclick="toggleEditMode()" style="background:#0f172a;color:#e2e8f0;padding:16px;border-radius:12px;font-family:monospace;font-size:14px;direction:rtl;text-align:right;overflow-x:auto;margin-bottom:8px;line-height:1.8;white-space:pre-wrap;border-left:4px solid #10b981;min-height:60px;cursor:pointer" title="اضغط للتعديل">' + (DhadInterpreter.highlight ? DhadInterpreter.highlight(c.starterCode || '// اكتب كودك هنا') : escapeHtml(c.starterCode || '// اكتب كودك هنا')) + '</div>' +
      '<textarea class="editor-area" id="codeEditor" rows="10" spellcheck="false" style="display:none" onblur="exitEditMode()" oninput="autoSaveChallengeCode()">' + escapeHtml(c.starterCode || '// اكتب كودك هنا') + '</textarea>' +
    '</div>' +
    '<div class="actions">' +
      '<button class="btn btn-success btn-sm" onclick="runCode()">تنفيذ</button>' +
      '<button class="btn btn-primary btn-sm" onclick="submitChallenge()">تسليم الحل</button>' +
    '</div>' +
    '<div style="margin-top:12px">' +
      '<label class="form-label">المخرجات</label>' +
      '<div class="output-box" id="codeOutput">جاهز للتنفيذ...</div>' +
    '</div>' +
    '<div class="result-bar" id="resultBar"></div>';

  if (c.passed) {
    html += '<div class="result-bar already-passed" style="display:block">&#10003; تم اجتياز هذا التحدي مسبقاً - حصلت على ' + (c.xpReward || c.points) + ' XP</div>';
  }

  document.getElementById('solutionContent').innerHTML = html;
  panel.style.display = 'block';
  panel.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

function toggleEditMode() {
  var highlight = document.getElementById('codeHighlight');
  var editor = document.getElementById('codeEditor');
  if (highlight && editor) {
    highlight.style.display = 'none';
    editor.style.display = 'block';
    editor.focus();
  }
}

function exitEditMode() {
  setTimeout(function() {
    var highlight = document.getElementById('codeHighlight');
    var editor = document.getElementById('codeEditor');
    if (highlight && editor) {
      highlight.innerHTML = DhadInterpreter.highlight ? DhadInterpreter.highlight(editor.value) : escapeHtml(editor.value);
      highlight.style.display = 'block';
      editor.style.display = 'none';
    }
  }, 100);
}

var _challengeAutoSaveTimer = null;
function autoSaveChallengeCode() {
  clearTimeout(_challengeAutoSaveTimer);
  _challengeAutoSaveTimer = setTimeout(function() {
    var editor = document.getElementById('codeEditor');
    if (editor && currentChallenge) {
      localStorage.setItem('dhad_challenge_autosave_' + currentChallenge.id, editor.value);
    }
  }, 2000);
}

function restoreChallengeCode(challengeId) {
  var saved = localStorage.getItem('dhad_challenge_autosave_' + challengeId);
  if (saved !== null) {
    var editor = document.getElementById('codeEditor');
    if (editor) editor.value = saved;
  }
}

function normalize(str) {
  return SmartEvaluator.normalize(str);
}

function runCode() {
  if (!currentChallenge) return;
  var codeEl = document.getElementById('codeEditor');
  if (!codeEl) return;
  var code = codeEl.value;
  var outputEl = document.getElementById('codeOutput');
  var resultBar = document.getElementById('resultBar');
  outputEl.textContent = 'جاري التنفيذ...';
  resultBar.className = 'result-bar';
  resultBar.style.display = 'none';

  // Update highlighted display
  var highlightEl = document.getElementById('codeHighlight');
  if (highlightEl && DhadInterpreter.highlight) {
    highlightEl.innerHTML = DhadInterpreter.highlight(code);
  }

  setTimeout(function() {
    var result = DhadInterpreter.execute(code);
    if (result.errors.length > 0) {
      outputEl.innerHTML = '<span style="color:#ef4444">خطأ:\n' + escapeHtml(result.errors.join('\n')) + '</span>';
      resultBar.className = 'result-bar error';
      resultBar.textContent = 'يوجد أخطاء في الكود';
    } else {
      outputEl.textContent = result.output || '(لا توجد مخرجات)';
      if (result.output && result.output.trim()) {
        resultBar.className = 'result-bar pass';
        resultBar.textContent = 'تم التنفيذ بنجاح - اضغط "تسليم الحل" للتحقق';
      } else {
        resultBar.className = 'result-bar';
        resultBar.textContent = 'تم التنفيذ - لا توجد مخرجات';
      }
    }
  }, 100);
}

function submitChallenge() {
  if (!currentChallenge) return;
  var code = document.getElementById('codeEditor').value;
  if (!code.trim()) { toast.error('اكتب الكود أولاً'); return; }

  // ═══════════════════════════════════════════════════════════════
  // التقييم الذكي في المتصفح
  // ═══════════════════════════════════════════════════════════════
  var evalResult = SmartEvaluator.evaluate(code, currentChallenge);

  // عرض النتائج فوراً
  var resultBar = document.getElementById('resultBar');
  var outputEl = document.getElementById('codeOutput');

  if (evalResult.codeErrors.length > 0) {
    outputEl.textContent = evalResult.actualOutput || '(لا توجد مخرجات)';
    resultBar.className = 'result-bar error';
    resultBar.textContent = evalResult.codeErrors[0];
    resultBar.style.display = 'block';
    toast.error(evalResult.codeErrors[0]);
    return;
  }

  if (evalResult.suspicious.suspicious) {
    outputEl.textContent = evalResult.actualOutput || '(لا توجد مخرجات)';
    resultBar.className = 'result-bar fail';
    resultBar.textContent = 'تم اكتشاف نشاط مشبوه — حاول استخدام متغيرات وعمليات حقيقية';
    resultBar.style.display = 'block';
    toast.error('نشاط مشبوه — حاول بطريقة مختلفة');
    return;
  }

  // ═══════════════════════════════════════════════════════════════
  // إرسال للسيرفر — challengeId + code + output
  // السيرفر مصدر الحقيقة الوحيد
  // ═══════════════════════════════════════════════════════════════
  function attemptSubmit(retries) {
    api.submitSolution({
      challengeId: currentChallenge.id,
      code: code,
      output: evalResult.actualOutput || ''
    }).then(function(res) {
      if (res && res.alreadyPassed) {
        toast.info('تم اجتياز هذا التحدي مسبقاً');
        loadAll().then(function() { goToNextChallenge(); });
      } else if (res && res.xpAwarded > 0) {
        toast.success('أحسنت! حصلت على ' + res.xpAwarded + ' XP (النتيجة: ' + res.score + ')');
        currentChallenge.passed = true;
        currentChallenge.status = 'COMPLETED';
        localStorage.removeItem('dhad_challenge_autosave_' + currentChallenge.id);
        loadAll().then(function() { goToNextChallenge(); });
      } else if (res && res.passed) {
        toast.success('تم تسليم الحل بنجاح!');
        currentChallenge.passed = true;
        currentChallenge.status = 'COMPLETED';
        localStorage.removeItem('dhad_challenge_autosave_' + currentChallenge.id);
        loadAll().then(function() { goToNextChallenge(); });
      } else {
        resultBar.className = 'result-bar fail';
        resultBar.textContent = res.message || 'المخرجات غير مطابقة';
        resultBar.style.display = 'block';
        toast.error(res.message || 'حاول مرة أخرى');
      }
      renderSolution();
    }).catch(function() {
      if (retries > 0) {
        setTimeout(function() { attemptSubmit(retries - 1); }, 1000);
      } else {
        toast.error('فشل تسليم الحل — تحقق من اتصال الشبكة');
      }
    });
  }
  attemptSubmit(2);
}

function goToNextChallenge() {
  if (!currentChallenge) return;
  var tierChallenges = allChallenges.filter(function(c) { return c.tier === activeTier; })
    .sort(function(a, b) { return a.order - b.order; });
  var idx = tierChallenges.findIndex(function(c) { return c.id === currentChallenge.id; });
  if (idx >= 0 && idx < tierChallenges.length - 1) {
    var next = tierChallenges[idx + 1];
    setTimeout(function() { selectChallenge(next.id); }, 800);
  } else {
    var nextTier = activeTier + 1;
    if (TIER_NAMES[nextTier] && getTierStatus(nextTier) !== 'LOCKED') {
      setTimeout(function() {
        activeTier = nextTier;
        buildTabs();
        renderChallenges();
        var nextTierChallenges = allChallenges.filter(function(c) { return c.tier === nextTier; })
          .sort(function(a, b) { return a.order - b.order; });
        if (nextTierChallenges.length > 0) {
          selectChallenge(nextTierChallenges[0].id);
        }
      }, 800);
    } else if (TIER_NAMES[nextTier]) {
      setTimeout(function() { toast.info('أكمل المستوى الحالي أولاً (80% على الأقل)'); }, 800);
    } else {
      setTimeout(function() { toast.success('أحسنت! أنهيت كل التحديات'); }, 800);
    }
  }
}

loadAll();
