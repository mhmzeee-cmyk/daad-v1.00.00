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
int مضروب(int ن) {
int نتيجة = 1;
for (int i = 1; (i <= ن); i = (i + 1)) {
نتيجة *= i;
}
return نتيجة;
}
int مجموع(int ن) {
int م = 0;
for (int i = 1; (i <= ن); i = (i + 1)) {
م += i;
}
return م;
}
int قوة(int أس, int أصل) {
int نتيجة = 1;
for (int i = 0; (i < أس); i = (i + 1)) {
نتيجة *= أصل;
}
return نتيجة;
}
int main() {
daad::runtime::daad_print("مضروب 5 = ");
daad::runtime::daad_print(مضروب(5));
daad::runtime::daad_print("المجموع من 1 إلى 10:");
daad::runtime::daad_print(مجموع(10));
daad::runtime::daad_print("2 أس 10 = ");
daad::runtime::daad_print(قوة(10, 2));
return 0;
}
