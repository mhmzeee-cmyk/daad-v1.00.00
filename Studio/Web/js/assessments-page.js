if (!api.isLoggedIn()) {
  window.location.href = '../login.html';
} else if (api._enforceRoleGuard()) {
}

var role = api.getRole();
var isTeacher = (role === 'TEACHER' || role === 'ADMIN');

Sidebar.init('assessments');

if (isTeacher) {
  document.getElementById('createBtn').style.display = '';
  document.getElementById('tableHead').innerHTML =
    '<tr><th>الاسم</th><th>النوع</th><th>الفصل</th><th>المدة</th><th>الحالة</th><th>النتائج</th></tr>';
} else {
  document.getElementById('tableHead').innerHTML =
    '<tr><th>الاسم</th><th>النوع</th><th>المدة</th><th>الحالة</th><th>الإجرا</th></tr>';
}

var assessments = [];
var currentAssessmentType = 'CODING';
var questionsList = [];
var questionIdCounter = 0;

function selectType(type) {
  currentAssessmentType = type;
  document.getElementById('typeCoding').className = 'type-btn' + (type === 'CODING' ? ' selected' : '');
  document.getElementById('typeMC').className = 'type-btn' + (type === 'MULTIPLE_CHOICE' ? ' selected' : '');
  document.getElementById('codingSection').style.display = type === 'CODING' ? '' : 'none';
  document.getElementById('mcSection').style.display = type === 'MULTIPLE_CHOICE' ? '' : 'none';
}

function addQuestion() {
  questionIdCounter++;
  var qId = questionIdCounter;
  questionsList.push({ id: qId, text: '', options: ['','','',''], correctIndex: 0 });
  renderQuestions();
}

function removeQuestion(qId) {
  questionsList = questionsList.filter(function(q) { return q.id !== qId; });
  renderQuestions();
}

function updateQuestionText(qId, text) {
  var q = questionsList.find(function(q) { return q.id === qId; });
  if (q) q.text = text;
}

function updateOptionText(qId, optIndex, text) {
  var q = questionsList.find(function(q) { return q.id === qId; });
  if (q) q.options[optIndex] = text;
}

function setCorrectOption(qId, optIndex) {
  var q = questionsList.find(function(q) { return q.id === qId; });
  if (q) q.correctIndex = optIndex;
}

function renderQuestions() {
  var container = document.getElementById('questionsContainer');
  if (questionsList.length === 0) {
    container.innerHTML = '<div class="empty-state"><p>لا توجد أسئلة بعد. اضغط "+ سؤال جديد" للإضافة.</p></div>';
    return;
  }
  container.innerHTML = questionsList.map(function(q, qi) {
    var optionsHtml = q.options.map(function(opt, oi) {
      var labels = ['أ', 'ب', 'ج', 'د'];
      var isCorrect = q.correctIndex === oi;
      return '<div class="option-row">' +
        '<input type="radio" name="correct_' + q.id + '" ' + (isCorrect ? 'checked' : '') + ' onclick="setCorrectOption(' + q.id + ',' + oi + ')">' +
        '<span class="correct-label">' + labels[oi] + '</span>' +
        '<input type="text" value="' + (opt || '').replace(/"/g, '&quot;') + '" placeholder="الخيار ' + labels[oi] + '" oninput="updateOptionText(' + q.id + ',' + oi + ',this.value)">' +
        '</div>';
    }).join('');

    return '<div class="question-card">' +
      '<div class="question-header">' +
      '<span class="question-num">سؤال ' + (qi + 1) + '</span>' +
      '<button type="button" class="remove-btn" onclick="removeQuestion(' + q.id + ')">حذف</button>' +
      '</div>' +
      '<div class="form-group">' +
      '<input type="text" value="' + (q.text || '').replace(/"/g, '&quot;') + '" placeholder="اكتب السؤال هنا..." oninput="updateQuestionText(' + q.id + ',this.value)" style="width:100%">' +
      '</div>' +
      '<div>' + optionsHtml + '</div>' +
      '</div>';
  }).join('');
}

function loadAssessments() {
  var p = isTeacher ? api.getAssessments() : api.getStudentAssessments();
  p.then(function(res) {
    assessments = Array.isArray(res) ? res : (res.assessments || []);
    renderAssessments();
    if (isTeacher) loadClassroomsForSelect();
  }).catch(function() { toast.error('\u0641\u0634\u0644 \u062a\u062d\u0645\u064a\u0644 \u0627\u0644\u0627\u0645\u062a\u062d\u0627\u0646\u0627\u062a'); });
}

function renderAssessments() {
  var tbody = document.getElementById('assessmentsTable');
  if (assessments.length === 0) {
    tbody.innerHTML = '<tr><td colspan="6" class="empty-state"><p>\u0644\u0627 \u064a\u0648\u062c\u062f \u0627\u0645\u062a\u062d\u0627\u0646\u0627\u062a \u0628\u0639\u062f</p></td></tr>';
    return;
  }
  if (isTeacher) {
    tbody.innerHTML = assessments.map(function(a) {
      var typeLabel = a.assessmentType === 'MULTIPLE_CHOICE' ? '\u0627\u062e\u062a\u064a\u0627\u0631\u0627\u062a' : '\u0628\u0631\u0645\u062c\u064a';
      var typeBadge = a.assessmentType === 'MULTIPLE_CHOICE' ? 'badge-warning' : 'badge-info';
      return '<tr><td><strong>' + escapeHtml(a.title) + '</strong></td>' +
        '<td><span class="badge ' + typeBadge + '">' + typeLabel + '</span></td>' +
        '<td>' + escapeHtml(a.classroom && a.classroom.name || '\u2014') + '</td>' +
        '<td>' + (a.allowedTime || 60) + ' \u062f\u0642\u064a\u0642\u0629</td>' +
        '<td><span class="badge ' + (a.isActive ? 'badge-success' : 'badge-warning') + '">' + (a.isActive ? '\u0646\u0634\u0637' : '\u0645\u063a\u0644\u0642') + '</span></td>' +
        '<td><button class="btn btn-secondary btn-sm" onclick="showResults(\'' + a.id + '\', \'' + escapeHtml(a.title || '').replace(/'/g, "\\'") + '\')">\u0627\u0644\u0646\u062a\u0627\u0626\u062c</button></td></tr>';
    }).join('');
  } else {
    tbody.innerHTML = assessments.map(function(a) {
      var now = new Date();
      var start = a.startTime ? new Date(a.startTime) : null;
      var end = a.endTime ? new Date(a.endTime) : null;
      var isActive = (a.status === 'active' || (start && end && now >= start && now <= end)) && !a.hasSubmitted;
      var isCompleted = a.hasSubmitted || a.status === 'completed';
      var statusText = isCompleted ? '\u0645\u0643\u062a\u0645\u0644' : a.status === 'active' ? '\u0646\u0634\u0637' : '\u0642\u0627\u062f\u0645';
      var badgeClass = isCompleted ? 'badge-success' : a.status === 'active' ? 'badge-success' : 'badge-warning';
      var actionBtn = isActive
        ? '<button class="btn btn-primary btn-sm" onclick="startAssessment(\'' + a.id + '\')">\u0627\u0628\u062f\u0623</button>'
        : isCompleted ? '<span style="color:var(--secondary);font-size:13px">\u0645\u0643\u062a\u0645\u0644</span>' : '<span style="color:var(--text-muted);font-size:12px">\u2014</span>';
      var typeLabel = a.assessmentType === 'MULTIPLE_CHOICE' ? '\u0627\u062e\u062a\u064a\u0627\u0631\u0627\u062a' : '\u0628\u0631\u0645\u062c\u064a';
      return '<tr><td><strong>' + escapeHtml(a.title) + '</strong></td>' +
        '<td>' + typeLabel + '</td>' +
        '<td>' + (a.allowedTime || 60) + ' \u062f\u0642\u064a\u0642\u0629</td>' +
        '<td><span class="badge ' + badgeClass + '">' + statusText + '</span></td>' +
        '<td>' + actionBtn + '</td></tr>';
    }).join('');
  }
}

var _currentAssessmentId = null;

function startAssessment(id) {
  var assessment = assessments.find(function(a) { return a.id === id; });
  if (!assessment) { toast.error('\u0627\u0644\u0627\u0645\u062a\u062d\u0627\u0646 \u063a\u064a\u0631 \u0645\u0648\u062c\u0648\u062f'); return; }
  if (assessment.hasSubmitted) { toast.error('\u0644\u0642\u062f \u0646\u0642\u0645\u062a \u0647\u0630\u0627 \u0627\u0644\u0627\u0645\u062a\u062d\u0627\u0646 \u0645\u0633\u0628\u0642\u0627'); return; }
  _currentAssessmentId = id;
  api.startAssessmentAttempt(id).then(function(r) {
    if (r && r.error) { toast.error(r.message || '\u0641\u0634\u0644 \u0628\u062f\u0621 \u0627\u0644\u0627\u0645\u062a\u062d\u0627\u0646'); return; }
    showAssessmentModal({ assessment: assessment });
  }).catch(function() { toast.error('\u0641\u0634\u0644 \u0628\u062f\u0621 \u0627\u0644\u0627\u0645\u062a\u062d\u0627\u0646'); });
}

var _assessmentTimer = null;
var _assessmentStartTime = null;
var _assessmentDuration = 0;

function startAssessmentTimer(minutes) {
  _assessmentDuration = minutes * 60;
  _assessmentStartTime = Date.now();
  var timerEl = document.getElementById('assessmentTimer');
  if (!timerEl) return;
  _assessmentTimer = setInterval(function() {
    var elapsed = Math.floor((Date.now() - _assessmentStartTime) / 1000);
    var remaining = _assessmentDuration - elapsed;
    if (remaining <= 0) {
      clearInterval(_assessmentTimer);
      timerEl.textContent = '00:00';
      timerEl.style.color = '#ef4444';
      toast.error('انتهى الوقت! جارِ التسليم التلقائي...');
      autoSubmitAssessment();
      return;
    }
    var mins = Math.floor(remaining / 60);
    var secs = remaining % 60;
    timerEl.textContent = (mins < 10 ? '0' : '') + mins + ':' + (secs < 10 ? '0' : '') + secs;
    if (remaining <= 60) timerEl.style.color = '#ef4444';
    else if (remaining <= 300) timerEl.style.color = '#f59e0b';
  }, 1000);
}

function autoSubmitAssessment() {
  if (_currentAssessmentId && window._currentAssessmentType === 'MULTIPLE_CHOICE') {
    var answersArr = [];
    var questions = window._currentAssessmentQuestions || [];
    for (var i = 0; i < questions.length; i++) { answersArr.push(mcAnswers[i]); }
    api.completeAssessment(_currentAssessmentId, answersArr).then(function() {
      toast.success('تم التسليم التلقائي');
      closeAssessmentModal();
    }).catch(function() {});
  } else if (_currentAssessmentId && window._currentAssessmentType === 'CODING') {
    api.completeAssessment(_currentAssessmentId).then(function() {
      toast.success('تم التسليم التلقائي');
      closeAssessmentModal();
    }).catch(function() {});
  }
}

function showAssessmentModal(data) {
  var a = data.assessment || {};
  var questions = [];
  try { questions = typeof a.questions === 'string' ? JSON.parse(a.questions) : (a.questions || []); } catch(e) { questions = []; }
  var type = a.assessmentType || 'CODING';
  window._currentAssessmentType = type;
  // Store first challengeId for coding assessments
  var challengeIds = [];
  try { challengeIds = typeof a.challengeIds === 'string' ? JSON.parse(a.challengeIds) : (a.challengeIds || []); } catch(e) { challengeIds = []; }
  window._currentCodingChallengeId = (challengeIds.length > 0) ? challengeIds[0] : '';
  window._currentAssessmentQuestions = questions;
  var container = document.getElementById('assessmentContent');
  var titleEl = document.getElementById('assessmentModalTitle');
  titleEl.textContent = a.title || 'الامتحان';

  var html = '<div style="margin-bottom:16px;display:flex;justify-content:space-between;align-items:center"><span style="color:var(--text-muted);font-size:13px">الوقت المتبقي: <strong id="assessmentTimer" style="font-size:16px;color:#10b981">--:--</strong></span></div>';

  if (type === 'MULTIPLE_CHOICE' && questions.length > 0) {
    html += '<div id="mcAnswers">';
    questions.forEach(function(q, qi) {
      var labels = ['\u0623', '\u0628', '\u062c', '\u062f'];
      html += '<div class="student-view-card"><div class="question-text"><strong>سؤال ' + (qi+1) + ':</strong> ' + escapeHtml(q.text) + '</div><div class="choices">';
      q.options.forEach(function(opt, oi) {
        html += '<button type="button" class="choice-btn" data-q="' + qi + '" data-o="' + oi + '" onclick="selectAnswer(' + qi + ',' + oi + ')">' + labels[oi] + '. ' + escapeHtml(opt) + '</button>';
      });
      html += '</div></div>';
    });
    html += '</div>';
    html += '<div style="margin-top:16px"><button class="btn btn-primary" onclick="submitMCAnswers(' + questions.length + ')">تسليم الإجابات</button></div>';
  } else if (type === 'CODING') {
    var instructions = a.instructions || '';
    var starterCode = a.starterCode || '';
    var expectedOutput = a.expectedOutput || '';
    html += '<div class="student-view-card">';
    if (instructions) html += '<div class="question-text">' + escapeHtml(instructions).replace(/\n/g, '<br>') + '</div>';
    if (expectedOutput) html += '<div style="margin-bottom:12px;font-size:13px;color:var(--text-muted)">المخرجات المتوقعة: <span style="background:#0f172a;color:#fbbf24;padding:4px 8px;border-radius:4px;font-family:monospace;font-size:12px;direction:rtl">' + (DhadInterpreter.highlight ? DhadInterpreter.highlight(expectedOutput) : expectedOutput) + '</span></div>';
    html += '<div style="margin-bottom:8px;font-size:13px;color:var(--text-muted)">الكود:</div>';
    html += '<div id="assessmentCodeHighlight" style="background:#0f172a;color:#e2e8f0;padding:16px;border-radius:8px;font-family:monospace;font-size:14px;direction:rtl;text-align:right;overflow-x:auto;margin-bottom:8px;line-height:1.8;white-space:pre-wrap;border-left:4px solid #10b981;min-height:60px;cursor:pointer" onclick="toggleAssessmentEdit()" title="اضغط للتعديل">' + (DhadInterpreter.highlight ? DhadInterpreter.highlight(starterCode || '// اكتب كودك هنا') : (starterCode || '// اكتب كودك هنا')) + '</div>';
    html += '<textarea id="assessmentCode" style="width:100%;min-height:150px;font-family:monospace;padding:12px;border:1px solid var(--border);border-radius:8px;background:var(--bg-secondary);color:var(--text);direction:rtl;text-align:right;display:none" onblur="exitAssessmentEdit()" oninput="autoSaveAssessmentCode()">' + (starterCode || '') + '</textarea>';
    html += '</div>';
    html += '<div style="margin-top:12px;display:flex;gap:8px"><button class="btn btn-secondary" onclick="runAssessmentCode()">تنفيذ</button><button class="btn btn-primary" onclick="submitCodingAnswer()">تسليم</button></div>';
    html += '<pre id="assessmentOutput" style="margin-top:12px;background:#0f172a;color:#e2e8f0;border:1px solid var(--border);border-radius:8px;padding:12px;min-height:40px;font-size:13px;direction:rtl;text-align:right;white-space:pre-wrap;font-family:monospace"></pre>';
  }

  container.innerHTML = html;
  document.getElementById('assessmentModal').classList.add('active');
  window._currentAssessmentQuestions = questions;
  // Start real countdown timer
  startAssessmentTimer(a.allowedTime || 60);
  // Restore saved code if any
  restoreAssessmentCode();
  // Save progress on page refresh
  window.addEventListener('beforeunload', function() {
    if (_currentAssessmentId && window._currentAssessmentType === 'CODING') {
      var editor = document.getElementById('assessmentCode');
      if (editor) {
        localStorage.setItem('dhad_assessment_autosave', JSON.stringify({ id: _currentAssessmentId, code: editor.value }));
      }
    }
  });
}

var mcAnswers = {};
function selectAnswer(qi, oi) {
  mcAnswers[qi] = oi;
  var btns = document.querySelectorAll('[data-q="' + qi + '"]');
  for (var i = 0; i < btns.length; i++) {
    btns[i].className = 'choice-btn' + (i === oi ? ' selected' : '');
  }
}

function submitMCAnswers(total) {
  var answered = 0;
  for (var k in mcAnswers) { if (mcAnswers.hasOwnProperty(k)) answered++; }
  if (answered < total) {
    toast.error('أجب على جميع الأسئلة (' + answered + '/' + total + ')');
    return;
  }
  if (!_currentAssessmentId) { toast.error('خطأ'); return; }
  // Build answers array: mcAnswers is {0: 2, 1: 3, ...} → [2, 3, ...]
  var answersArr = [];
  for (var i = 0; i < total; i++) { answersArr.push(mcAnswers[i]); }
  api.completeAssessment(_currentAssessmentId, answersArr).then(function(data) {
    var score = data && data.result ? data.result.score : 0;
    toast.success('نتيجتك: ' + score + '%');
    closeAssessmentModal();
    mcAnswers = {};
    _currentAssessmentId = null;
    loadAssessments();
  }).catch(function() { toast.error('فشل تسليم الإجابة'); });
}

function runAssessmentCode() {
  var code = document.getElementById('assessmentCode').value;
  var outputEl = document.getElementById('assessmentOutput');
  if (typeof DhadInterpreter !== 'undefined') {
    var result = DhadInterpreter.execute(code);
    outputEl.textContent = result.output || result.errors.join('\n') || '(لا توجد مخرجات)';
    // Update highlighted display
    var highlightEl = document.getElementById('assessmentCodeHighlight');
    if (highlightEl && DhadInterpreter.highlight) {
      highlightEl.innerHTML = DhadInterpreter.highlight(code);
    }
  } else {
    outputEl.textContent = 'المحلل غير متاح';
  }
}

function toggleAssessmentEdit() {
  var highlight = document.getElementById('assessmentCodeHighlight');
  var editor = document.getElementById('assessmentCode');
  if (highlight && editor) {
    highlight.style.display = 'none';
    editor.style.display = 'block';
    editor.focus();
  }
}

function exitAssessmentEdit() {
  setTimeout(function() {
    var highlight = document.getElementById('assessmentCodeHighlight');
    var editor = document.getElementById('assessmentCode');
    if (highlight && editor) {
      highlight.innerHTML = DhadInterpreter.highlight ? DhadInterpreter.highlight(editor.value) : escapeHtml(editor.value);
      highlight.style.display = 'block';
      editor.style.display = 'none';
    }
  }, 100);
}

function submitCodingAnswer() {
  var code = document.getElementById('assessmentCode').value;
  if (!_currentAssessmentId) { toast.error('خطأ'); return; }
  if (!code || !code.trim()) { toast.error('اكتب الكود أولاً'); return; }
  var challengeId = window._currentCodingChallengeId || '';
  if (!challengeId) { toast.error('خطأ: لم يتم تحديد التحدي'); return; }
  api.submitAssessmentAnswer({
    assessmentId: _currentAssessmentId,
    challengeId: challengeId,
    code: code
  }).then(function() {
    return api.completeAssessment(_currentAssessmentId);
  }).then(function() {
    toast.success('تم تسليم الحل بنجاح!');
    localStorage.removeItem('dhad_assessment_autosave');
    closeAssessmentModal();
    _currentAssessmentId = null;
    loadAssessments();
  }).catch(function() { toast.error('فشل تسليم الحل'); });
}

var _assessmentAutoSaveTimer = null;
function autoSaveAssessmentCode() {
  clearTimeout(_assessmentAutoSaveTimer);
  _assessmentAutoSaveTimer = setTimeout(function() {
    var code = document.getElementById('assessmentCode');
    if (code && _currentAssessmentId) {
      localStorage.setItem('dhad_assessment_autosave', JSON.stringify({ id: _currentAssessmentId, code: code.value }));
    }
  }, 2000);
}

function restoreAssessmentCode() {
  try {
    var saved = JSON.parse(localStorage.getItem('dhad_assessment_autosave') || 'null');
    if (saved && saved.id === _currentAssessmentId) {
      var editor = document.getElementById('assessmentCode');
      if (editor) editor.value = saved.code;
    }
  } catch(e) {}
}

function closeAssessmentModal() {
  if (_assessmentTimer) { clearInterval(_assessmentTimer); _assessmentTimer = null; }
  document.getElementById('assessmentModal').classList.remove('active');
}

function loadClassroomsForSelect() {
  api.getClassrooms().then(function(res) {
    var classrooms = Array.isArray(res) ? res : (res.classrooms || []);
    var select = document.getElementById('examClassroom');
    select.innerHTML = '<option value="">\u0627\u062e\u062a\u0631 \u0627\u0644\u0641\u0635\u0644</option>';
    classrooms.forEach(function(c) {
      var opt = document.createElement('option');
      opt.value = c.id;
      opt.textContent = c.name;
      select.appendChild(opt);
    });
  }).catch(function() {});
}

function showCreateModal() {
  document.getElementById('createModal').classList.add('active');
}

function closeModal() {
  document.getElementById('createModal').classList.remove('active');
}

document.getElementById('createForm').addEventListener('submit', function(e) {
  e.preventDefault();
  var startTimeVal = document.getElementById('examStartTime').value;
  var payload = {
    title: document.getElementById('examTitle').value,
    classroomId: document.getElementById('examClassroom').value,
    durationMinutes: parseInt(document.getElementById('examDuration').value),
    startTime: startTimeVal ? new Date(startTimeVal).toISOString() : new Date().toISOString(),
    assessmentType: currentAssessmentType
  };

  if (currentAssessmentType === 'CODING') {
    payload.instructions = document.getElementById('codingInstructions').value;
    payload.starterCode = document.getElementById('codingStarter').value;
    payload.expectedOutput = document.getElementById('codingExpected').value;
    payload.questions = [];
  } else {
    if (questionsList.length === 0) {
      toast.error('يجب إضافة سؤال واحد على الأقل');
      return;
    }
    var invalidQ = questionsList.find(function(q) { return !q.text || q.options.some(function(o) { return !o; }); });
    if (invalidQ) {
      toast.error('يجب ملء جميع الأسئلة والخيارات');
      return;
    }
    payload.questions = questionsList.map(function(q) {
      return { text: q.text, options: q.options, correctIndex: q.correctIndex };
    });
    payload.instructions = '';
    payload.starterCode = '';
    payload.expectedOutput = '';
  }

  api.createAssessment(payload).then(function() {
    toast.success('\u062a\u0645 \u0625\u0646\u0634\u0627\u0621 \u0627\u0644\u0627\u0645\u062a\u062d\u0627\u0646 \u0628\u0646\u062c\u0627\u062d');
    closeModal();
    document.getElementById('createForm').reset();
    selectType('CODING');
    questionsList = [];
    renderQuestions();
    loadAssessments();
  }).catch(function(err) { toast.error(err.message); });
});

function showResults(id, title) {
  document.getElementById('resultsTitle').textContent = '\u0646\u062a\u0627\u0626\u062c: ' + title;
  var content = document.getElementById('resultsContent');
  content.innerHTML = '<div class="loading"><div class="spinner"></div></div>';
  document.getElementById('resultsModal').classList.add('active');

  api.getAssessmentResults(id).then(function(res) {
    var arr = Array.isArray(res) ? res : (res.results || []);
    var passingScore = (res.assessment && res.assessment.passingScore) || 60;
    if (!arr || arr.length === 0) {
      content.innerHTML = '<div class="empty-state"><p>\u0644\u0645 \u064a\u0633\u062c\u0644 \u0623\u064a \u0637\u0627\u0644\u0628 \u0628\u0639\u062f</p></div>';
      return;
    }
    content.innerHTML = '<div class="table-wrapper"><table><thead><tr>' +
      '<th>\u0627\u0644\u0637\u0627\u0644\u0628</th><th>\u0627\u0644\u062f\u0631\u062c\u0629</th><th>\u0627\u0644\u062d\u0627\u0644\u0629</th>' +
      '</tr></thead><tbody>' +
      arr.map(function(r) {
        var passed = (r.score || 0) >= passingScore;
        return '<tr><td>' + escapeHtml(r.student && r.student.name || '\u0637\u0627\u0644\u0628') + '</td>' +
          '<td>' + (r.score || 0) + '/' + (r.totalPointsPossible || 100) + '</td>' +
          '<td><span class="badge ' + (passed ? 'badge-success' : 'badge-danger') + '">' + (passed ? '\u0646\u0627\u062c\u062d' : '\u0631\u0627\u0633\u0628') + '</span></td></tr>';
      }).join('') +
      '</tbody></table></div>';
  }).catch(function() {
    content.innerHTML = '<p style="color:var(--text-muted)">\u0644\u0627 \u064a\u0645\u0646 \u062a\u062d\u0645\u064a\u0644 \u0627\u0644\u0646\u062a\u0627\u0626\u062c</p>';
  });
}

function closeResultsModal() {
  document.getElementById('resultsModal').classList.remove('active');
}

loadAssessments();
