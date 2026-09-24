# BUILD_PIPELINE.md — من المصدر إلى الثنائي (المنفذ فعلًا، لا المخطط)
> `./dhad` هو المنسق الوحيد. لا CLI جديد ولا مترجم جديد — طبقة فوق الأدوات الحالية.

## Dialects (الحقيقة من المستودع — لا `.ضب` موجودة)
| اللهجة | الامتداد | المسار | اللغة الرئيسية؟ |
|---|---|---|---|
| ض عالية المستوى | `.ض` | Track A → C++20 | **نعم — المنتج الأساسي** |
| ض منخفضة المستوى (DAAD) | `.daad` | Track B → DAAD IR | لغة ثانية مدعومة (موثقة في LANGUAGE_CONTRACT) |
| تجميع DHAD | `.ضasm` | assembler → `.bin` → CPU | صيغة داخلية/تعليمية — ليست للمستخدم النهائي |
| `.ضب` | — | **غير موجودة في المستودع** | فكرة تسمية مستقبلية فقط — لا تستخدمها |

## Pipeline (كل سهم يعمل هنا)
```text
program.ض ──► Frontend A ──► C++ emission stage ──┬─► linux-x64-cpp ──► g++ ──► ELF ──► run ✓
                                                  └─► windows-x64   ──► MinGW ──► EXE (PE verified)

program.daad ──► Frontend B ──► DAAD IR ──┬─► linux-x64 ──► BACKEND_X86 ──► gcc ──► ELF ──► run ✓
                                          └─► dhad-cpu  ──► BACKEND_DHAD ──► asm ──► .bin ──► run ✓
```

## Resolver rules (كما ينفذها `./dhad`)
- الحالة تُقرأ من `targets.json` (`AVAILABLE` + `proof`) — أي `NOT_IMPLEMENTED` تُرفض بخروج 3.
- عدم تطابق اللهجة/الهدف يُرفض بسبب واضح (مثال: `.daad`+windows-x64 لأن x86 خاص بلينكس).
- لا مخرجات وهمية: أي فشل في أي مرحلة → خروج غير صفري + لا ملف ناتج (يُحذف المؤقت؛ لا 0-byte binaries).

## Implemented / Verified / Blocked / Not Implemented
- Implemented+Verified: الأسطر الأربع ذات ✓ أعلاه (25+ حالة في `Tests/real_regression.sh` + `dhad` نفسه).
- Implemented+Blocked: `windows-x64` تنفيذ (boot على Windows الحقيقي معلق).
- Not Implemented: macOS/Android/iOS/ARM/RISC-V/GUI (لا toolchain هنا — تصميم فقط).
