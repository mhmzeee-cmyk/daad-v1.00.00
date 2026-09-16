#!/usr/bin/env node
// ض (Dhad) — محول تصحيح DAP فوق gdb/MI.
// التصميم: المترجم يصدر توجيهات `#line N "prog.ض"` (CodeGen.emitSourceComment + StmtAST.line)
// فيبقى gdb نفسه يعرف أسطر .ض — المحول يمرر المواقع كما هي بلا جداول ربط.
// المراجع: docs/SEMANTIC_RULES.md (R-17)، وتصميم #line المختبر بـ gdb --batch.
'use strict';

const { DebugSession, InitializedEvent, TerminatedEvent, StoppedEvent, OutputEvent, BreakpointEvent,
	Thread, StackFrame, Scope, Source, Handles, Variable } = require('vscode-debugadapter');
const { spawn, execFile } = require('child_process');
const path = require('path');
const fs = require('fs');
const os = require('os');

/* ── محلل قيم GDB/MI (سلاسل مع escapes ثمانية للعربية + tuples + lists) ── */
function unescapeMI(s) {
	const bytes = [];
	for (let i = 0; i < s.length; i++) {
		const c = s[i];
		if (c === '\\' && i + 1 < s.length) {
			const n = s[i + 1];
			if (n === 'n') { bytes.push(10); i++; }
			else if (n === 't') { bytes.push(9); i++; }
			else if (n === 'r') { bytes.push(13); i++; }
			else if (n === '"') { bytes.push(34); i++; }
			else if (n === '\\') { bytes.push(92); i++; }
			else if (n >= '0' && n <= '7') {
				let oct = '', j = i + 1;
				while (j < s.length && oct.length < 3 && s[j] >= '0' && s[j] <= '7') { oct += s[j]; j++; }
				bytes.push(parseInt(oct, 8) & 0xFF); i = j - 1;
			} else { bytes.push(s.charCodeAt(n) & 0xFF); i++; }
		} else {
			for (const b of Buffer.from(c, 'utf8')) bytes.push(b);
		}
	}
	return Buffer.from(bytes).toString('utf8');
}

function parseMIValue(s, i) {
	if (s[i] === '"') {
		let out = '', closed = false;
		i++;
		while (i < s.length) {
			if (s[i] === '\\' && i + 1 < s.length) { out += s[i] + s[i + 1]; i += 2; continue; }
			if (s[i] === '"') { closed = true; i++; break; }
			out += s[i]; i++;
		}
		return [unescapeMI(out), i, closed];
	}
	if (s[i] === '{') {
		const obj = {}; i++;
		while (i < s.length && s[i] !== '}') {
			if (s[i] === ',') { i++; continue; }
			const m = /^[A-Za-z_][\w-]*=/.exec(s.slice(i));
			if (!m) break;
			const name = m[0].slice(0, -1); i += m[0].length;
			let v; [v, i] = parseMIValue(s, i);
			if (obj[name] === undefined) obj[name] = v;
			else if (Array.isArray(obj[name])) obj[name].push(v);
			else obj[name] = [obj[name], v];
		}
		return [obj, s[i] === '}' ? i + 1 : i];
	}
	if (s[i] === '[') {
		const arr = []; i++;
		while (i < s.length && s[i] !== ']') {
			if (s[i] === ',') { i++; continue; }
			const m = /^[A-Za-z_][\w-]*=/.exec(s.slice(i));
			if (m) {
				const name = m[0].slice(0, -1); i += m[0].length;
				let v; [v, i] = parseMIValue(s, i);
				arr.push({ __pair: name, value: v });
			} else {
				let v; [v, i] = parseMIValue(s, i);
				arr.push(v);
			}
		}
		return [arr, s[i] === ']' ? i + 1 : i];
	}
	return ['', i];
}

// [a,b,c] حيث العناصر {__pair,v} بنفس الاسم → [v,v..] وإلا كما هي
function miList(arr, pairName) {
	if (!Array.isArray(arr)) return [];
	return arr.map(it => (it && it.__pair === pairName) ? it.value : it);
}
function asArray(v) { return v === undefined ? [] : (Array.isArray(v) ? v : [v]); }

// NOTE: parseMIValue تعمل على كامل السطر بفهرس مطلق — نمرر السطر والفهرس الحقيقي:
function parseResults(str) {
	const results = {};
	let i = 0;
	while (str[i] === ',') {
		i++;
		const nm = /^[A-Za-z_][\w-]*=/.exec(str.slice(i));
		if (!nm) break;
		const name = nm[0].slice(0, -1); i += nm[0].length;
		let v; [v, i] = parseMIValue(str, i);
		if (results[name] === undefined) results[name] = v;
		else if (Array.isArray(results[name])) results[name].push(v);
		else results[name] = [results[name], v];
	}
	return results;
}

/* سجل تشخيصي للجلسات (لفهم أخطاء F5 لاحقًا) — ملف واحد محدود الحجم، لا يؤثر على الجلسة أبدًا */
function sessionLog(line) {
	try {
		const f = '/tmp/dhad-debug.log';
		let st = null;
		try { st = fs.statSync(f); } catch (e) { /* جديد */ }
		if (st && st.size > 131072) fs.writeFileSync(f, '');
		fs.appendFileSync(f, new Date().toISOString() + ' ' + line + '\n');
	} catch (e) { /* تجاوز */ }
}

// هل يستخدم المصدر إدخالًا تفاعليًا؟ (بعد تجريد التعليقات والنصوص — تفادي الإنذار الكاذب)
function usesInteractiveInput(srcText) {
	const t = srcText
		.replace(/\/\*[\s\S]*?\*\//g, ' ')
		.replace(/\/\/[^\n]*/g, ' ')
		.replace(/"([^"\\]|\\.)*"/g, '""');
	return /ادخل\s*\(/.test(t);
}

/* ── سائق gdb/MI ── */
class GdbMI {
	constructor(exe, onEvent, isRunning) {
		this.token = 0;
		this.pending = new Map();
		this.onEvent = onEvent;
		this.isRunning = isRunning || (() => false);
		this.buf = '';
		this.trace = !!process.env.DHAD_DEBUG_MI;
		this.proc = spawn('gdb', ['--interpreter=mi2', '-q', '--nx', exe], {
			env: { ...process.env, DEBUGINFOD_URLS: '' },
		});
		this.proc.stdout.on('data', d => this._feed(d.toString('utf8')));
		this.proc.stderr.on('data', () => {});
		this.proc.on('exit', () => this.onEvent({ type: 'proc-exit' }));
	}
	_feed(chunk) {
		if (this.trace) try { require('fs').appendFileSync('/tmp/dhad-mi.log', 'GDB<< ' + JSON.stringify(chunk) + '\n'); } catch (e) {}
		this.buf += chunk;
		let idx;
		while ((idx = this.buf.indexOf('\n')) >= 0) {
			const line = this.buf.slice(0, idx).replace(/\r$/, '');
			this.buf = this.buf.slice(idx + 1);
			if (!line) continue;
			// نتجاهل سطر الموجه فقط؛ الباقي يُحلل (parseResults بالفهرس المطلق)
			const tm = /^(\d+)?([\^*+=@&~]|\(gdb\))/.exec(line);
			if (!tm) {
				// سطر خام لا يطابق بروتوكول MI — أثناء تشغيل البرنامج يكون مخرجاته
				// المباشرة (gdb يمررها دون تغليف @"..." أحيانًا)، فنوجهها للstdout.
				if (process.env.DHAD_DEBUG_MI) try { require('fs').appendFileSync('/tmp/dhad-mi.log', 'RAW running=' + this.isRunning() + ' line=' + JSON.stringify(line) + '\n'); } catch (e) {}
				if (this.isRunning() && line) this.onEvent({ type: 'target', text: line });
				continue;
			}
			if (tm[2] === '(gdb)') continue;
			const hasToken = !!tm[1];
			const marker = tm[2];
			const rest = line.slice(tm[0].length);
			if (marker === '^') {
				const cm = /^[a-z-]+/.exec(rest) || [''];
				const entry = this.pending.get(hasToken ? parseInt(tm[1], 10) : 0);
				if (entry) {
					this.pending.delete(entry.token);
					clearTimeout(entry.timer);
					entry.resolve({ state: cm[0], results: parseResults(rest.slice(cm[0].length)) });
				}
			} else if (marker === '*' || marker === '+' || marker === '=') {
				const cm = /^[a-z-]+/.exec(rest) || [''];
				this.onEvent({ type: 'notify', marker, cls: cm[0], results: parseResults(rest.slice(cm[0].length)) });
			} else {
				const [v] = parseMIValue(rest, 0);
				this.onEvent({ type: marker === '@' ? 'target' : (marker === '~' ? 'console' : 'log'), text: v });
			}
		}
	}
	send(cmd, timeoutMs = 10000) {
		return new Promise((resolve, reject) => {
			const token = ++this.token;
			if (this.trace) try { require('fs').appendFileSync('/tmp/dhad-mi.log', 'SEND>> ' + JSON.stringify(`${token}${cmd}`) + '\n'); } catch (e) {}
			const timer = setTimeout(() => {
				this.pending.delete(token);
				reject(new Error('انتهت مهلة أمر gdb: ' + cmd.split(' ')[0]));
			}, timeoutMs);
			this.pending.set(token, { token, resolve, reject, timer });
			this.proc.stdin.write(`${token}${cmd}\n`, err => {
				if (err) { clearTimeout(timer); this.pending.delete(token); reject(err); }
			});
		}).then(res => {
			if (res.state === 'error') {
				const msg = (res.results && res.results.msg) || 'خطأ gdb غير معروف';
				throw new Error(String(msg));
			}
			return res.results || {};
		});
	}
	kill() { try { this.proc.kill('SIGKILL'); } catch (e) { /* ميت */ } }
}

// إيجاد المترجم صعودًا (نسخة بلا vscode — المحول يعمل في node خام)
function findCompiler(startDir) {
	let dir = startDir;
	for (let i = 0; i < 8; i++) {
		const c = path.join(dir, 'Compiler');
		try {
			if (fs.statSync(c).isDirectory()) {
				const b1 = path.join(c, 'build', 'bin', 'daad-compiler');
				const b2 = path.join(c, 'build-make', 'bin', 'daad-compiler');
				if (fs.existsSync(b1)) return { root: c, bin: b1 };
				if (fs.existsSync(b2)) return { root: c, bin: b2 };
				return { root: c, bin: 'daad-compiler' };
			}
		} catch (e) { /* تابع */ }
		const parent = path.dirname(dir);
		if (parent === dir) break;
		dir = parent;
	}
	return { root: null, bin: 'daad-compiler' };
}

function execFileP(file, args, opts) {
	return new Promise((resolve, reject) => {
		execFile(file, args, opts || {}, (err, stdout, stderr) => {
			if (err) reject(Object.assign(new Error(stderr || err.message), { stdout, stderr }));
			else resolve({ stdout: stdout || '', stderr: stderr || '' });
		});
	});
}

/* ── جلسة التصحيح ── */
class DhadDebugSession extends DebugSession {
	constructor() {
		super();
		this.setDebuggerLinesStartAt1(true);
		this.setDebuggerColumnsStartAt1(true);
		this._varHandles = new Handles();
		this._bp = new Map();       // srcPath -> Map(line -> gdbNum)
		this._pendingBp = new Map();// srcPath -> [lines] قبل الإطلاق
		this._gdb = null;
		this._running = false;
		this._exited = false;
		this._entered = false;
		this._exe = null;
		this._varSeq = 0;
	}

	/* ——— initialize ——— */
	initializeRequest(response, args) {
		response.body = response.body || {};
		response.body.supportsConfigurationDoneRequest = true;
		response.body.supportsEvaluateForHovers = true;
		response.body.supportsTerminateRequest = true;
		response.body.supportTerminateDebuggee = true;
		this.sendResponse(response);
		this.sendEvent(new InitializedEvent());
	}

	/* ——— launch: ترجمة .ض → C++‎ (#line) → -g -O0 → gdb ——— */
	async launchRequest(response, args) {
		const program = args.program;
		try {
			if (!program || !fs.existsSync(program)) {
				const cwd = (args && args.cwd) || process.cwd();
				throw new Error('ملف البرنامج غير موجود: ' + program + '\n' +
					'تأكد من: 1) فتح مجلد المشروع Dhad-Studio-Unified نفسه (وليس Documents)\n' +
					'2) فتح ملف .ض في المحرر قبل F5\n' +
					'3) اختيار الإعداد «تصحيح ض: الملف الحالي» من قائمة التشغيل أعلى الشريط الجانبي\n' +
					'(مجلد العمل الحالي للمحول: ' + cwd + ')');
			}
			sessionLog('launch program=' + program);
			// حارس الإدخال التفاعلي: القارئ في التصحيح كان سيلتهم قناة DAP فيعلق الجلسة بصمت.
			// نفشل بسرعة برسالة واضحة بدل التعليق الغامض (البديل: F9 للتشغيل الطرفي).
			let srcText = '';
			try { srcText = fs.readFileSync(program, 'utf8'); } catch (e) { /* سيكشفها المترجم */ }
			if (srcText && usesInteractiveInput(srcText)) {
				sessionLog('launch REJECTED (interactive ادخل): ' + program);
				throw new Error('البرنامج يستخدم «ادخل» (إدخال تفاعلي) وهو غير مدعوم في جلسة التصحيح — سيعلق انتظار الإدخال.\nشغّله بـ F9 (ض: ترجمة وتشغيل) للإدخال من الطرفية.');
			}
			const dir = path.dirname(program);
			const base = path.basename(program, path.extname(program)).replace(/[^\p{L}\p{N}_-]+/gu, '_');
			const { root, bin } = findCompiler(dir);
			const tmpDir = fs.mkdtempSync(path.join(os.tmpdir(), 'dhad-dbg-'));
			const cpp = path.join(tmpDir, base + '.cpp');
			const exe = path.join(tmpDir, base + '.out');
			this.sendEvent(new OutputEvent(`ترجمة ${program} ...\n`, 'console'));
			try {
				await execFileP(bin, [program, '-g', '-o', cpp]);
			} catch (e) {
				throw new Error('فشلت ترجمة ض:\n' + (e.stderr || e.message));
			}
			const gxx = ['-std=c++20', '-g', '-O0', cpp, '-o', exe];
			if (root) gxx.push('-I', root, '-I', path.join(root, 'include'));
			try {
				await execFileP('g++', gxx);
			} catch (e) {
				throw new Error('فشل بناء g++:\n' + (e.stderr || e.message));
			}
			this._exe = exe;
			this._cppFile = cpp;
			sessionLog('build ok exe=' + exe);
			// خريطة .ض→cpp من توجيهات #line: gdb الكسول لا يحل أسماء .ض العربية
			// قبل القراءة الكاملة للرموز، فنضع النقاط على سطور cpp المكافئة مباشرة
			this._lineMap = new Map();
			try {
				const cppText = fs.readFileSync(cpp, 'utf8').split('\n');
				const base = program.replace(/\\/g, '/');
				cppText.forEach((ln, i) => {
					const m = /^#line\s+(\d+)\s+"(.*)"\s*$/.exec(ln.trim());
					if (m && m[2].replace(/\\/g, '/') === base && !this._lineMap.has(parseInt(m[1], 10))) {
						this._lineMap.set(parseInt(m[1], 10), i + 2); // السطر التالي للتوجيه
					}
				});
				sessionLog('linemap entries=' + this._lineMap.size);
			} catch (e) { /* بلا خريطة — رجوع للمسار القديم */ }
			this._stopOnEntry = !!args.stopOnEntry;
			this._gdb = new GdbMI(exe, ev => this._onGdbEvent(ev), () => this._running);
			// الرموز تُحمَّل لاتزامنيًا بعد spawn — بدون هذا ترفض gdb النقطة بـ No source file
			try { await this._gdb.send('-gdb-set breakpoint pending on'); } catch (e) { /* تجاوز */ }
			// طبّق نقاط التوقف المعلقة + أخبر العميل بتحققها (كانت ستظهر رمادية)
			for (const [src, lines] of this._pendingBp) {
				const applied = await this._applyBreakpoints(src, lines);
				const srcObj = new Source(path.basename(src), src);
				for (const b of applied) {
					this.sendEvent(new BreakpointEvent('changed', { id: b.id || 0, verified: b.verified, line: b.line, source: srcObj }));
				}
			}
			this._pendingBp.clear();
			if (args.stopOnEntry) {
				try { await this._gdb.send('-break-insert -t main'); } catch (e) { /* تجاوز */ }
			}
			if (args.cwd) {
				try { await this._gdb.send(`-environment-cd "${args.cwd.replace(/\\/g, '/')}"`); } catch (e) { /* تجاوز */ }
			}
			this.sendResponse(response);
		} catch (e) {
			sessionLog('launch FAILED: ' + String(e.message || e).slice(0, 200));
			this.sendErrorResponse(response, 1, e.message);
		}
	}

	async _applyBreakpoints(src, lines) {
		let cur = this._bp.get(src);
		if (!cur) { cur = new Map(); this._bp.set(src, cur); }
		for (const [ln, num] of [...cur]) {
			if (!lines.includes(ln)) {
				try { await this._gdb.send(`-break-delete ${num}`); } catch (e) { /* تجاوز */ }
				cur.delete(ln);
			}
		}
		const out = [];
		for (const ln of lines) {
			if (cur.has(ln)) { out.push({ verified: true, line: ln, id: cur.get(ln) }); continue; }
			// النقطة على مسار .ض مباشرة مع -f: توجيهات #line تنسب الكود للمصدر،
			// وسطور cpp نفسها لا تحمل كودًا حسب gdb — والخريطة احتياط تشخيصي
			const loc = `"${src}:${ln}"`;
			try {
				// -f: إنشاء معلق يتحقق عند اكتمال قراءة الرموز (gdb كسول مع مسارات .ض)
				const r = await this._gdb.send(`-break-insert -f ${loc}`);
				const num = parseInt(r.bkpt && r.bkpt.number, 10);
				if (Number.isFinite(num)) {
					cur.set(ln, num);
					out.push({ verified: true, line: ln, id: num });
				} else out.push({ verified: false, line: ln });
			} catch (e) {
				out.push({ verified: false, line: ln });
			}
		}
		return out;
	}

	/* ——— setBreakpoints ——— */
	async setBreakPointsRequest(response, args) {
		const src = args.source && args.source.path;
		const lines = (args.breakpoints || []).map(b => b.line);
		if (!src) { response.body = { breakpoints: [] }; this.sendResponse(response); return; }
		if (!this._gdb) {
			this._pendingBp.set(src, lines);
			response.body = { breakpoints: lines.map(line => ({ verified: false, line })) };
			this.sendResponse(response);
			return;
		}
		const bps = await this._applyBreakpoints(src, lines);
		response.body = { breakpoints: bps };
		this.sendResponse(response);
	}

	/* ——— configurationDone → ابدأ التشغيل ——— */
	async configurationDoneRequest(response, args) {
		this.sendResponse(response);
		if (this._gdb && !this._running && !this._exited) {
			try {
				await this._gdb.send('-exec-run');
				this._running = true;
			} catch (e) {
				this.sendEvent(new OutputEvent('تعذر التشغيل: ' + e.message + '\n', 'stderr'));
				this._terminate();
			}
		}
	}

	_onGdbEvent(ev) {
		if (ev.type === 'target') {
			this.sendEvent(new OutputEvent(ev.text, 'stdout'));
			return;
		}
		if (ev.type === 'console' || ev.type === 'log') return; // ضجيج gdb
		if (ev.type === 'proc-exit') {
			if (!this._exited) { this._exited = true; this.sendEvent(new TerminatedEvent()); }
			return;
		}
		if (ev.type !== 'notify') return;
		if (ev.cls === 'stopped') {
			this._running = false;
			const r = ev.results || {};
			const reason = r.reason || '';
			const tid = parseInt((r['thread-id'] !== undefined ? r['thread-id'] : '1'), 10) || 1;
			if (reason === 'breakpoint-hit') {
				let how = 'breakpoint';
				if (!this._entered && this._stopOnEntry) how = 'entry';
				this._entered = true;
				this.sendEvent(new StoppedEvent(how, tid));
			} else if (reason === 'end-stepping-range' || reason === 'function-finished') {
				this.sendEvent(new StoppedEvent('step', tid));
			} else if (reason === 'signal-received') {
				this.sendEvent(new StoppedEvent('pause', tid, String(r['signal-name'] || 'إشارة')));
			} else if (reason === 'exited-normally' || reason === 'exited') {
				this.sendEvent(new OutputEvent(`\nانتهى البرنامج (خروج طبيعي)\n`, 'console'));
				this._terminate();
			} else if (reason === 'exited-signalled') {
				this.sendEvent(new OutputEvent(`\nانتهى البرنامج بإشارة: ${r['signal-name'] || ''}\n`, 'stderr'));
				this._terminate();
			} else {
				this.sendEvent(new StoppedEvent('breakpoint', tid));
			}
		}
	}

	_terminate() {
		if (this._exited) return;
		this._exited = true;
		sessionLog('terminated exe=' + (this._exe || '?'));
		if (this._gdb) this._gdb.kill();
		this.sendEvent(new TerminatedEvent());
	}

	/* ——— threads / stack / scopes / variables ——— */
	async threadsRequest(response) {
		try {
			const r = await this._gdb.send('-thread-info');
			const list = miList(r.threads, null).length ? (Array.isArray(r.threads) ? r.threads : [r.threads]) : [];
			const flat = [];
			(function collect(v) {
				if (Array.isArray(v)) v.forEach(collect);
				else if (v && typeof v === 'object') {
					if (v.__pair) collect(v.value);
					else if (v.id !== undefined) flat.push(v);
				}
			})(r.threads);
			response.body = { threads: flat.map(t => new Thread(parseInt(t.id, 10), `خيط ${t.id}${t.name ? ' (' + t.name + ')' : ''}`)) };
			if (!response.body.threads.length) response.body = { threads: [new Thread(1, 'خيط 1')] };
		} catch (e) {
			response.body = { threads: [new Thread(1, 'خيط 1')] };
		}
		this.sendResponse(response);
	}

	_frameId(tid, level) { return tid * 100000 + level; }
	_decodeFrame(id) { return { tid: Math.floor(id / 100000), level: id % 100000 }; }

	async stackTraceRequest(response, args) {
		try {
			const tid = args.threadId; // معرف خيط gdb الخام (ليس frameId مرمزًا)
			const r = await this._gdb.send(`-stack-list-frames --thread ${tid}`);
			let frames = miList(r.stack, 'frame');
			const start = args.startFrame || 0;
			const count = args.levels || frames.length;
			frames = frames.slice(start, start + count).map((f, i) => {
				const level = start + i;
				const name = String((f.func || '??') + (f.addr ? ` @${f.addr}` : ''));
				let src = undefined;
				if (f.file) src = new Source(path.basename(String(f.file)), String(f.fullname || f.file));
				return new StackFrame(this._frameId(tid, level), name, src, f.line ? parseInt(f.line, 10) : 0, 0);
			});
			response.body = { stackFrames: frames, totalFrames: miList(r.stack, 'frame').length };
		} catch (e) {
			response.body = { stackFrames: [], totalFrames: 0 };
		}
		this.sendResponse(response);
	}

	scopesRequest(response, args) {
		const { tid, level } = this._decodeFrame(args.frameId);
		const locRef = this._varHandles.create({ tid, level, kind: 'locals' });
		const argRef = this._varHandles.create({ tid, level, kind: 'args' });
		response.body = {
			scopes: [
				{ name: 'المتغيرات المحلية', variablesReference: locRef, expensive: false },
				{ name: 'الوسائط', variablesReference: argRef, expensive: false },
			],
		};
		this.sendResponse(response);
	}

	async variablesRequest(response, args) {
		try {
			const info = this._varHandles.get(args.variablesReference);
			let vars = [];
			if (info && (info.kind === 'locals')) {
				const r = await this._gdb.send(`-stack-list-variables --thread ${info.tid} --frame ${info.level} --all-values`);
				const list = miList(r.variables, null);
				const flat = [];
				(function collect(v) {
					if (Array.isArray(v)) v.forEach(collect);
					else if (v && typeof v === 'object' && !v.__pair && v.name !== undefined) flat.push(v);
				})(list);
				for (const v of flat) vars.push(await this._toVariable(info, String(v.name), v));
			} else if (info && info.kind === 'args') {
				const r = await this._gdb.send(`-stack-list-arguments 1 --thread ${info.tid} --frame ${info.level}`);
				let frames = miList(r['stack-args'], 'frame');
				const f0 = frames[0] || {};
				const arr = miList(f0.args, null);
				const flat = [];
				(function collect(v) {
					if (Array.isArray(v)) v.forEach(collect);
					else if (v && typeof v === 'object' && !v.__pair && v.name !== undefined) flat.push(v);
				})(arr);
				for (const v of flat) vars.push(await this._toVariable(info, String(v.name), v));
			} else if (info && info.kind === 'children') {
				const r = await this._gdb.send(`-var-list-children --all-values "${info.varName}"`);
				const kids = miList(r.children, 'child');
				for (const c of kids) {
					const num = parseInt(c.numchild || '0', 10) || 0;
					let ref = 0;
					if (num > 0) ref = this._varHandles.create({ tid: info.tid, level: info.level, kind: 'children', varName: String(c.name) });
					vars.push(new Variable(String(c.exp || c.name), String(c.value !== undefined ? c.value : ''), ref, num, { kind: 'property' }));
				}
			}
			response.body = { variables: vars };
		} catch (e) {
			response.body = { variables: [] };
		}
		this.sendResponse(response);
	}

	async _toVariable(info, expr, v) {
		const val = String(v.value !== undefined ? v.value : '');
		const type = String(v.type || '');
		if (val.startsWith('{')) {
			this._varSeq++;
			const varName = `dhad_v${this._varSeq}`;
			try {
				await this._gdb.send(`-var-create --thread ${info.tid} --frame ${info.level} ${varName} * "${expr.replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`);
				const ref = this._varHandles.create({ tid: info.tid, level: info.level, kind: 'children', varName });
				return new Variable(expr, type || '{...}', ref, 1, { kind: 'variable' });
			} catch (e) { /* سقط للعرض الخام */ }
		}
		return new Variable(expr, val, 0, 0, { kind: 'variable' });
	}

	/* ——— التنقل ——— */
	async _exec(cmd, response, willRun = true) {
		try {
			await this._gdb.send(cmd);
			if (willRun) this._running = true;
			this.sendResponse(response);
		} catch (e) {
			this.sendErrorResponse(response, 2, e.message);
		}
	}
	continueRequest(response) { this._exec('-exec-continue', response); }
	nextRequest(response) { this._exec('-exec-next', response); }
	stepInRequest(response) { this._exec('-exec-step', response); }
	stepOutRequest(response) { this._exec('-exec-finish', response); }
	async pauseRequest(response) {
		try { await this._gdb.send('-exec-interrupt'); this.sendResponse(response); }
		catch (e) { this.sendErrorResponse(response, 2, e.message); }
	}

	/* ——— تقييم تعبير (مراقبة/تحويم) ——— */
	async evaluateRequest(response, args) {
		try {
			let cmd = `-data-evaluate-expression "${String(args.expression).replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`;
			if (args.frameId !== undefined) {
				const { tid, level } = this._decodeFrame(args.frameId);
				cmd = `-data-evaluate-expression --thread ${tid} --frame ${level} "${String(args.expression).replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`;
			}
			const r = await this._gdb.send(cmd);
			response.body = { result: String(r.value !== undefined ? r.value : ''), variablesReference: 0 };
			this.sendResponse(response);
		} catch (e) {
			this.sendErrorResponse(response, 3, e.message);
		}
	}

	/* ——— إنهاء ——— */
	async disconnectRequest(response, args) {
		try { if (this._gdb && !this._exited) await this._gdb.send('-gdb-exit', 2000); } catch (e) { /* تجاوز */ }
		this._terminate();
		this.sendResponse(response);
	}
	async terminateRequest(response) {
		try { if (this._gdb && !this._exited) await this._gdb.send('-gdb-exit', 2000); } catch (e) { /* تجاوز */ }
		this._terminate();
		this.sendResponse(response);
	}
}

DebugSession.run(DhadDebugSession);
