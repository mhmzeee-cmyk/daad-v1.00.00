#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

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
int قوة(int أس, int أصل) {
int نتيجة = 1;
for (int i = 0; (i < أس); i = (i + 1)) {
نتيجة *= أصل;
}
return نتيجة;
}
int مضروب(int ن) {
int نتيجة = 1;
for (int i = 1; (i <= ن); i = (i + 1)) {
نتيجة *= i;
}
return نتيجة;
}
int أكبر(int أ, int ب) {
if ((أ > ب)) {
return أ;
}
return ب;
}
int أصغر(int أ, int ب) {
if ((أ < ب)) {
return أ;
}
return ب;
}
int القيمة_المطلقة(int ن) {
if ((ن < 0)) {
return (0 - ن);
}
return ن;
}
daad::runtime::daad_print("=== آلة حاسبة متقدمة ===");
daad::runtime::daad_print("5 + 3 = ");
جمع(5, 3);
daad::runtime::daad_print(3);
daad::runtime::daad_print("10 - 4 = ");
طرح(10, 4);
daad::runtime::daad_print(4);
daad::runtime::daad_print("6 × 7 = ");
ضرب(6, 7);
daad::runtime::daad_print(7);
daad::runtime::daad_print("20 ÷ 3 = ");
قسمة(20, 3);
daad::runtime::daad_print(3);
daad::runtime::daad_print("20 % 3 = ");
باقي_القسمة(20, 3);
daad::runtime::daad_print(3);
daad::runtime::daad_print("2 أس 10 = ");
قوة(10, 2);
daad::runtime::daad_print(2);
daad::runtime::daad_print("5! = ");
مضروب(5);
daad::runtime::daad_print(5);
daad::runtime::daad_print("الأكبر بين 15 و 20 = ");
أكبر(15, 20);
daad::runtime::daad_print(20);
daad::runtime::daad_print("القيمة المطلقة لـ -42 = ");
القيمة_المطلقة((-42));
daad::runtime::daad_print((-42));
