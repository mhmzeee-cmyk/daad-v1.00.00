#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int مضروب(int ن) {
if ((ن <= 1)) {
return 1;
}
return (ن * مضروب((ن - 1)));
}
int مربع(int س) {
return (س * س);
}
int مجموع(int أ, int ب) {
return (أ + ب);
}
daad::runtime::daad_print("═══ اختبار BUG-01: طباعة مع دوال ═══");
daad::runtime::daad_print(مضروب(5));
daad::runtime::daad_print(مضروب(10));
daad::runtime::daad_print(مربع(7));
daad::runtime::daad_print(مجموع(100, 200));
daad::runtime::daad_print(مربع(مجموع(2, 3)));
daad::runtime::daad_print(مضروب(مربع(3)));
int س = 10;
daad::runtime::daad_print((س + 5));
daad::runtime::daad_print((س * 2));
daad::runtime::daad_print(std::pow(س, 2));
daad::runtime::daad_print((س > 5));
daad::runtime::daad_print("═══ اختبار المنطق العربي ═══");
int أ = 10;
int ب = 20;
if (((أ > 5) && (ب > 15))) {
daad::runtime::daad_print("كلا الشرطين صواب");
}
if (((أ > 15) || (ب > 15))) {
daad::runtime::daad_print("على الأقل واحد صواب");
}
if (((أ > 15) && (ب > 15))) {
daad::runtime::daad_print("هذا لن يظهر");
}
 else {
daad::runtime::daad_print("صحيح — ليس كلاهما أكبر");
}
bool شرط1 = ((أ > 5) && (ب > 15));
daad::runtime::daad_print(شرط1);
bool شرط2 = ((أ > 15) || (ب > 15));
daad::runtime::daad_print(شرط2);
bool شرط3 = ((أ > 15) && (ب > 15));
daad::runtime::daad_print(شرط3);
daad::runtime::daad_print("═══ اختبار زد/انقص ═══");
int عداد = 0;
عداد += 1;
عداد += 1;
عداد += 1;
daad::runtime::daad_print(عداد);
عداد -= 1;
daad::runtime::daad_print(عداد);
daad::runtime::daad_print("═══ اختبار ^= ═══");
int ق = 2;
ق = std::pow(ق, 3);
daad::runtime::daad_print(ق);
ق = std::pow(ق, 2);
daad::runtime::daad_print(ق);
daad::runtime::daad_print("═══ اختبار بينما + انقص ═══");
int countdown = 5;
while ((countdown > 0)) {
daad::runtime::daad_print(countdown);
countdown -= 1;
}
daad::runtime::daad_print("انطلاق!");
daad::runtime::daad_print("═══ اختبار لكل + زد ═══");
for (i = 0; (i < 5); i + 1;
1) {
daad::runtime::daad_print((i * i));
}
daad::runtime::daad_print("═══ اختبار مقارنات مع منطق عربي ═══");
int درجة = 85;
if (((درجة >= 90) || (درجة >= 80))) {
daad::runtime::daad_print("ممتاز أو جيد جداً");
}
if (((درجة >= 90) && (درجة <= 100))) {
daad::runtime::daad_print("نطاق سليم");
}
daad::runtime::daad_print("═══ اكتمل اختبار طباعة والمنطق العربي ═══");
