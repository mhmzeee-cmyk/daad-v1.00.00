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
int جمع(int أ, int ب) {
return (أ + ب);
}
int طرح(int أ, int ب) {
return (أ - ب);
}
int ضرب(int أ, int ب) {
return (أ * ب);
}
int قسمة(int أ, int ب) {
if ((ب == 0)) {
return 0;
}
return (أ / ب);
}
int باقي_القسمة(int أ, int ب) {
if ((ب == 0)) {
return 0;
}
return (أ % ب);
}
int main() {
int س1 = 20;
int س2 = 6;
daad::runtime::daad_print("المجموع: ");
daad::runtime::daad_print(جمع(س1, س2));
daad::runtime::daad_print("الفرق: ");
daad::runtime::daad_print(طرح(س1, س2));
daad::runtime::daad_print("الحاصل: ");
daad::runtime::daad_print(ضرب(س1, س2));
daad::runtime::daad_print("الناتج: ");
daad::runtime::daad_print(قسمة(س1, س2));
daad::runtime::daad_print("الباقي: ");
daad::runtime::daad_print(باقي_القسمة(س1, س2));
return 0;
}
