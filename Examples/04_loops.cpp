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
int main() {
daad::runtime::daad_print("الأعداد من 1 إلى 10:");
for (int i = 1; (i <= 10); i = (i + 1)) {
daad::runtime::daad_print(i);
}
daad::runtime::daad_print("جدول ضرب 7:");
for (int i = 1; (i <= 10); i = (i + 1)) {
daad::runtime::daad_print("7 × ");
daad::runtime::daad_print(i);
daad::runtime::daad_print(" = ");
daad::runtime::daad_print((7 * i));
}
int ع = 1;
int مجموع = 0;
while ((ع <= 100)) {
مجموع = (مجموع + ع);
ع = (ع + 1);
}
daad::runtime::daad_print("مجموع الأعداد من 1 إلى 100:");
daad::runtime::daad_print(مجموع);
return 0;
}
