#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using daad::stdlib::طول_مصفوفة;
using daad::stdlib::أضف;
using daad::stdlib::احذف_من;
using daad::stdlib::عكس_مصفوفة;
using daad::stdlib::ابحث_في;
using daad::stdlib::جذر;
using daad::stdlib::مضروب;
using daad::stdlib::اقرأ_stdin;
using daad::stdlib::طول;
using daad::stdlib::استخرج;
using daad::stdlib::ابحث;
using daad::stdlib::استبدل;
using daad::stdlib::صغير;
using daad::stdlib::كبير;
using daad::stdlib::نظف;
using daad::stdlib::يبدأ_بـ;
using daad::stdlib::ينتهي_بـ;
using daad::stdlib::قسّم;
using daad::stdlib::قوة;
using daad::stdlib::مطلق;
using daad::stdlib::أقصى;
using daad::stdlib::أدناه;
using daad::stdlib::عشوائي;
using daad::stdlib::حجم_المصفوفة;
using daad::stdlib::أضف_لمصفوفة;
using daad::stdlib::احصل_من_مصفوفة;
using daad::stdlib::عّين_في_مصفوفة;
using daad::stdlib::احذف_من_مصفوفة;
using daad::stdlib::قلب_المصفوفة;
using daad::stdlib::فرّغ_المصفوفة;
using daad::stdlib::انسخ_مصفوفة;
using daad::stdlib::ادمج_مصفوفتين;
using daad::stdlib::اكتب_stdout;
using daad::stdlib::اكتب_stderr;
int فيبوناتشي(int ن) {
if ((ن <= 0)) {
return 0;
}
if ((ن == 1)) {
return 1;
}
int أ = 0;
int ب = 1;
int مؤقت = 0;
for (int i = 2; (i <= ن); i = (i + 1)) {
مؤقت = (أ + ب);
أ = ب;
ب = مؤقت;
}
return ب;
}
int main() {
daad::runtime::daad_print("أول 15 عدد فيبوناتشي:");
for (int i = 0; (i < 15); i = (i + 1)) {
daad::runtime::daad_print(فيبوناتشي(i));
}
int مجموع = 0;
for (int i = 0; (i < 10); i = (i + 1)) {
مجموع += فيبوناتشي(i);
}
daad::runtime::daad_print("مجموع أول 10 أعداد فيبوناتشي:");
daad::runtime::daad_print(مجموع);
return 0;
}
