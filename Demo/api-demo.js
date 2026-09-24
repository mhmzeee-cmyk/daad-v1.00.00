// api-demo.js — إثبات المنصة ضد خادم يعمل (يُشغَّل الخادم خارجيًا)
// الاستخدام: node api-demo.js <port> [email]
// يسجّل طالبًا (يتجاوز إن كان مسجلًا)، يدخل، يعرض التحديات والملف، ويطبع بيانات الدخول للمقدّم.
const http = require("http");

const PORT = process.argv[2] || 3000;
const EMAIL = process.argv[3] || "demo@example.com";
const PASSWORD = "Test1234!";

function req(method, urlPath, body, token) {
  return new Promise((resolve, reject) => {
    const data = body ? JSON.stringify(body) : null;
    const o = {
      hostname: "127.0.0.1", port: Number(PORT), path: urlPath, method,
      headers: { "Content-Type": "application/json" },
    };
    if (token) o.headers["Authorization"] = "Bearer " + token;
    const r = http.request(o, (res) => {
      let chunks = "";
      res.on("data", (c) => (chunks += c));
      res.on("end", () => { try { resolve({ status: res.statusCode, body: JSON.parse(chunks) }); } catch (e) { resolve({ status: res.statusCode, body: {} }); } });
    });
    r.on("error", reject);
    r.setTimeout(10000, () => { r.destroy(); reject(new Error("timeout")); });
    if (data) r.write(data);
    r.end();
  });
}
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

(async () => {
  try {
    await req("GET", "/health");
    let reg;
    try { reg = await req("POST", "/api/v1/auth/register", { name: "طالب العرض", email: EMAIL, password: PASSWORD, role: "STUDENT" }); } catch (e) { reg = {}; }
    if (reg.body && reg.body.user) console.log("تم إنشاء الحساب: " + reg.body.user.name);
    else console.log("الحساب موجود مسبقًا — ندخل به مباشرة");
    const login = await req("POST", "/api/v1/auth/login", { username: EMAIL, password: PASSWORD });
    if (!login.body || !login.body.accessToken) { console.log("❌ تعذر الدخول"); process.exit(1); }
    console.log("تم الدخول بنجاح ✅");
    const ch = await req("GET", "/api/v1/challenges", null, login.body.accessToken);
    const list = (ch.body && ch.body.challenges) || [];
    console.log(list.length + " تحديًا — أولها: " + ((list[0] && list[0].title) || "؟"));
    console.log("--- بيانات الدخول للعرض الحي في المتصفح ---");
    console.log("البريد: " + EMAIL);
    console.log("كلمة المرور: " + PASSWORD);
    process.exit(0);
  } catch (e) {
    console.log("❌ تعذر الاتصال بالخادم: " + e.message);
    process.exit(1);
  }
})();
