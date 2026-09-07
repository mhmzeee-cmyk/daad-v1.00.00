#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int مجموع(int أ, int ب) {
return (أ + ب);
}
int ضرب(int أ, int ب) {
return (أ * ب);
}
int مربع(int س) {
return (س * س);
}
int مكعب(int س) {
return ((س * س) * س);
}
daad::runtime::daad_print("═══ اختبار الدوال الأساسية ═══");
int ن1 = مجموع(10, 20);
daad::runtime::daad_print(ن1);
int ن2 = ضرب(6, 7);
daad::runtime::daad_print(ن2);
int ن3 = مربع(8);
daad::runtime::daad_print(ن3);
int ن4 = مكعب(3);
daad::runtime::daad_print(ن4);
int مضروب(int ن) {
if ((ن <= 1)) {
return 1;
}
return (ن * مضروب((ن - 1)));
}
daad::runtime::daad_print("═══ اختبار التكرار العودي ═══");
int ف5 = مضروب(5);
daad::runtime::daad_print(ف5);
int فيبوناتشي(int ن) {
if ((ن <= 0)) {
return 0;
}
if ((ن == 1)) {
return 1;
}
return (فيبوناتشي((ن - 1)) + فيبوناتشي((ن - 2)));
}
daad::runtime::daad_print("═══ اختبار فيبوناتشي ═══");
int فب1 = فيبوناتشي(10);
daad::runtime::daad_print(فب1);
bool أولي(int ن) {
if ((ن < 2)) {
return false;
}
int ا = 2;
while (((ا * ا) <= ن)) {
if (((ن % ا) == 0)) {
return false;
}
ا += 1;
}
return true;
}
daad::runtime::daad_print("═══ اختبار الأعداد الأولية ═══");
bool هل23أولي = أولي(23);
daad::runtime::daad_print(هل23أولي);
bool هل24أولي = أولي(24);
daad::runtime::daad_print(هل24أولي);
int أكبر_عامل(int ن) {
int largest = 1;
int ا = 2;
while (((ا * ا) <= ن)) {
while (((ن % ا) == 0)) {
largest = ا;
ن = (ن / ا);
}
ا += 1;
}
if ((ن > 1)) {
largest = ن;
}
return largest;
}
daad::runtime::daad_print("═══ اختبار أكبر عامل ═══");
int عامل100 = أكبر_عامل(100);
daad::runtime::daad_print(عامل100);
int عامل1319 = أكبر_عامل(1319);
daad::runtime::daad_print(عامل1319);
daad::runtime::daad_print("═══ اكتمل اختبار 2 ═══");
