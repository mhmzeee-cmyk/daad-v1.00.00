#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

void رنين_عادي() {
daad::runtime::daad_print("بيب... بيب... بيب...");
}
void رنين_سريع() {
daad::runtime::daad_print("بيب بيب بيب بيب بيب!");
}
void رنين_هادئ() {
daad::runtime::daad_print("همس... همس... همس...");
}
void رنين_مفاجئ() {
daad::runtime::daad_print("وام! وام! وام!");
}
std::string اختيار_الرنين(int اختيار) {
if ((اختيار == 1)) {
return "عادي";
}
if ((اختيار == 2)) {
return "سريع";
}
if ((اختيار == 3)) {
return "هادئ";
}
if ((اختيار == 4)) {
return "مفاجئ";
}
return "عادي";
}
void تشغيل_المنبه(int ساعة, int دقيقة, int نوع) {
daad::runtime::daad_print("=== المنبه ===");
daad::runtime::daad_print("الوقت: ");
daad::runtime::daad_print(ساعة);
daad::runtime::daad_print(":");
if ((دقيقة < 10)) {
daad::runtime::daad_print("0");
}
daad::runtime::daad_print(دقيقة);
daad::runtime::daad_print("الرنين: ");
اختيار_الرنين(نوع);
daad::runtime::daad_print(نوع);
if ((نوع == 1)) {
رنين_عادي();
(نوع == 1);
}
if ((نوع == 2)) {
رنين_سريع();
(نوع == 2);
}
if ((نوع == 3)) {
رنين_هادئ();
(نوع == 3);
}
if ((نوع == 4)) {
رنين_مفاجئ();
(نوع == 4);
}
}
std::string تحديد_الحالة(int ساعة) {
if ((ساعة >= 5)) {
if ((ساعة < 12)) {
return "صباحاً";
}
}
if ((ساعة >= 12)) {
if ((ساعة < 17)) {
return "ظهراً";
}
}
if ((ساعة >= 17)) {
if ((ساعة < 21)) {
return "مساءً";
}
}
return "ليلاً";
}
تشغيل_المنبه(7, 30, 1);
1;
daad::runtime::daad_print("الحالة: ");
تحديد_الحالة(7);
daad::runtime::daad_print(7);
تشغيل_المنبه(14, 0, 2);
2;
daad::runtime::daad_print("الحالة: ");
تحديد_الحالة(14);
daad::runtime::daad_print(14);
تشغيل_المنبه(22, 45, 3);
3;
daad::runtime::daad_print("الحالة: ");
تحديد_الحالة(22);
daad::runtime::daad_print(22);
daad::runtime::daad_print("=== عد تنازلي ===");
for (int i = 5; (i >= 1); i = (i - 1)) {
daad::runtime::daad_print(i);
}
daad::runtime::daad_print("وقت!");
