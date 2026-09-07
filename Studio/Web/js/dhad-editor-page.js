    /* ═══════════════════════════════════════════════════════════════════
       Dhad Code Editor - Main Logic
       ═══════════════════════════════════════════════════════════════════ */

    function escapeHtml(s) {
      if (!s) return '';
      return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;').replace(/'/g,'&#39;');
    }

    const editor = document.getElementById('codeEditor');
    const highlightLayer = document.getElementById('highlightLayer');
    const lineNumbers = document.getElementById('lineNumbers');
    const outputConsole = document.getElementById('outputConsole');
    const transpiledCode = document.getElementById('transpiledCode');
    const autocomplete = document.getElementById('autocomplete');
    const statusDot = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    const cursorPos = document.getElementById('cursorPos');
    const charCount = document.getElementById('charCount');
    const lineCount = document.getElementById('lineCount');
    const fileNameInput = document.getElementById('fileName');
    const inputLine = document.getElementById('inputLine');
    const inlineInput = document.getElementById('inlineInput');
    const inputSubmit = document.getElementById('inputSubmit');

    let isRunning = false;
    let currentFile = null;
    let executionTimeout = null;

    /* ═══════════════════════════════════════════════════════════════════
       Code Editor Features
       ═══════════════════════════════════════════════════════════════════ */

    // Syntax Highlighting
    function updateHighlight() {
      const code = editor.value;
      highlightLayer.innerHTML = DhadInterpreter.highlight(code) + '\n';
    }

    // Line Numbers
    function updateLineNumbers() {
      const lines = editor.value.split('\n').length;
      let html = '';
      for (let i = 1; i <= lines; i++) {
        html += '<span class="ln">' + i + '</span>';
      }
      lineNumbers.innerHTML = html;
      lineCount.textContent = lines + ' سطر';
      charCount.textContent = editor.value.length + ' حرف';
    }

    // Sync Scroll
    editor.addEventListener('scroll', function() {
      highlightLayer.scrollLeft = editor.scrollLeft;
      highlightLayer.scrollTop = editor.scrollTop;
      lineNumbers.scrollTop = editor.scrollTop;
    });

    // Update on Input
    editor.addEventListener('input', function() {
      updateHighlight();
      updateLineNumbers();
      saveToAutoSave();
      if (currentFilePath && openFiles.has(currentFilePath)) {
        const data = openFiles.get(currentFilePath);
        data.modified = editor.value !== data.originalContent;
        updateEditorTabs();
      }
    });

    // Cursor Position
    editor.addEventListener('click', updateCursorPos);
    editor.addEventListener('keyup', updateCursorPos);

    function updateCursorPos() {
      const pos = editor.selectionStart;
      const text = editor.value.substring(0, pos);
      const line = text.split('\n').length;
      const col = pos - text.lastIndexOf('\n');
      cursorPos.textContent = 'سطر ' + line + '، عمود ' + col;
    }

    // Tab Support
    editor.addEventListener('keydown', function(e) {
      if (e.key === 'Tab') {
        e.preventDefault();
        const start = this.selectionStart;
        const end = this.selectionEnd;
        this.value = this.value.substring(0, start) + '  ' + this.value.substring(end);
        this.selectionStart = this.selectionEnd = start + 2;
        updateHighlight();
      }

      // Ctrl+Enter = Run
      if (e.ctrlKey && e.key === 'Enter') {
        e.preventDefault();
        runCode();
      }

      // Ctrl+S = Save
      if (e.ctrlKey && e.key === 's') {
        e.preventDefault();
        saveFile();
      }

      // Ctrl+Shift+F = Format
      if (e.ctrlKey && e.shiftKey && e.key === 'F') {
        e.preventDefault();
        formatCode();
      }

      // Auto-close brackets
      if (e.key === '(' || e.key === '{' || e.key === '[') {
        const pairs = { '(': ')', '{': '}', '[': ']' };
        const start = this.selectionStart;
        const end = this.selectionEnd;
        if (start !== end) {
          e.preventDefault();
          const selected = this.value.substring(start, end);
          this.value = this.value.substring(0, start) + e.key + selected + pairs[e.key] + this.value.substring(end);
          this.selectionStart = start + 1;
          this.selectionEnd = end + 1;
          updateHighlight();
        }
      }

      // Autocomplete trigger + navigation
      if (autocomplete.classList.contains('active')) {
        // Ctrl+1 through Ctrl+9 to insert suggestion
        if (e.ctrlKey && e.key >= '1' && e.key <= '9') {
          const idx = parseInt(e.key) - 1;
          if (idx < currentFilteredKeywords.length) {
            e.preventDefault();
            insertAutocomplete(currentFilteredKeywords[idx].text);
            return;
          }
        }
        if (e.key === 'ArrowDown') {
          e.preventDefault();
          navigateAutocomplete(1);
          return;
        }
        if (e.key === 'ArrowUp') {
          e.preventDefault();
          navigateAutocomplete(-1);
          return;
        }
        if (e.key === 'Enter' || e.key === 'Tab') {
          const sel = autocomplete.querySelector('.autocomplete-item.selected');
          if (sel) {
            e.preventDefault();
            insertAutocomplete(sel.dataset.text);
            return;
          }
        }
      }
      // Ctrl+1-9 even when autocomplete is not visible - show and pick
      if (e.ctrlKey && e.key >= '1' && e.key <= '9' && !autocomplete.classList.contains('active')) {
        const idx = parseInt(e.key) - 1;
        if (currentFilteredKeywords.length > idx) {
          e.preventDefault();
          insertAutocomplete(currentFilteredKeywords[idx].text);
          return;
        }
      }
      if (e.key === 'Escape') {
        hideAutocomplete();
        return;
      }
      if (e.key.length === 1 && /[a-zA-Z\u0600-\u06FF]/.test(e.key)) {
        setTimeout(showAutocomplete, 100);
      }
    });

    /* ═══════════════════════════════════════════════════════════════════
       Autocomplete
       ═══════════════════════════════════════════════════════════════════ */

    const keywords = [
      // ── Types ────────────────────────────────────────
      { text: 'صحيح', type: 'type', desc: 'عدد صحيح' },
      { text: 'عشري', type: 'type', desc: 'عدد عشري (float)' },
      { text: 'نص', type: 'type', desc: 'نص (string)' },
      { text: 'منطقي', type: 'type', desc: 'قيمة منطقية (bool)' },
      { text: 'حرف', type: 'type', desc: 'حرف واحد (char)' },
      { text: 'فراغ', type: 'type', desc: 'نوع فارغ (void)' },
      { text: 'أي', type: 'type', desc: 'أي نوع (any)' },
      { text: 'موعود', type: 'type', desc: 'وعد (Promise)' },
      // ── Literals ─────────────────────────────────────
      { text: 'صواب', type: 'value', desc: 'قيمة صحيحة (true)' },
      { text: 'خطأ', type: 'value', desc: 'قيمة خاطئة (false)' },
      { text: 'عدم', type: 'value', desc: 'قيمة فارغة (null)' },
      { text: 'ثابت', type: 'value', desc: 'ثابت (const)' },
      // ── Control Flow ─────────────────────────────────
      { text: 'إذا', type: 'keyword', desc: 'statement شرطي (if)' },
      { text: 'وإلا', type: 'keyword', desc: 'بديل شرطي (else)' },
      { text: 'وإلا إذا', type: 'keyword', desc: 'شرط إضافي (else if)' },
      { text: 'طالما', type: 'keyword', desc: 'تكرار (while)' },
      { text: 'لكل', type: 'keyword', desc: 'تكرار محدد (for)' },
      { text: 'في', type: 'keyword', desc: 'في (for-in)' },
      { text: 'افعل', type: 'keyword', desc: 'تنفيذ (do)' },
      { text: 'اختر', type: 'keyword', desc: 'اختيار (switch)' },
      { text: 'حالة', type: 'keyword', desc: 'حالة (case)' },
      { text: 'افتراضي', type: 'keyword', desc: 'القيمة الافتراضية (default)' },
      { text: 'توقف', type: 'keyword', desc: 'إيقاف (break)' },
      { text: 'اكسر', type: 'keyword', desc: 'إيقاف (break) — بديل' },
      { text: 'استمر', type: 'keyword', desc: 'متابعة (continue)' },
      { text: 'تابع', type: 'keyword', desc: 'متابعة (continue) — بديل' },
      // ── Functions ────────────────────────────────────
      { text: 'دالة', type: 'keyword', desc: 'تعريف دالة (function)' },
      { text: 'ارجع', type: 'keyword', desc: 'إعادة قيمة (return)' },
      { text: 'بانتظار', type: 'keyword', desc: 'بانتظار (async/await)' },
      { text: 'انتظر', type: 'keyword', desc: 'بانتظار (await) — بديل' },
      // ── OOP ──────────────────────────────────────────
      { text: 'صنف', type: 'keyword', desc: 'تعريف صنف (class)' },
      { text: 'جديد', type: 'keyword', desc: 'كائن جديد (new)' },
      { text: 'هذا', type: 'keyword', desc: 'الكائن الحالي (this)' },
      { text: 'يرث', type: 'keyword', desc: 'وراثة (extends)' },
      { text: 'خاص', type: 'keyword', desc: 'خاص (private)' },
      { text: 'عام', type: 'keyword', desc: 'عام (public)' },
      { text: 'محمي', type: 'keyword', desc: 'محمي (protected)' },
      { text: 'مجرّد', type: 'keyword', desc: 'مجرّد (abstract)' },
      { text: 'واجهة', type: 'keyword', desc: 'واجهة (interface)' },
      { text: 'هيكل', type: 'keyword', desc: 'هيكل (struct)' },
      { text: 'نطاق', type: 'keyword', desc: 'نطاق (namespace)' },
      { text: 'تعداد', type: 'keyword', desc: 'تعداد (enum)' },
      { text: 'ساكن', type: 'keyword', desc: 'عضو ثابت (static)' },
      { text: 'مؤشر', type: 'keyword', desc: 'مؤشر (pointer)' },
      { text: 'مرجع', type: 'keyword', desc: 'مرجع (reference)' },
      { text: 'قالب', type: 'keyword', desc: 'قالب (template)' },
      { text: 'خارجی', type: 'keyword', desc: 'خارجی (extern)' },
      { text: 'مضمن', type: 'keyword', desc: 'مضمن (inline)' },
      { text: 'حجم_الـ', type: 'keyword', desc: 'حجم النوع (sizeof)' },
      { text: 'نوع_الـ', type: 'keyword', desc: 'نوع القيمة (typeof)' },
      { text: 'تأكد', type: 'keyword', desc: 'تأكد من الشرط (assert)' },
      { text: 'ذاتي', type: 'keyword', desc: 'الكائن الحالي (this) — بديل' },
      { text: 'الأصل', type: 'keyword', desc: 'الأصل (base)' },
      { text: 'مجرد', type: 'keyword', desc: 'مجرد (abstract)' },
      { text: 'قائمة', type: 'keyword', desc: 'قائمة مختلطة (mixed list)' },
      { text: 'تزامن', type: 'keyword', desc: 'تزامن (sync)' },
      { text: 'خيط', type: 'keyword', desc: 'خيط (thread)' },
      { text: 'احجز', type: 'keyword', desc: 'قفل (lock)' },
      { text: 'مشترك', type: 'keyword', desc: 'متغير مشترك (shared)' },
      { text: 'فريد', type: 'keyword', desc: 'فريد (unique)' },
      { text: 'بديل', type: 'keyword', desc: 'بديل (alternative)' },
      { text: 'استثناء', type: 'keyword', desc: 'استثناء (exception)' },
      { text: 'خطأ', type: 'value', desc: 'قيمة خطأ (error object)' },
      // ── Error Handling ───────────────────────────────
      { text: 'حاول', type: 'keyword', desc: 'محاولة (try)' },
      { text: 'امسك', type: 'keyword', desc: 'التقاط خطأ (catch)' },
      { text: 'أخيراً', type: 'keyword', desc: 'أخيراً (finally)' },
      { text: 'ارمِ', type: 'keyword', desc: 'رمي خطأ (throw)' },
      // ── Import/Export ────────────────────────────────
      { text: 'استورد', type: 'keyword', desc: 'استيراد مكتبة (import)' },
      { text: 'صدّر', type: 'keyword', desc: 'تصدير (export)' },
      // ── Built-in Functions ───────────────────────────
      { text: 'طباعة', type: 'function', desc: 'طباعة نص (print)' },
      { text: 'ادخال', type: 'function', desc: 'إدخال من المستخدم (input)' },
      { text: 'طول', type: 'function', desc: 'طول النص/المصفوفة (length)' },
      // ── النصوص (Text Library) ───────────────────────
      { text: 'استخرج', type: 'function', desc: 'استخراج نص (substring)' },
      { text: 'ابحث', type: 'function', desc: 'بحث في نص (indexOf)' },
      { text: 'استبدل', type: 'function', desc: 'استبدال في نص (replace)' },
      { text: 'لأحرف_صغيرة', type: 'function', desc: 'تحويل لأحرف صغيرة (toLowerCase)' },
      { text: 'لأحرف_كبيرة', type: 'function', desc: 'تحويل لأحرف كبيرة (toUpperCase)' },
      { text: 'اتجه', type: 'function', desc: 'إزالة المسافات (trim)' },
      { text: 'يبدأ', type: 'function', desc: 'يبدأ بـ (startsWith)' },
      { text: 'ينتهي', type: 'function', desc: 'ينتهي بـ (endsWith)' },
      { text: 'تقسيم', type: 'function', desc: 'تقسيم نص (split)' },
      { text: 'يحتوي', type: 'function', desc: 'يحتوي على (includes)' },
      { text: 'كرر', type: 'function', desc: 'تكرار نص (repeat)' },
      { text: 'احترس', type: 'function', desc: 'حرف في موضع (charAt)' },
      { text: 'قص_من', type: 'function', desc: 'قص من موضع (slice)' },
      { text: 'طول', type: 'function', desc: 'طول النص/المصفوفة (length)' },
      // ── المصفوفات (Array Methods) ───────────────────
      { text: 'أضف', type: 'function', desc: 'إضافة عنصر (push)' },
      { text: 'احذف', type: 'function', desc: 'حذف عنصر (splice)' },
      { text: 'مزج', type: 'function', desc: 'دمج مصفوفتين (concat)' },
      { text: 'ابحث_في', type: 'function', desc: 'بحث في مصفوفة (indexOf)' },
      // ── الرياضيات (Math Library) ────────────────────
      { text: 'جمع', type: 'function', desc: 'جمع عددين (add)' },
      { text: 'طرح', type: 'function', desc: 'طرح عددين (subtract)' },
      { text: 'ضرب', type: 'function', desc: 'ضرب عددين (multiply)' },
      { text: 'قسمة', type: 'function', desc: 'قسمة عددين (divide)' },
      { text: 'باقي', type: 'function', desc: 'باقي القسمة (modulus)' },
      { text: 'مطلق', type: 'function', desc: 'القيمة المطلقة (abs)' },
      { text: 'قوة', type: 'function', desc: 'أس (power)' },
      { text: 'جذر', type: 'function', desc: 'جذر تربيعي (sqrt)' },
      { text: 'أقصى', type: 'function', desc: 'أكبر قيمة (max)' },
      { text: 'أدناه', type: 'function', desc: 'أصغر قيمة (min)' },
      { text: 'عشوائي', type: 'function', desc: 'رقم عشوائي (random)' },
      { text: 'عشوائي_بين', type: 'function', desc: 'رقم عشوائي بين (random between)' },
      { text: 'مضروب', type: 'function', desc: 'مضروب (factorial)' },
      { text: 'log', type: 'function', desc: 'لوغاريتم (log)' },
      { text: 'أس_الطبيعي', type: 'function', desc: 'أس طبيعي (exp)' },
      { text: 'sin', type: 'function', desc: 'جا (sine)' },
      { text: 'cos', type: 'function', desc: 'جتا (cosine)' },
      { text: 'tan', type: 'function', desc: 'ظا (tangent)' },
      { text: 'π', type: 'function', desc: 'العدد باي (pi)' },
      // ── المصفوفات (Array Library) ───────────────────
      { text: 'إنشاء_مصفوفة', type: 'function', desc: 'إنشاء مصفوفة فارغة' },
      { text: 'أضف', type: 'function', desc: 'إضافة عنصر (push)' },
      { text: 'احذف_من', type: 'function', desc: 'حذف من موضع (splice)' },
      { text: 'ابحث_في', type: 'function', desc: 'بحث في مصفوفة (indexOf)' },
      { text: 'فرز', type: 'function', desc: 'ترتيب مصفوفة (sort)' },
      { text: 'عكس_مصفوفة', type: 'function', desc: 'عكس ترتيب (reverse array)' },
      { text: 'مسطح', type: 'function', desc: 'تسوية مصفوفة متعددة (flat)' },
      { text: 'تصفية', type: 'function', desc: 'تصفية عناصر (filter)' },
      { text: 'تحويل', type: 'function', desc: 'تحويل عناصر (map)' },
      { text: 'مجموع', type: 'function', desc: 'مجموع العناصر (reduce)' },
      { text: 'يوجد', type: 'function', desc: 'يوجد عنصر (some)' },
      { text: 'يوجد_الكل', type: 'function', desc: 'جميع العناصر (every)' },
      { text: 'طول_مصفوفة', type: 'function', desc: 'طول المصفوفة (length)' },
      // ── المكدس والطابور (Stack & Queue) ─────────────
      { text: 'إنشاء_مكدس', type: 'function', desc: 'إنشاء مكدس (create stack)' },
      { text: 'ادفع', type: 'function', desc: 'إضافة للمكدس (push stack)' },
      { text: 'أخرج', type: 'function', desc: 'إخراج من المكدس (pop)' },
      { text: 'أعلى', type: 'function', desc: 'أعلى المكدس (top)' },
      { text: 'حجم_مكدس', type: 'function', desc: 'حجم المكدس (stack size)' },
      { text: 'إنشاء_طابور', type: 'function', desc: 'إنشاء طابور (create queue)' },
      { text: 'ادخل', type: 'function', desc: 'إدخال للطابور (enqueue)' },
      { text: 'اسحب', type: 'function', desc: 'إخراج من الطابور (dequeue)' },
      { text: 'مقدمة', type: 'function', desc: 'بداية الطابور (front)' },
      { text: 'حجم_طابور', type: 'function', desc: 'حجم الطابور (queue size)' },
      // ── الرسوميات (Graphics Library) ────────────────
      { text: 'إنشاء_لوحة', type: 'function', desc: 'إنشاء لوحة رسم (canvas)' },
      { text: 'ملء', type: 'function', desc: 'ملء اللوحة (fill background)' },
      { text: 'مستطيل', type: 'function', desc: 'رسم مستطيل (rectangle)' },
      { text: 'دائرة', type: 'function', desc: 'رسم دائرة (circle)' },
      { text: 'خط', type: 'function', desc: 'رسم خط (line)' },
      { text: 'نص_على_لوحة', type: 'function', desc: 'كتابة نص على اللوحة' },
      { text: 'مسح', type: 'function', desc: 'مسح اللوحة (clear)' },
      { text: 'لون', type: 'function', desc: 'إنشاء لون (color)' },
      { text: 'لون_عشري', type: 'function', desc: 'لون من hex (hex color)' },
      { text: 'خلط', type: 'function', desc: 'خلط لونين (mix colors)' },
      { text: 'تدرج', type: 'function', desc: 'تدرج لوني (gradient)' },
      // ── الشبكات (Network Library) ────────────────────
      { text: 'GET', type: 'function', desc: 'طلب GET (fetch)' },
      { text: 'POST', type: 'function', desc: 'طلب POST' },
      { text: 'PUT', type: 'function', desc: 'طلب PUT' },
      { text: 'DELETE', type: 'function', desc: 'طلب DELETE' },
      { text: 'تحميل_ملف', type: 'function', desc: 'تحميل ملف نصي' },
      { text: 'تحميل_JSON', type: 'function', desc: 'تحميل بيانات JSON' },
      // ── التخزين (Storage Library) ────────────────────
      { text: 'احفظ', type: 'function', desc: 'حفظ في التخزين المحلي (localStorage)' },
      { text: 'اقرأ', type: 'function', desc: 'قراءة من التخزين المحلي' },
      { text: 'احذف_تخزين', type: 'function', desc: 'حذف من التخزين المحلي' },
      { text: 'امسح_الكل', type: 'function', desc: 'مسح كل التخزين' },
      { text: 'قائمة_المفاتيح', type: 'function', desc: 'جميع المفاتيح (keys)' },
      // ── الوقت (Time Library) ─────────────────────────
      { text: 'الوقت_الحالي', type: 'function', desc: 'الوقت الحالي (Date.now)' },
      { text: 'الطابع_الزمني', type: 'function', desc: 'الطابع الزمني (timestamp)' },
      { text: 'السنة', type: 'function', desc: 'السنة الحالية (year)' },
      { text: 'الشهر', type: 'function', desc: 'الشهر الحالي (month)' },
      { text: 'اليوم', type: 'function', desc: 'اليوم (day)' },
      { text: 'الساعة', type: 'function', desc: 'الساعة (hour)' },
      { text: 'الدقيقة', type: 'function', desc: 'الدقيقة (minute)' },
      { text: 'الثانية', type: 'function', desc: 'الثانية (second)' },
      { text: 'يوم_الاسبوع', type: 'function', desc: 'اسم يوم الأسبوع' },
      { text: 'اسم_الشهر', type: 'function', desc: 'اسم الشهر' },
      { text: 'فرق', type: 'function', desc: 'فرق بين تاريخين (diff)' },
      { text: 'تأخير', type: 'function', desc: 'تأخير بالميللي ثانية (delay)' },
      { text: 'جدولة', type: 'function', desc: 'جدولة تنفيذ (setTimeout)' },
      { text: 'تكرار', type: 'function', desc: 'تكرار مؤقت (setInterval)' },
      // ── الأحداث (Events Library) ─────────────────────
      { text: 'استمع', type: 'function', desc: 'استماع لحدث (addEventListener)' },
      { text: 'أوقف', type: 'function', desc: 'إيقاف الاستماع (removeEventListener)' },
      { text: 'أرسل_حدث', type: 'function', desc: 'إرسال حدث (dispatchEvent)' },
      { text: 'نقر', type: 'function', desc: 'استماع للنقر (click)' },
      { text: 'ضغط_لوحة', type: 'function', desc: 'استماع للوحة المفاتيح (keydown)' },
      // ── الواجهة (DOM Library) ────────────────────────
      { text: 'احصل_على', type: 'function', desc: 'الحصول على عنصر (getElementById)' },
      { text: 'احصل_على_كل', type: 'function', desc: 'الحصول على الكل (querySelectorAll)' },
      { text: 'انشئ', type: 'function', desc: 'إنشاء عنصر (createElement)' },
      { text: 'ادرج', type: 'function', desc: 'إضافة عنصر فرعي (appendChild)' },
      { text: 'نص_العنصر', type: 'function', desc: 'نص العنصر (textContent)' },
      { text: 'html_العنصر', type: 'function', desc: 'innerHTML للعنصر' },
      { text: 'صنف_العنصر', type: 'function', desc: 'الأصناف (classList)' },
      { text: 'أضف_صنف', type: 'function', desc: 'إضافة صنف (addClass)' },
      { text: 'احذف_صنف', type: 'function', desc: 'حذف صنف (removeClass)' },
      { text: 'بدل_صنف', type: 'function', desc: 'تبديل صنف (toggleClass)' },
      { text: 'نمط', type: 'function', desc: 'تعيين أنماط (style)' },
      { text: 'مواقع', type: 'function', desc: 'موقع العنصر (getBoundingClientRect)' },
      // ── الأمان (Security Library) ────────────────────
      { text: 'تشفير_base64', type: 'function', desc: 'تشفير base64 (encode)' },
      { text: 'فك_base64', type: 'function', desc: 'فك تشفير base64 (decode)' },
      { text: 'تشفير_sha256', type: 'function', desc: 'تشفير SHA-256' },
      { text: 'إنشاء_رمـز', type: 'function', desc: 'معرف فريد (UUID)' },
      { text: 'عشوائي_أمان', type: 'function', desc: 'نص عشوائي آمن (secure random)' },
      { text: 'تحقق_بريد', type: 'function', desc: 'تحقق من البريد الإلكتروني' },
      { text: 'تحقق_رابط', type: 'function', desc: 'تحقق من صحة الرابط' },
      { text: 'تحقق_رقم', type: 'function', desc: 'تحقق من الرقم' },
      // ── الأمان المتقدم (Advanced Security) ────────────
      { text: 'الهاش', type: 'function', desc: 'تشفير SHA-256 هاش' },
      { text: 'الهاش_MD5', type: 'function', desc: 'تشفير MD5 هاش' },
      { text: 'hmac', type: 'function', desc: 'HMAC توقيع' },
      { text: 'تشفير_قيصر', type: 'function', desc: 'تشفير قيصر (Caesar cipher)' },
      { text: 'فك_قيصر', type: 'function', desc: 'فك تشفير قيصر' },
      { text: 'تشفير_xor', type: 'function', desc: 'تشفير XOR' },
      { text: 'تشفير_AES', type: 'function', desc: 'تشفير AES-256' },
      { text: 'فك_AES', type: 'function', desc: 'فك تشفير AES' },
      { text: 'ولّد_مفتاح', type: 'function', desc: 'توليد مفتاح آمن' },
      { text: 'ولّد_ملح', type: 'function', desc: 'توليد ملح عشوائي' },
      { text: 'تجزئة', type: 'function', desc: 'تجزئة كلمة مرور مع ملح' },
      { text: 'تحقق_كلمة', type: 'function', desc: 'تحقق من كلمة المرور' },
      // ── الويب (Web Library) ───────────────────────────
      { text: 'صفحة', type: 'function', desc: 'إنشاء صفحة HTML' },
      { text: 'أغلق_صفحة', type: 'function', desc: 'إغلاق صفحة HTML' },
      { text: 'عنوان1', type: 'function', desc: 'عنوان رئيسي h1' },
      { text: 'عنوان2', type: 'function', desc: 'عنوان فرعي h2' },
      { text: 'عنوان3', type: 'function', desc: 'عنوان فرعي h3' },
      { text: 'فقرة', type: 'function', desc: 'فقرة نصية p' },
      { text: 'نص_عريض', type: 'function', desc: 'نص عريض strong' },
      { text: 'نص_مائل', type: 'function', desc: 'نص مائل em' },
      { text: 'كود', type: 'function', desc: 'نص كود code' },
      { text: 'رابط', type: 'function', desc: 'رابط تنقل a' },
      { text: 'صورة', type: 'function', desc: 'صورة img' },
      { text: 'قائمة', type: 'function', desc: 'قائمة غير مرتبة ul' },
      { text: 'قائمة_مرتبة', type: 'function', desc: 'قائمة مرتبة ol' },
      { text: 'جدول', type: 'function', desc: 'جدول بيانات' },
      { text: 'جدول_عناوين', type: 'function', desc: 'جدول مع عناوين' },
      { text: 'نموذج', type: 'function', desc: 'نموذج إدخال بيانات' },
      { text: 'حاوية', type: 'function', desc: 'حاوية div' },
      { text: 'بطاقة', type: 'function', desc: 'بطاقة card' },
      { text: 'تنسيق', type: 'function', desc: 'إضافة CSS style' },
      { text: 'كلاس', type: 'function', desc: 'تعريف class CSS' },
      { text: 'اعرض', type: 'function', desc: 'عرض HTML في الصفحة' },
      // ── الأدوات الأمنية (Security Tools) ───────────────
      { text: 'جمع_معلومات', type: 'function', desc: 'جمع معلومات OSINT' },
      { text: 'فحص_منافذ', type: 'function', desc: 'فحص منافذ الشبكة' },
      { text: 'كشف_XSS', type: 'function', desc: 'كشف ثغرات XSS' },
      { text: 'تنقية', type: 'function', desc: 'تنقية مدخلات من XSS' },
      { text: 'كشف_SQLi', type: 'function', desc: 'كشف SQL Injection' },
      { text: 'عدّل_SQL', type: 'function', desc: 'تنقية استعلامات SQL' },
      { text: 'كشف_Path', type: 'function', desc: 'كشف Path Traversal' },
      { text: 'حلل_رؤوس', type: 'function', desc: 'تحليل رؤوس HTTP' },
      { text: 'تقرير_أمان', type: 'function', desc: 'تقرير فحص أمان شامل' },
      // ── RSA والتشفير غير المتماثل ──────────────────
      { text: 'ولّد_مفاتيح_RSA', type: 'function', desc: 'توليد مفتاح RSA' },
      { text: 'تشفير_RSA', type: 'function', desc: 'تشفير RSA' },
      { text: 'فك_RSA', type: 'function', desc: 'فك تشفير RSA' },
      // ── TLS/SSL ─────────────────────────────────────
      { text: 'فحص_TLS', type: 'function', desc: 'فحص اتصال TLS/SSL' },
      { text: 'فحص_الشهادة', type: 'function', desc: 'فحص الشهادة الرقمية' },
      // ── Command Injection ────────────────────────────
      { text: 'كشف_Command', type: 'function', desc: 'كشف Command Injection' },
      // ── CSRF ────────────────────────────────────────
      { text: 'كشف_CSRF', type: 'function', desc: 'كشف محاولة CSRF' },
      { text: 'ولّد_رمز_CSRF', type: 'function', desc: 'توليد رمز CSRF' },
      // ── اختبار كلمات المرور ─────────────────────────
      { text: 'فحص_كلمة_مرور', type: 'function', desc: 'فحص قوة كلمة المرور' },
      // ── جدران الحماية ───────────────────────────────
      { text: 'قواعد_جدار_النار', type: 'function', desc: 'عرض قواعد جدار الحماية' },
      { text: 'أضف_قاعدة', type: 'function', desc: 'إضافة قاعدة جدار حماية' },
      { text: 'احذف_قاعدة', type: 'function', desc: 'حذف قاعدة جدار حماية' },
      // ── IDS/IPS ─────────────────────────────────────
      { text: 'كشف_الاختراق', type: 'function', desc: 'كشف محاولات الاختراق' },
      // ── VPN ─────────────────────────────────────────
      { text: 'محاكاة_VPN', type: 'function', desc: 'محاكاة اتصال VPN' },
      // ── WiFi ────────────────────────────────────────
      { text: 'فحص_WiFi', type: 'function', desc: 'فحص أمان الشبكة اللاسلكية' },
      // ── DNS/DHCP ────────────────────────────────────
      { text: 'فحص_DNS', type: 'function', desc: 'فحص سجلات DNS' },
      { text: 'كشف_DNS_مشبوه', type: 'function', desc: 'كشف نطاقات DNS مشبوهة' },
      // ── بروتوكولات الشبكة ──────────────────────────
      { text: 'حلل_بروتوكول', type: 'function', desc: 'تحليل البروتوكول حسب المنفذ' },
      // ── مراقبة الشبكة ───────────────────────────────
      { text: 'مراقبة_الشبكة', type: 'function', desc: 'مراقبة حركة الشبكة' },
      { text: 'التقاط_حزم', type: 'function', desc: 'التقاط حزم الشبكة' },
      // ── استخراج البيانات ─────────────────────────────
      { text: 'استخراج_بيانات', type: 'function', desc: 'استخراج بيانات من ملف' },
      { text: 'استرجاع_محذوفات', type: 'function', desc: 'استرجاع ملفات محذوفة' },
      // ── تحليل الذاكرة ───────────────────────────────
      { text: 'تحليل_الذاكرة', type: 'function', desc: 'تحليل ملف الذاكرة' },
      { text: 'كشف_عمليات_مشبوهة', type: 'function', desc: 'كشف عمليات مشبوهة' },
      // ── تحليل السجلات ───────────────────────────────
      { text: 'تحليل_سجلات', type: 'function', desc: 'تحليل ملفات السجلات' },
      { text: 'بحث_في_سجلات', type: 'function', desc: 'بحث في السجلات' },
      // ── التحليل الجنائي للبريد ───────────────────────
      { text: 'حلل_بريد', type: 'function', desc: 'تحليل رسالة إلكترونية' },
      { text: 'فحص_مرفقات', type: 'function', desc: 'فحص مرفقات مشبوهة' },
      // ── تحليل البرمجيات الخبيثة ─────────────────────
      { text: 'حلل_برمجيات', type: 'function', desc: 'تحليل برمجيات خبيثة' },
      { text: 'فحص_بصمة', type: 'function', desc: 'فحص بصمة الملف' },
      // ── التحليل الجنائي للويب ────────────────────────
      { text: 'حلل_سجلات_الويب', type: 'function', desc: 'تحليل سجلات خادم الويب' },
      { text: 'كشف_web_shell', type: 'function', desc: 'كشف Web Shell خبيث' },
      // ── كتابة التقارير ───────────────────────────────
      { text: 'تقرير_اختبار', type: 'function', desc: 'تقرير اختبار اختراق' },
      // ── الأخلاقيات ──────────────────────────────────
      { text: 'تحقق_أخلاقي', type: 'function', desc: 'التحقق من الإطار الأخلاقي' },
      // ── HTML متقدم ──────────────────────────────────
      { text: 'صفحة_كاملة', type: 'function', desc: 'إنشاء صفحة HTML كاملة' },
      { text: 'ترويسة', type: 'function', desc: 'ترويسة الصفحة' },
      { text: 'تذييل', type: 'function', desc: 'تذييل الصفحة' },
      { text: 'شبكة', type: 'function', desc: 'شبكة عناصر (Grid)' },
      { text: 'شريط_تنقل', type: 'function', desc: 'شريط تنقل nav' },
      { text: 'زر', type: 'function', desc: 'زر تفاعلي button' },
      { text: 'حقل_إدخال', type: 'function', desc: 'حقل إدخال input' },
      // ── CSS متقدم (Flexbox + Grid) ──────────────────
      { text: 'تخطيط_مرن', type: 'function', desc: 'تخطيط Flexbox مرن' },
      { text: 'تخطيط_شبكة', type: 'function', desc: 'تخطيط CSS Grid' },
      { text: 'تخطيط_جانبي', type: 'function', desc: 'تخطيط جانبي ثابت' },
      { text: 'بطاقة_متحركة', type: 'function', desc: 'بطاقة بتأثير حركي' },
      { text: 'عمود_مرن', type: 'function', desc: 'عمود مرن بالكامل' },
      { text: 'مركز', type: 'function', desc: 'توسيط المحتوى' },
      { text: 'فاصل', type: 'function', desc: 'فاصل أفقي hr' },
      // ── الملفات (Files Library) ──────────────────────
      { text: 'اقرأ_ملف', type: 'function', desc: 'قراءة ملف (read file)' },
      { text: 'اكتب_لملف', type: 'function', desc: 'كتابة لملف (write file)' },
      { text: 'نسخ_ملف', type: 'function', desc: 'نسخ ملف (copy file)' },
      { text: 'احذف_ملف', type: 'function', desc: 'حذف ملف (delete file)' },
      // ── الاختبارات (Testing Library) ─────────────────
      { text: 'وصف', type: 'function', desc: 'وصف مجموعة اختبارات (describe)' },
      { text: 'it', type: 'function', desc: 'اختبار فرعي (it)' },
      { text: 'توقع', type: 'function', desc: 'توقع نتيجة (expect)' },
      { text: 'تقرير', type: 'function', desc: 'تقرير الاختبارات (report)' },
    ];

    let currentFilteredKeywords = [];

    function showAutocomplete() {
      const pos = editor.selectionStart;
      const text = editor.value.substring(0, pos);
      const match = text.match(/[\u0600-\u06FF\u0750-\u077F\w]+$/);
      if (!match || match[0].length < 1) {
        hideAutocomplete();
        return;
      }

      const query = match[0].toLowerCase();
      const filtered = keywords.filter(k =>
        k.text.toLowerCase().includes(query)
      ).sort((a, b) => {
        const aStarts = a.text.toLowerCase().startsWith(query) ? 0 : 1;
        const bStarts = b.text.toLowerCase().startsWith(query) ? 0 : 1;
        return aStarts - bStarts;
      });

      if (filtered.length === 0) {
        hideAutocomplete();
        return;
      }

      currentFilteredKeywords = filtered.slice(0, 10);

      let html = '';
      currentFilteredKeywords.forEach((item, i) => {
        const typeClass = item.type === 'keyword' ? 'ac-keyword' :
                          item.type === 'function' ? 'ac-function' :
                          item.type === 'type' ? 'ac-type' :
                          item.type === 'value' ? 'ac-value' : 'ac-keyword';
        const typeLabel = item.type === 'keyword' ? 'keyword' :
                          item.type === 'function' ? 'function' :
                          item.type === 'type' ? 'type' :
                          item.type === 'value' ? 'value' : item.type;
        const shortcut = i < 9 ? (i + 1) : '';
        html += '<div class="autocomplete-item' + (i === 0 ? ' selected' : '') + '" ' +
                'data-index="' + i + '" data-text="' + escapeHtml(item.text) + '" onclick="insertAutocomplete(\'' + item.text.replace(/'/g, "\\'") + '\')">' +
                '<span class="ac-shortcut">' + shortcut + '</span>' +
                '<span class="ac-type ' + typeClass + '">' + typeLabel + '</span>' +
                '<span class="ac-name">' + escapeHtml(item.text) + '</span>' +
                '<span class="ac-desc">' + escapeHtml(item.desc) + '</span>' +
                '</div>';
      });

      autocomplete.innerHTML = html;
      autocomplete.classList.add('active');

      // Position near cursor using viewport coords
      const coords = getEditorCoords(pos);
      const editorRect = editor.getBoundingClientRect();
      const acHeight = 260;
      const acWidth = 340;

      // Calculate cursor position in viewport
      const cursorViewportTop = editorRect.top + coords.top + 20;
      const cursorViewportLeft = editorRect.right - coords.left;

      // Vertical: show above if not enough space below
      let top = cursorViewportTop;
      const spaceBelow = window.innerHeight - cursorViewportTop;
      if (spaceBelow < acHeight && cursorViewportTop > acHeight) {
        top = cursorViewportTop - acHeight - 24;
      }
      top = Math.max(4, Math.min(top, window.innerHeight - acHeight - 4));

      // Horizontal: keep within viewport
      let left = cursorViewportLeft - acWidth / 2;
      left = Math.max(4, Math.min(left, window.innerWidth - acWidth - 4));

      autocomplete.style.top = top + 'px';
      autocomplete.style.left = left + 'px';
    }

    function hideAutocomplete() {
      autocomplete.classList.remove('active');
      autocompleteCurrentIndex = 0;
    }

    let autocompleteCurrentIndex = 0;

    function navigateAutocomplete(dir) {
      const items = autocomplete.querySelectorAll('.autocomplete-item');
      if (!items.length) return;
      items[autocompleteCurrentIndex].classList.remove('selected');
      autocompleteCurrentIndex = (autocompleteCurrentIndex + dir + items.length) % items.length;
      items[autocompleteCurrentIndex].classList.add('selected');
      items[autocompleteCurrentIndex].scrollIntoView({ block: 'nearest' });
    }

    function insertAutocomplete(text) {
      const pos = editor.selectionStart;
      const before = editor.value.substring(0, pos);
      const after = editor.value.substring(editor.selectionEnd);
      const wordStart = before.search(/[\u0600-\u06FF\u0750-\u077F\w]+$/);
      editor.value = before.substring(0, wordStart) + text + ' ' + after;
      editor.selectionStart = editor.selectionEnd = wordStart + text.length + 1;
      editor.focus();
      hideAutocomplete();
      updateHighlight();
      updateLineNumbers();
    }

    function getEditorCoords(pos) {
      // Use a hidden mirror element for accurate cursor position measurement
      const text = editor.value.substring(0, pos);
      const lines = text.split('\n');
      const line = lines.length - 1;
      const col = lines[line].length;

      // Create mirror element to measure exact text width
      const mirror = document.createElement('div');
      mirror.style.cssText = 'position:absolute;visibility:hidden;white-space:pre;font-family:' +
        getComputedStyle(editor).fontFamily + ';font-size:' + getComputedStyle(editor).fontSize +
        ';line-height:' + getComputedStyle(editor).lineHeight + ';padding:' +
        getComputedStyle(editor).paddingTop + ' ' + getComputedStyle(editor).paddingRight +
        ' ' + getComputedStyle(editor).paddingBottom + ' ' + getComputedStyle(editor).paddingLeft +
        ';direction:rtl;text-align:right;tab-size:2;';
      mirror.textContent = lines[line];
      document.body.appendChild(mirror);
      const textWidth = mirror.offsetWidth;
      document.body.removeChild(mirror);

      const lineHeight = parseFloat(getComputedStyle(editor).lineHeight) || 21.45;

      return {
        top: line * lineHeight - editor.scrollTop,
        left: editor.offsetWidth - textWidth - 16 + editor.scrollLeft // 16 = padding
      };
    }

    /* ═══════════════════════════════════════════════════════════════════
       Code Execution
       ═══════════════════════════════════════════════════════════════════ */

    let inputQueue = [];
    let inputWaiting = false;
    let currentInputResolve = null;

    function runCode() {
      const rawCode = editor.value.trim();
      if (!rawCode) {
        outputLine('لا يوجد كود للتشغيل', 'warn');
        return;
      }

      if (isRunning) {
        outputLine('الكود قيد التشغيل بالفعل', 'warn');
        return;
      }

      // Inject imported library code
      let code = rawCode;
      const libNames = DhadLibraries.getNames();
      libNames.forEach(name => {
        if (code.includes('استورد "' + name + '"') || code.includes("استورد '" + name + "'")) {
          const libCode = DhadLibraries.getCode(name);
          code = libCode + '\n' + code;
        }
      });

      isRunning = true;
      statusDot.className = 'status-dot running';
      statusText.textContent = 'جاري التشغيل...';
      outputConsole.innerHTML = '';
      inputLine.style.display = 'none';
      inputQueue = [];
      inputWaiting = false;

      // Show canvas if code uses graphics
      const previewCanvas = document.getElementById('previewCanvas');
      const canvasPlaceholder = document.getElementById('canvasPlaceholder');
      if (code.includes('محرك_جديد') || code.includes('ارسم_') || code.includes('مسح_الشاشة')) {
        previewCanvas.style.display = 'block';
        canvasPlaceholder.style.display = 'none';
        switchBottomTab('preview', null);
        document.querySelectorAll('.bottom-tab').forEach(t => t.classList.remove('active'));
        document.querySelectorAll('.bottom-tab')[1].classList.add('active');
      } else {
        previewCanvas.style.display = 'none';
        canvasPlaceholder.style.display = 'block';
      }

      outputLine('▶ جاري تشغيل الكود...', 'info');
      const startTime = performance.now();

      try {
        const jsCode = DhadInterpreter.transpile(code);
        transpiledCode.textContent = jsCode;

        // Execute with 30s timeout
        executionTimeout = setTimeout(() => {
          outputLine('تم إيقاف التنفيذ: تجاوز الوقت المحدد (30 ثانية)', 'error');
          finishExecution(performance.now() - startTime);
          inputLine.style.display = 'none';
        }, 30000);

        // Execute the code
        const result = DhadInterpreter.execute(code);

        clearTimeout(executionTimeout);

        if (result.output) {
          result.output.split('\n').forEach(line => {
            outputLine(line, 'success');
          });
        }

        if (result.errors && result.errors.length > 0) {
          result.errors.forEach(err => {
            outputLine('✗ ' + err, 'error');
          });
          statusDot.className = 'status-dot error';
        }

        // Check if input is pending — the animation loop will show overlay via __dhad_input
        if (window.__dhad_pendingInput) {
          // Overlay is being created by __dhad_input via setTimeout
          // Don't finish execution yet — keep isRunning true
        } else {
          const elapsed = performance.now() - startTime;
          finishExecution(elapsed, result.success);
          inputLine.style.display = 'none';
        }

        // Focus canvas for keyboard events in graphics programs
        if (previewCanvas.style.display === 'block') {
          setTimeout(function() {
            previewCanvas.tabIndex = 1;
            previewCanvas.style.outline = 'none';
            previewCanvas.focus();
          }, 100);
        }

      } catch (err) {
        outputLine('✗ خطأ غير متوقع: ' + err.message, 'error');
        statusDot.className = 'status-dot error';
        finishExecution(performance.now() - startTime, false);
        inputLine.style.display = 'none';
      }
    }

    // Handle inline input submission
    function submitInput() {
      const value = inlineInput.value;
      outputLine('▶ ' + value, 'info');
      inputLine.style.display = 'none';
      inputWaiting = false;
      if (currentInputResolve) {
        currentInputResolve(value);
        currentInputResolve = null;
      }
    }

    // Show overlay for __dhad_input() (non-blocking input)
    function showInputOverlay() {
      const msg = window.__dhad_inputMsg || 'أدخل قيمة:';
      let overlay = document.getElementById('__dhad_input_overlay');
      if (!overlay) {
        overlay = document.createElement('div');
        overlay.id = '__dhad_input_overlay';
        overlay.style.cssText = 'position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.7);display:flex;align-items:center;justify-content:center;z-index:99999';
        const box = document.createElement('div');
        box.style.cssText = 'background:#1e293b;border:2px solid #3b82f6;border-radius:12px;padding:24px;text-align:center;min-width:300px;direction:rtl';
        const label = document.createElement('div');
        label.id = '__dhad_input_label';
        label.style.cssText = 'color:#e2e8f0;font-size:16px;margin-bottom:12px;font-family:Arial,sans-serif';
        box.appendChild(label);
        const inp = document.createElement('input');
        inp.id = '__dhad_input_field';
        inp.type = 'text';
        inp.style.cssText = 'width:100%;padding:10px;font-size:16px;border:1px solid #475569;border-radius:6px;background:#0f172a;color:#fff;outline:none;font-family:monospace;text-align:center;direction:ltr';
        box.appendChild(inp);
        const btn = document.createElement('button');
        btn.id = '__dhad_input_btn';
        btn.textContent = 'تأكيد';
        btn.style.cssText = 'margin-top:12px;padding:8px 24px;background:#3b82f6;color:#fff;border:none;border-radius:6px;cursor:pointer;font-size:14px';
        box.appendChild(btn);
        overlay.appendChild(box);
        document.body.appendChild(overlay);
      }
      overlay.style.display = 'flex';
      const label = document.getElementById('__dhad_input_label');
      label.textContent = msg;
      const inp = document.getElementById('__dhad_input_field');
      inp.value = '';
      // Remove old listeners by replacing element
      const newInp = inp.cloneNode(true);
      inp.parentNode.replaceChild(newInp, inp);
      const newBtn = document.getElementById('__dhad_input_btn');
      const newBtn2 = newBtn.cloneNode(true);
      newBtn.parentNode.replaceChild(newBtn2, newBtn);
      function submitOverlay() {
        const value = newInp.value;
        overlay.style.display = 'none';
        window.__dhad_inputResult = value;
        window.__dhad_pendingInput = false;
        if (window.__dhad_continue) {
          window.__dhad_continue();
        }
        // After resuming, check if another input is needed (loop may have re-triggered)
        setTimeout(function() {
          if (window.__dhad_pendingInput) {
            showInputOverlay();
          }
        }, 50);
      }
      newInp.addEventListener('keydown', function(e) {
        if (e.key === 'Enter') {
          e.preventDefault();
          submitOverlay();
        }
        e.stopPropagation();
      });
      newBtn2.addEventListener('click', submitOverlay);
      setTimeout(function() { newInp.focus(); }, 50);
    }

    inlineInput.addEventListener('keydown', function(e) {
      if (e.key === 'Enter') {
        e.preventDefault();
        submitInput();
      }
      e.stopPropagation();
    });

    inputSubmit.addEventListener('click', submitInput);

    function finishExecution(elapsed, success) {
      isRunning = false;
      const time = elapsed.toFixed(1);
      if (success === true) {
        outputLine('✓ تم التنفيذ بنجاح (' + time + ' مللي ثانية)', 'success');
        statusDot.className = 'status-dot';
      } else if (success === false) {
        statusDot.className = 'status-dot error';
      } else {
        statusDot.className = 'status-dot';
      }
      statusText.textContent = 'تم التنفيذ في ' + time + ' مللي ثانية';
      outputConsole.scrollTop = 0;
    }

    function stopExecution() {
      if (executionTimeout) {
        clearTimeout(executionTimeout);
        executionTimeout = null;
      }
      isRunning = false;
      outputLine('⛔ تم إيقاف التنفيذ', 'warn');
      statusDot.className = 'status-dot';
      statusText.textContent = 'تم الإيقاف';
    }

    function outputLine(text, type) {
      const line = document.createElement('div');
      line.className = 'output-line ' + (type || '');
      line.textContent = text;
      outputConsole.appendChild(line);
    }

    function clearOutput() {
      outputConsole.innerHTML = '';
      transpiledCode.textContent = 'اضغط "تشغيل" لرؤية الكود المترجم';
      statusDot.className = 'status-dot';
      statusText.textContent = 'جاهز';
    }

    /* ═══════════════════════════════════════════════════════════════════
       File Management (localStorage)
       ═══════════════════════════════════════════════════════════════════ */

    function getFiles() {
      try {
        return JSON.parse(localStorage.getItem('dhad_files') || '{}');
      } catch (e) { return {}; }
    }

    function saveFile() {
      const name = fileNameInput.value.trim() || 'program.ض';
      const code = editor.value;
      const files = getFiles();
      files[name] = { code: code, savedAt: new Date().toISOString() };
      localStorage.setItem('dhad_files', JSON.stringify(files));
      currentFile = name;
      updateFilesList();
      showToast('تم حفظ: ' + name, 'success');
    }

    function loadFile(name) {
      const files = getFiles();
      if (files[name]) {
        editor.value = files[name].code;
        fileNameInput.value = name;
        currentFile = name;
        updateHighlight();
        updateLineNumbers();
        hideFiles();
        showToast('تم فتح: ' + name, 'info');
      }
    }

    function deleteFile(name, e) {
      e.stopPropagation();
      if (!confirm('هل تريد حذف "' + name + '"؟')) return;
      const files = getFiles();
      delete files[name];
      localStorage.setItem('dhad_files', JSON.stringify(files));
      if (currentFile === name) currentFile = null;
      updateFilesList();
      showToast('تم حذف: ' + name, 'info');
    }

    function updateFilesList() {
      const container = document.getElementById('filesList');
      if (!container) return;
      const files = getFiles();
      const names = Object.keys(files);
      if (names.length === 0) {
        container.innerHTML = '<div class="empty-state">لا توجد ملفات محفوظة</div>';
        return;
      }
      container.innerHTML = names.map(name => {
        const date = new Date(files[name].savedAt).toLocaleDateString('ar');
        return '<div class="file-item" onclick="loadFile(\'' + name.replace(/'/g, "\\'") + '\')">' +
               '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="var(--primary)" stroke-width="2"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/></svg>' +
               '<span class="file-name-text">' + escapeHtml(name) + '</span>' +
               '<span style="font-size:10px;color:var(--text-muted)">' + date + '</span>' +
               '<button class="file-delete" onclick="deleteFile(\'' + name.replace(/'/g, "\\'") + '\', event)">×</button>' +
               '</div>';
      }).join('');
    }

    function showFiles() {
      updateFilesList();
      document.getElementById('filesModal').classList.add('active');
    }

    function hideFiles() {
      document.getElementById('filesModal').classList.remove('active');
    }

    // Auto-save
    let autoSaveTimer = null;
    function saveToAutoSave() {
      clearTimeout(autoSaveTimer);
      autoSaveTimer = setTimeout(() => {
        localStorage.setItem('dhad_autosave', editor.value);
      }, 2000);
    }

    function loadAutoSave() {
      const saved = localStorage.getItem('dhad_autosave');
      if (saved && !editor.value) {
        editor.value = saved;
      }
    }

    /* ═══════════════════════════════════════════════════════════════════
       Templates
       ═══════════════════════════════════════════════════════════════════ */

    const templates = [
      {
        name: 'مرحبا بالعالم',
        desc: 'أول برنامج في لغة ض',
        code: 'طباعة("مرحبا بالعالم!")\nطباعة("أهلاً بك في لغة ض")'
      },
      {
        name: 'المتغيرات',
        desc: 'تعريف المتغيرات وأنواع البيانات',
        code: 'صحيح اسم = "محمد"\nصحيح رسالة = "مرحبا"\nصحيح عمر = 25\nصحيح مسجل = صواب\n\nطباعة(رسالة + " يا " + اسم)\nطباعة("عمرك " + عمر)\nطباعة("التسجيل: " + مسجل)'
      },
      {
        name: 'الشروط',
        desc: 'استخدام اذا وإلا',
        code: 'صحيح درجة = 85\n\nإذا (درجة >= 90) {\n  طباعة("ممتاز!")\n} وإلا إذا (درجة >= 80) {\n  طباعة("جيد جداً")\n} وإلا إذا (درجة >= 70) {\n  طباعة("جيد")\n} وإلا {\n  طباعة("يحتاج تحسين")\n}'
      },
      {
        name: 'التكرار',
        desc: 'حلقات التكرار لكل وطالما',
        code: '# تكرار لكل\nلكل (صحيح i = 0؛ i < 5؛ i++) {\n  طباعة("العدد: " + i)\n}\n\n# تكرار طالما\nصحيح عداد = 0\nطالما (عداد < 3) {\n  طباعة("العدد: " + عداد)\n  عداد++\n}'
      },
      {
        name: 'الدوال',
        desc: 'تعريف واستدعاء الدوال',
        code: 'دالة جمع(a, b) {\n  ارجع a + b\n}\n\nدالة تحية(اسم) {\n  ارجع "مرحبا يا " + اسم + "!"\n}\n\nطباعة("في قديم الزمان...")\nصحيح نت1 = جمع(5، 3)\nطباعة("5 + 3 = " + نت1)\nطباعة(تحية("أحمد"))'
      },
      {
        name: 'الفئات',
        desc: 'البرمجة الكائنية التوجه',
        code: 'فئة حيوان {\n  دالة() {\n    هذا.اسم = "حيوان"\n  }\n  دالة صوت() {\n    طباعة("صوت الحيوان")\n  }\n}\n\nجديد حيوان1 = حيوان()\nحيوان1.صوت()'
      },
      {
        name: 'معالجة الأخطاء',
        desc: 'استخدام حاول وامسك',
        code: 'حاول {\n  صحيح نتيجة = 10 / 0\n  طباعة(نتيجة)\n} امسك {\n  طباعة("حدث خطأ!")\n}'
      },
      {
        name: 'مصفوفة',
        desc: 'إنشاء والتعامل مع المصفوفات',
        code: 'ثابت ألوان = ["أحمر"، "أزرق"، "أخضر"]\n\n# طباعة جميع الألوان\nلكل (صحيح i = 0؛ i < ألوان.length؛ i++) {\n  طباعة("اللون: " + ألوان[i])\n}\n\n# طباعة العدد\nطباعة("عدد الألوان: " + ألوان.length)'
      },
      {
        name: 'الإدخال',
        desc: 'التفاعل مع المستخدم',
        code: 'صحيح اسم = ادخال("اكتب اسمك:")\n\nإذا (اسم) {\n  طباعة("مرحبا يا " + اسم + "!")\n} وإلا {\n  طباعة("لم تكتب اسماً")\n}'
      },
      {
        name: 'محرك رسومي',
        desc: 'إنشاء محرك رسومي ثنائي الأبعاد',
        code: '# استورد "المحرك_الرسومي"\n\nصحيح لوح = محرك_جديد(600، 400)\nمسح_الشاشة("#111")\n\n# رسم أشكال\nارسم_مستطيل(100، 100، 200، 150، "#10b981")\nارسم_دائرة(400، 200، 50، "#3b82f6")\nارسم_نص("مرحبا بعالم ض!", 200، 350، "#fff", 24)\n\nطباعة("تم الرسم بنجاح!")'
      },
      {
        name: 'لعبة بسيطة',
        desc: 'لعبة حركة بسيطة',
        code: '# استورد "المحرك_الرسومي"\n\nصحيح لوح = محرك_جديد(400، 300)\nصحيح x = 200\nصحيح y = 150\nصحيح حجم = 20\n\nدالة رسم() {\n  مسح_الشاشة("#111")\n  ارسم_مستطيل(x، y، حجم، حجم، "#10b981")\n  ارسم_نص("استخدم الأسهم للحركة", 100، 280، "#666", 14)\n}\n\nابدأ_التحريك(دالة( dt) {\n  اذا (مفتاح_مضغوط("ArrowUp")) { y = y - 3 }\n  اذا (مفتاح_مضغوط("ArrowDown")) { y = y + 3 }\n  اذا (مفتاح_مضغوط("ArrowLeft")) { x = x - 3 }\n  اذا (مفتاح_مضغوط("ArrowRight")) { x = x + 3 }\n  رسم()\n})'
      }
    ];

    function showTemplates() {
      const container = document.getElementById('templateList');
      container.innerHTML = templates.map((t, i) =>
        '<div class="template-card" onclick="loadTemplate(' + i + ')">' +
        '<h4>' + escapeHtml(t.name) + '</h4>' +
        '<p>' + escapeHtml(t.desc) + '</p>' +
        '</div>'
      ).join('');
      document.getElementById('templateModal').classList.add('active');
    }

    function hideTemplates() {
      document.getElementById('templateModal').classList.remove('active');
    }

    function loadTemplate(index) {
      editor.value = templates[index].code;
      fileNameInput.value = templates[index].name + '.ض';
      updateHighlight();
      updateLineNumbers();
      hideTemplates();
      showToast('تم تحميل القالب: ' + templates[index].name, 'success');
    }

    /* ═══════════════════════════════════════════════════════════════════
       Challenges Section
       ═══════════════════════════════════════════════════════════════════ */

    const challenges = [
      {
        id: 1,
        name: 'المحاسب',
        desc: 'اكتب برنامج يجمع عددين يدخلهما المستخدم',
        level: 'مبتدئ',
        points: 10,
        starter: 'صحيح ع1 = ادخال("أدخل العدد الأول:")\nصحيح ع2 = ادخال("أدخل العدد الثاني:")\n\n# أكمل الكود هنا',
        hint: 'استخدم: نتيجة = ع1 + ع2',
        test: function(code) {
          return code.includes('+') && (code.includes('طباعة') || code.includes('نتيجة'));
        }
      },
      {
        id: 2,
        name: 'فاحص العمر',
        desc: 'اكتب برنامج يتحقق من عمر المستخدم ويحدد فئته',
        level: 'مبتدئ',
        points: 15,
        starter: 'صحيح عمر = ادخال("أدخل عمرك:")\nعمر = Number(عمر)\n\n# أكمل الكود هنا',
        hint: 'استخدم إذا وإلا إذا لمقارنة العمر مع 13 و 18',
        test: function(code) {
          return code.includes('إذا') && code.includes('age');
        }
      },
      {
        id: 3,
        name: 'محرك رسومي',
        desc: 'اكتب لعبة بسيطة باستخدام المحرك الرسومي',
        level: 'متقدم',
        points: 30,
        starter: '# استورد "المحرك_الرسومي"\n\nصحيح لوح = محرك_جديد(400، 300)\n\n# أكمل الكود هنا',
        hint: 'استخدم مسح_الشاشة و ارسم_مستطيل و ابدأ_التحريك',
        test: function(code) {
          return code.includes('محرك_جديد') && code.includes('ارسم_');
        }
      },
      {
        id: 4,
        name: 'لعبة الثعبان',
        desc: 'اكتب لعبة ثعبان نصية باستخدام المحرك',
        level: 'خبير',
        points: 50,
        starter: '# استورد "المحرك_الرسومي"\n\nصحيح لوح = محرك_جديد(600، 400)\nصحيح ثعبان_x = [300، 290، 280]\nصحيح ثعبان_y = [200، 200، 200]\nصحيح طعام_x = 100\nصحيح طعام_y = 100\nصحيح اتجاه = 1\n\n# أكمل الكود هنا',
        hint: 'استخدم ابدأ_التحريك مع معالج يحرك الثعبان ويتحقق من التصادم',
        test: function(code) {
          return code.includes('محرك_جديد') && code.includes('ابدأ_التحريك') && code.includes('مفتاح_مضغوط');
        }
      },
      {
        id: 5,
        name: 'آلة حاسبة',
        desc: 'اكتب آلة حاسبة تدعم 4 عمليات',
        level: 'مبتدئ',
        points: 20,
        starter: 'طباعة("الآلة الحاسبة")\nصحيح ع1 = ادخال("العدد الأول:")\nصحيح ع2 = ادخال("العملي: 1=جمع 2=طرح 3=ضرب 4=قسمة")\n\n# أكمل الكود هنا',
        hint: 'استخدم إذا وإلا إذا لكل عملية حسابية',
        test: function(code) {
          return code.includes('إذا') && code.includes('+') && code.includes('-');
        }
      },
      {
        id: 6,
        name: 'رسم نجمة',
        desc: 'ارسم نجمة خماسية باستخدام المحرك الرسومي',
        level: 'متوسط',
        points: 25,
        starter: '# استورد "المحرك_الرسومي"\n\nصحيح لوح = محرك_جديد(400، 400)\n\n# أكمل الكود هنا',
        hint: 'استخدم ارسم_خط لرسم النجمة مع حساب الزوايا',
        test: function(code) {
          return code.includes('محرك_جديد') && code.includes('ارسم_خط');
        }
      }
    ];

    let currentChallenge = null;

    function showChallenges() {
      const container = document.getElementById('challengesList');
      container.innerHTML = challenges.map(c => {
        const levelColor = c.level === 'مبتدئ' ? 'var(--success)' : c.level === 'متوسط' ? 'var(--warning)' : c.level === 'متقدم' ? 'var(--info)' : 'var(--danger)';
        return '<div class="challenge-card" style="background:var(--bg-input);border:1px solid var(--border);border-radius:8px;padding:16px;margin-bottom:12px;cursor:pointer" onclick="loadChallenge(' + c.id + ')">' +
          '<div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px">' +
            '<h4 style="margin:0;color:var(--text)">' + escapeHtml(c.name) + '</h4>' +
            '<span style="background:' + levelColor + '20;color:' + levelColor + ';padding:2px 8px;border-radius:4px;font-size:11px">' + escapeHtml(c.level) + '</span>' +
          '</div>' +
          '<p style="margin:0 0 8px;color:var(--text-muted);font-size:12px">' + escapeHtml(c.desc) + '</p>' +
          '<div style="display:flex;justify-content:space-between;align-items:center">' +
            '<span style="font-size:11px;color:var(--warning)">⭐ ' + c.points + ' نقطة</span>' +
            '<button class="btn btn-run" style="padding:4px 12px;font-size:11px">حل التحدي</button>' +
          '</div>' +
        '</div>';
      }).join('');
      document.getElementById('challengesModal').classList.add('active');
    }

    function hideChallenges() {
      document.getElementById('challengesModal').classList.remove('active');
    }

    function loadChallenge(id) {
      const challenge = challenges.find(c => c.id === id);
      if (!challenge) return;
      currentChallenge = challenge;
      editor.value = challenge.starter;
      fileNameInput.value = 'تحدي_' + challenge.name + '.ض';
      updateHighlight();
      updateLineNumbers();
      hideChallenges();
      showToast('تم تحميل التحدي: ' + challenge.name, 'success');
      outputLine('🏆 التحدي: ' + challenge.name, 'info');
      outputLine('📋 ' + challenge.desc, 'info');
      outputLine('💡 تلميح: ' + challenge.hint, 'warn');
      outputLine('▶ أكمل الكود ثم اضغط تشغيل', 'info');
    }

    function checkChallenge() {
      if (!currentChallenge) return false;
      const code = editor.value.trim();
      const passed = currentChallenge.test(code);
      if (passed) {
        outputLine('🎉 أحسنت! لقد أكملت التحدي: ' + currentChallenge.name, 'success');
        outputLine('⭐ نقاط: ' + currentChallenge.points, 'success');
        showToast('🎉 تم إكمال التحدي!', 'success');
      } else {
        outputLine('❌ لم يمر الاختبار بعد. حاول مرة أخرى!', 'error');
        outputLine('💡 تلميح: ' + currentChallenge.hint, 'warn');
      }
      return passed;
    }

    /* ═══════════════════════════════════════════════════════════════════
       Libraries Panel
       ═══════════════════════════════════════════════════════════════════ */

    function renderLibraries() {
      const libs = DhadLibraries.getAll();
      const container = document.getElementById('libsList');

      container.innerHTML = libs.map(lib => {
        return '<div class="lib-item" onclick="importLibrary(\'' + lib.name + '\')">' +
               '<div class="lib-icon" style="background:rgba(16,185,129,0.15);color:var(--success)">' + escapeHtml(lib.icon) + '</div>' +
               '<div>' +
               '<div class="lib-name">' + escapeHtml(lib.name) + '</div>' +
               '<div class="lib-desc">' + escapeHtml(lib.desc) + '</div>' +
               '</div>' +
               '</div>';
      }).join('');
    }

    function importLibrary(name) {
      const code = DhadLibraries.getImportCode(name);
      const pos = editor.selectionStart;
      const lineStart = editor.value.lastIndexOf('\n', pos - 1) + 1;

      // Check if already imported
      if (editor.value.includes('# استورد "' + name + '"') || editor.value.includes('استورد "' + name + '"')) {
        showToast('المكتبة "' + name + '" مستوردة بالفعل', 'info');
        return;
      }

      editor.value = editor.value.substring(0, lineStart) + code + '\n' + editor.value.substring(lineStart);
      updateHighlight();
      updateLineNumbers();
      updateImportedLibs();
      showToast('تم استيراد مكتبة: ' + name, 'success');
    }

    function updateImportedLibs() {
      const container = document.getElementById('importedLibs');
      const imported = [];
      const libs = DhadLibraries.getNames();
      libs.forEach(lib => {
        if (editor.value.includes('# استورد "' + lib + '"') || editor.value.includes('استورد "' + lib + '"')) {
          imported.push(lib);
        }
      });

      if (imported.length === 0) {
        container.innerHTML = '<div class="empty-state">لم يتم استيراد أي مكتبة</div>';
      } else {
        container.innerHTML = imported.map(lib => {
          const libData = DhadLibraries.getAll().find(l => l.name === lib);
          const icon = libData ? libData.icon : '📦';
          return '<div class="lib-item">' +
          '<span style="font-size:14px">' + icon + '</span>' +
          '<span style="color:var(--success)">' + lib + '</span>' +
          '</div>';
        }).join('');
      }
    }

    /* ═══════════════════════════════════════════════════════════════════
       Panel Switching
       ═══════════════════════════════════════════════════════════════════ */

    function switchPanel(panel, e) {
      document.querySelectorAll('.panel-tab').forEach(t => t.classList.remove('active'));
      document.querySelectorAll('.panel-section').forEach(s => s.classList.remove('active'));
      (e || event).target.classList.add('active');
      document.getElementById('panel-' + panel).classList.add('active');
    }

    function toggleSidePanel() {
      const panel = document.getElementById('sidePanel');
      const btn = document.getElementById('panelToggle');
      panel.classList.toggle('collapsed');
      btn.textContent = panel.classList.contains('collapsed') ? '▶' : '◀';
    }

    function switchBottomTab(tab, e) {
      document.querySelectorAll('.bottom-tab').forEach(t => t.classList.remove('active'));
      document.querySelectorAll('.bottom-section').forEach(s => s.classList.remove('active'));
      if (e && e.target) {
        e.target.classList.add('active');
      } else {
        var tabMap = {'output': 0, 'preview': 1, 'transpiled': 2};
        var idx = tabMap[tab];
        if (idx !== undefined) {
          var tabs = document.querySelectorAll('.bottom-tab');
          if (tabs[idx]) tabs[idx].classList.add('active');
        }
      }
      document.getElementById('section-' + tab).classList.add('active');
    }

    function toggleBottomPanel() {
      document.getElementById('bottomPanel').classList.toggle('collapsed');
    }

    /* ═══════════════════════════════════════════════════════════════════
       Code Formatting
       ═══════════════════════════════════════════════════════════════════ */

    function formatCode() {
      let code = editor.value;
      // Basic formatting: normalize spaces, ensure proper indentation
      const lines = code.split('\n');
      let indent = 0;
      const formatted = lines.map(line => {
        const trimmed = line.trim();
        if (!trimmed) return '';

        // Decrease indent for closing braces
        if (trimmed.startsWith('}') || trimmed.startsWith(')')) {
          indent = Math.max(0, indent - 1);
        }

        const result = '  '.repeat(indent) + trimmed;

        // Increase indent for opening braces
        if (trimmed.endsWith('{') || trimmed.endsWith('(')) {
          indent++;
        }

        return result;
      });

      editor.value = formatted.join('\n');
      updateHighlight();
      updateLineNumbers();
      showToast('تم تنسيق الكود', 'success');
    }

    /* ═══════════════════════════════════════════════════════════════════
       Toast Notifications
       ═══════════════════════════════════════════════════════════════════ */

    function showToast(message, type) {
      const existing = document.querySelector('.toast-notification');
      if (existing) existing.remove();

      const toast = document.createElement('div');
      toast.className = 'toast-notification';
      toast.style.cssText = 'position:fixed;top:60px;left:50%;transform:translateX(-50%);' +
        'background:var(--bg-card);border:1px solid var(--' + (type === 'success' ? 'success' : type === 'error' ? 'danger' : 'info') + ');' +
        'border-radius:8px;padding:10px 20px;font-size:13px;z-index:300;box-shadow:var(--shadow);' +
        'color:var(--text);font-family:inherit;animation:fadeIn 0.2s';
      toast.textContent = message;
      document.body.appendChild(toast);
      setTimeout(() => {
        toast.style.opacity = '0';
        toast.style.transition = 'opacity 0.3s';
        setTimeout(() => toast.remove(), 300);
      }, 2000);
    }

    /* ═══════════════════════════════════════════════════════════════════
       Resize Handles
       ═══════════════════════════════════════════════════════════════════ */

    // Horizontal resize (side panel)
    const resizeH = document.getElementById('resizeH');
    const sidePanel = document.getElementById('sidePanel');
    let isResizingH = false;

    resizeH.addEventListener('mousedown', function(e) {
      isResizingH = true;
      document.body.style.cursor = 'col-resize';
      document.body.style.userSelect = 'none';
    });

    // Vertical resize (bottom panel)
    const resizeV = document.getElementById('resizeV');
    const bottomPanel = document.getElementById('bottomPanel');
    let isResizingV = false;

    resizeV.addEventListener('mousedown', function(e) {
      isResizingV = true;
      document.body.style.cursor = 'row-resize';
      document.body.style.userSelect = 'none';
    });

    document.addEventListener('mousemove', function(e) {
      if (isResizingH) {
        const newWidth = window.innerWidth - e.clientX;
        sidePanel.style.width = Math.max(200, Math.min(400, newWidth)) + 'px';
      }
      if (isResizingV) {
        const newHeight = window.innerHeight - e.clientY;
        bottomPanel.style.height = Math.max(100, Math.min(500, newHeight)) + 'px';
      }
    });

    document.addEventListener('mouseup', function() {
      isResizingH = false;
      isResizingV = false;
      document.body.style.cursor = '';
      document.body.style.userSelect = '';
    });

    /* ═══════════════════════════════════════════════════════════════════
       Keyboard Shortcuts
       ═══════════════════════════════════════════════════════════════════ */

    document.addEventListener('keydown', function(e) {
      // Ctrl+Enter = Run
      if (e.ctrlKey && e.key === 'Enter') {
        e.preventDefault();
        runCode();
      }

      // Ctrl+S = Save
      if (e.ctrlKey && e.key === 's') {
        e.preventDefault();
        if (projectDirHandle && currentFilePath) {
          saveCurrentFile();
        } else {
          saveFile();
        }
      }

      // Escape = Close modals
      if (e.key === 'Escape') {
        hideTemplates();
        hideFiles();
        hideAutocomplete();
        hideCtxMenu();
      }
    });

    /* ═══════════════════════════════════════════════════════════════════
       File Explorer - Project Manager
       ═══════════════════════════════════════════════════════════════════ */

    let projectDirHandle = null;
    let openFiles = new Map();
    let currentFilePath = null;
    let ctxTarget = null;

    const fileIcons = {
      daad: '🟣', js: '🟨', ts: '🔷', html: '🟧', css: '🎨',
      json: '📋', md: '📝', py: '🐍', txt: '📄', default: '📄'
    };

    function getFileIcon(name) {
      const ext = name.split('.').pop().toLowerCase();
      return fileIcons[ext] || fileIcons.default;
    }

    async function openProjectFolder() {
      if (!('showDirectoryPicker' in window)) {
        alert('متصفحك لا يدعم فتح المجلدات. استخدم Chrome أو Edge.');
        return;
      }
      try {
        projectDirHandle = await window.showDirectoryPicker({ mode: 'readwrite' });
        document.getElementById('projectClosed').style.display = 'none';
        document.getElementById('projectOpen').style.display = 'block';
        document.getElementById('projectNameText').textContent = projectDirHandle.name;
        await refreshTree();
      } catch (e) {
        if (e.name !== 'AbortError') console.error(e);
      }
    }

    function closeProject() {
      projectDirHandle = null;
      openFiles.clear();
      currentFilePath = null;
      document.getElementById('projectClosed').style.display = 'block';
      document.getElementById('projectOpen').style.display = 'none';
      document.getElementById('fileTree').innerHTML = '';
    }

    async function refreshTree() {
      if (!projectDirHandle) return;
      const tree = document.getElementById('fileTree');
      tree.innerHTML = '<div style="padding:8px;color:var(--text-muted);font-size:12px">جاري تحميل الملفات...</div>';
      try {
        const entries = await readDirSorted(projectDirHandle);
        tree.innerHTML = '';
        if (entries.length === 0) {
          tree.innerHTML = '<div style="padding:8px;color:var(--text-muted);font-size:12px">المجلد فارغ</div>';
          return;
        }
        for (const entry of entries) {
          const el = await createTreeItem(entry, '');
          tree.appendChild(el);
        }
      } catch (e) {
        tree.innerHTML = '<div style="padding:8px;color:#f44;font-size:12px">خطأ في تحميل الملفات: ' + e.message.replace(/</g,'&lt;').replace(/>/g,'&gt;') + '</div>';
        console.error('refreshTree error:', e);
      }
    }

    async function readDirSorted(dirHandle) {
      const entries = [];
      for await (const [name, handle] of dirHandle) {
        entries.push({ name, handle });
      }
      entries.sort((a, b) => {
        if (a.handle.kind === b.handle.kind) return a.name.localeCompare(b.name);
        return a.handle.kind === 'directory' ? -1 : 1;
      });
      return entries;
    }

    async function createTreeItem(entry, parentPath) {
      const isDir = entry.handle.kind === 'directory';
      const fullPath = parentPath ? parentPath + '/' + entry.name : entry.name;
      const item = document.createElement('div');

      const row = document.createElement('div');
      row.className = 'tree-item';
      row.dataset.path = fullPath;
      row.dataset.type = isDir ? 'dir' : 'file';
      row.dataset.name = entry.name;

      let childrenContainer = null;
      let isExpanded = false;

      if (isDir) {
        const toggle = document.createElement('span');
        toggle.className = 'tree-toggle';
        toggle.textContent = '▶';
        row.appendChild(toggle);

        const icon = document.createElement('span');
        icon.className = 'tree-icon';
        icon.textContent = '📁';
        row.appendChild(icon);

        childrenContainer = document.createElement('div');
        childrenContainer.className = 'tree-children collapsed';
        item.appendChild(childrenContainer);

        row.addEventListener('click', async (e) => {
          if (e.target.closest('.tree-actions')) return;
          isExpanded = !isExpanded;
          toggle.classList.toggle('expanded', isExpanded);
          icon.textContent = isExpanded ? '📂' : '📁';
          childrenContainer.classList.toggle('collapsed', !isExpanded);
          if (isExpanded && childrenContainer.children.length === 0) {
            const children = await readDirSorted(entry.handle);
            for (const child of children) {
              const childEl = await createTreeItem(child, fullPath);
              childrenContainer.appendChild(childEl);
            }
          }
        });
      } else {
        const spacer = document.createElement('span');
        spacer.style.width = '14px';
        spacer.style.flexShrink = '0';
        row.appendChild(spacer);

        const icon = document.createElement('span');
        icon.className = 'tree-icon';
        icon.textContent = getFileIcon(entry.name);
        row.appendChild(icon);
      }

      const nameSpan = document.createElement('span');
      nameSpan.className = 'tree-name';
      nameSpan.textContent = entry.name;
      row.appendChild(nameSpan);

      const actions = document.createElement('span');
      actions.className = 'tree-actions';
       actions.innerHTML = '<button class="tree-action-btn" title="إعادة تسمية" onclick="event.stopPropagation();startRename(\'' + fullPath.replace(/'/g, "\\'") + '\',\'' + (isDir?'dir':'file') + '\')">' +
        '<svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M17 3a2.828 2.828 0 1 1 4 4L7.5 20.5 2 22l1.5-5.5L17 3z"/></svg></button>' +
        '<button class="tree-action-btn delete" title="حذف" onclick="event.stopPropagation();ctxDeleteItem(\'' + fullPath.replace(/'/g, "\\'") + '\',\'' + (isDir?'dir':'file') + '\',\'' + entry.name.replace(/'/g, "\\'") + '\')">' +
        '<svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="3 6 5 6 21 6"/><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/></svg></button>';
      row.appendChild(actions);

      if (!isDir) {
        row.addEventListener('click', async (e) => {
          if (e.target.closest('.tree-actions')) return;
          await openFileFromProject(entry.handle, entry.name, fullPath);
        });
      }
      row.addEventListener('contextmenu', (e) => {
        e.preventDefault();
        showCtxMenu(e, { handle: entry.handle, name: entry.name, type: isDir ? 'dir' : 'file', path: fullPath });
      });

      item.prepend(row);
      return item;
    }

    async function openFileFromProject(fileHandle, name, path) {
      try {
        const file = await fileHandle.getFile();
        const content = await file.text();
        if (currentFilePath && openFiles.has(currentFilePath)) {
          openFiles.get(currentFilePath).content = editor.value;
        }
        openFiles.set(path, { handle: fileHandle, content: content, originalContent: content, modified: false });
        currentFilePath = path;
        editor.value = content;
        document.getElementById('fileName').value = name;
        updateHighlight();
        updateLineNumbers();
        updateFileTreeActive();
        updateEditorTabs();
      } catch (e) {
        console.error('Error opening file:', e);
      }
    }

    function updateFileTreeActive() {
      document.querySelectorAll('.tree-item').forEach(el => {
        el.classList.toggle('active', el.dataset.path === currentFilePath);
      });
    }

    function updateEditorTabs() {
      const tabs = document.getElementById('editorTabs');
      if (!tabs) return;
      tabs.innerHTML = '';
      for (const [path, data] of openFiles) {
        const name = path.split('/').pop();
        const tab = document.createElement('div');
        tab.className = 'editor-tab' + (path === currentFilePath ? ' active' : '');
        const span = document.createElement('span');
        span.textContent = name;
        tab.appendChild(span);
        if (data.modified) {
          const dot = document.createElement('span');
          dot.style.cssText = 'color:var(--warning);font-size:10px';
          dot.textContent = '●';
          tab.appendChild(dot);
        }
        const closeBtn = document.createElement('span');
        closeBtn.className = 'tab-close';
        closeBtn.textContent = '✕';
        closeBtn.onclick = function(e) { e.stopPropagation(); closeEditorTab(path); };
        tab.appendChild(closeBtn);
        tab.addEventListener('click', function() { switchToTab(path); });
        tabs.appendChild(tab);
      }
    }

    async function switchToTab(path) {
      if (currentFilePath && openFiles.has(currentFilePath)) {
        openFiles.get(currentFilePath).content = editor.value;
      }
      const data = openFiles.get(path);
      if (data) {
        currentFilePath = path;
        editor.value = data.content;
        document.getElementById('fileName').value = path.split('/').pop();
        updateHighlight();
        updateLineNumbers();
        updateFileTreeActive();
        updateEditorTabs();
      }
    }

    function closeEditorTab(path) {
      const data = openFiles.get(path);
      if (data && data.modified) {
        if (!confirm('الملف غير محفوظ. هل تريد إغلاقه؟')) return;
      }
      openFiles.delete(path);
      if (currentFilePath === path) {
        const remaining = [...openFiles.keys()];
        if (remaining.length > 0) {
          switchToTab(remaining[remaining.length - 1]);
        } else {
          currentFilePath = null;
          editor.value = '';
          document.getElementById('fileName').value = 'برنامج_جديد.ض';
          updateHighlight();
          updateLineNumbers();
        }
      }
      updateEditorTabs();
    }

    async function createNewFile(parentPath) {
      const name = prompt('اسم الملف الجديد:', 'ملف_جديد.ض');
      if (!name) return;
      let dirHandle = projectDirHandle;
      if (parentPath) {
        const parts = parentPath.split('/');
        for (const part of parts) {
          dirHandle = await dirHandle.getDirectoryHandle(part);
        }
      }
      try {
        const fileHandle = await dirHandle.getFileHandle(name, { create: true });
        const writable = await fileHandle.createWritable();
        await writable.write('');
        await writable.close();
        await refreshTree();
        await openFileFromProject(fileHandle, name, parentPath ? parentPath + '/' + name : name);
      } catch (e) {
        console.error(e);
        alert('خطأ في إنشاء الملف');
      }
    }

    async function createNewFolder(parentPath) {
      const name = prompt('اسم المجلد الجديد:', 'مجلد_جديد');
      if (!name) return;
      let dirHandle = projectDirHandle;
      if (parentPath) {
        const parts = parentPath.split('/');
        for (const part of parts) {
          dirHandle = await dirHandle.getDirectoryHandle(part);
        }
      }
      try {
        await dirHandle.getDirectoryHandle(name, { create: true });
        await refreshTree();
      } catch (e) {
        console.error(e);
        alert('خطأ في إنشاء المجلد');
      }
    }

    async function saveCurrentFile() {
      if (!currentFilePath || !openFiles.has(currentFilePath)) {
        saveFile();
        return;
      }
      const data = openFiles.get(currentFilePath);
      try {
        const writable = await data.handle.createWritable();
        await writable.write(editor.value);
        await writable.close();
        data.content = editor.value;
        data.originalContent = editor.value;
        data.modified = false;
        updateEditorTabs();
        updateFileTreeActive();
        showToast('تم الحفظ: ' + currentFilePath.split('/').pop(), 'success');
      } catch (e) {
        console.error(e);
        saveFile();
      }
    }

    function startRename(path, type) {
      const nameEl = document.querySelector('.tree-item[data-path="' + path + '"] .tree-name');
      if (!nameEl) return;
      const oldName = nameEl.textContent;
      const input = document.createElement('input');
      input.className = 'rename-input';
      input.value = oldName;
      nameEl.textContent = '';
      nameEl.appendChild(input);
      input.focus();
      input.select();

      async function finish() {
        const newName = input.value.trim();
        if (newName && newName !== oldName) {
          await renameItem(path, type, oldName, newName);
        } else {
          nameEl.textContent = oldName;
        }
      }
      input.addEventListener('blur', finish);
      input.addEventListener('keydown', function(e) {
        if (e.key === 'Enter') input.blur();
        if (e.key === 'Escape') { input.value = oldName; input.blur(); }
      });
    }

    async function renameItem(path, type, oldName, newName) {
      const parts = path.split('/');
      const parentParts = parts.slice(0, -1);
      let parentDir = projectDirHandle;
      for (const part of parentParts) {
        parentDir = await parentDir.getDirectoryHandle(part);
      }
      try {
        if (type === 'dir') {
          const oldDir = await parentDir.getDirectoryHandle(oldName);
          const newDir = await parentDir.getDirectoryHandle(newName, { create: true });
          await copyDirContents(oldDir, newDir);
          await parentDir.removeEntry(oldName, { recursive: true });
        } else {
          const oldFile = await parentDir.getFileHandle(oldName);
          const blob = await oldFile.getFile();
          const newFile = await parentDir.getFileHandle(newName, { create: true });
          const writable = await newFile.createWritable();
          await writable.write(blob);
          await writable.close();
          await parentDir.removeEntry(oldName);
        }
        const newPath = parentParts.length > 0 ? parentParts.join('/') + '/' + newName : newName;
        if (currentFilePath === path) {
          openFiles.delete(path);
          currentFilePath = newPath;
          openFiles.set(newPath, { handle: type === 'file' ? await parentDir.getFileHandle(newName) : null, content: editor.value, originalContent: editor.value, modified: true });
        }
        await refreshTree();
      } catch (e) {
        console.error(e);
        alert('خطأ في إعادة التسمية');
      }
    }

    async function copyDirContents(src, dst) {
      for await (const [name, handle] of src) {
        if (handle.kind === 'directory') {
          const sub = await dst.getDirectoryHandle(name, { create: true });
          await copyDirContents(handle, sub);
        } else {
          const file = await dst.getFileHandle(name, { create: true });
          const writable = await file.createWritable();
          await writable.write(await handle.getFile());
          await writable.close();
        }
      }
    }

    async function ctxDeleteItem(path, type, name) {
      if (!confirm('هل تريد حذف "' + name + '"؟')) return;
      const parts = path.split('/');
      const parentParts = parts.slice(0, -1);
      let dir = projectDirHandle;
      for (const part of parentParts) {
        dir = await dir.getDirectoryHandle(part);
      }
      try {
        await dir.removeEntry(name, { recursive: type === 'dir' });
        if (currentFilePath === path || (currentFilePath && currentFilePath.startsWith(path + '/'))) {
          openFiles.delete(path);
          currentFilePath = null;
          editor.value = '';
          document.getElementById('fileName').value = 'برنامج_جديد.ض';
          updateHighlight();
          updateLineNumbers();
          updateEditorTabs();
        }
        await refreshTree();
      } catch (e) {
        console.error(e);
        alert('خطأ في الحذف');
      }
    }

    function showCtxMenu(e, target) {
      ctxTarget = target;
      const menu = document.getElementById('ctxMenu');
      menu.style.left = e.clientX + 'px';
      menu.style.top = e.clientY + 'px';
      menu.classList.add('active');
    }

    function hideCtxMenu() {
      document.getElementById('ctxMenu').classList.remove('active');
      ctxTarget = null;
    }

    document.addEventListener('click', hideCtxMenu);

    function ctxNewFile() {
      hideCtxMenu();
      createNewFile(ctxTarget && ctxTarget.type === 'dir' ? ctxTarget.path : null);
    }

    function ctxNewFolder() {
      hideCtxMenu();
      createNewFolder(ctxTarget && ctxTarget.type === 'dir' ? ctxTarget.path : null);
    }

    function ctxRename() {
      hideCtxMenu();
      if (ctxTarget) startRename(ctxTarget.path, ctxTarget.type);
    }

    function ctxDelete() {
      hideCtxMenu();
      if (ctxTarget) ctxDeleteItem(ctxTarget.path, ctxTarget.type, ctxTarget.name);
    }

    /* ═══════════════════════════════════════════════════════════════════
       Initialization
       ═══════════════════════════════════════════════════════════════════ */

    function init() {
      loadAutoSave();
      updateHighlight();
      updateLineNumbers();
      updateCursorPos();
      updateImportedLibs();
      renderLibraries();
      updateFilesList();
      editor.focus();
    }

    init();