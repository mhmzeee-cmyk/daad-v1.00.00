#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

std::string تحديد_اليوم(int يوم) {
if ((يوم == 1)) {
return "الاثنين";
}
if ((يوم == 2)) {
return "الثلاثاء";
}
if ((يوم == 3)) {
return "الأربعاء";
}
if ((يوم == 4)) {
return "الخميس";
}
if ((يوم == 5)) {
return "الجمعة";
}
if ((يوم == 6)) {
return "السبت";
}
if ((يوم == 7)) {
return "الأحد";
}
return "غير معروف";
}
std::string تحديد_الشهر(int شهر) {
if ((شهر == 1)) {
return "يناير";
}
if ((شهر == 2)) {
return "فبراير";
}
if ((شهر == 3)) {
return "مارس";
}
if ((شهر == 4)) {
return "أبريل";
}
if ((شهر == 5)) {
return "مايو";
}
if ((شهر == 6)) {
return "يونيو";
}
if ((شهر == 7)) {
return "يوليو";
}
if ((شهر == 8)) {
return "أغسطس";
}
if ((شهر == 9)) {
return "سبتمبر";
}
if ((شهر == 10)) {
return "أكتوبر";
}
if ((شهر == 11)) {
return "نوفمبر";
}
if ((شهر == 12)) {
return "ديسمبر";
}
return "غير معروف";
}
int هل_كبيسة(int سنة) {
if (((سنة % 4) == 0)) {
if (((سنة % 100) != 0)) {
return 1;
}
}
if (((سنة % 400) == 0)) {
return 1;
}
return 0;
}
int شهر_31(int شهر) {
if ((شهر == 1)) {
return 1;
}
if ((شهر == 3)) {
return 1;
}
if ((شهر == 5)) {
return 1;
}
if ((شهر == 7)) {
return 1;
}
if ((شهر == 8)) {
return 1;
}
if ((شهر == 10)) {
return 1;
}
if ((شهر == 12)) {
return 1;
}
return 0;
}
int شهر_30(int شهر) {
if ((شهر == 4)) {
return 1;
}
if ((شهر == 6)) {
return 1;
}
if ((شهر == 9)) {
return 1;
}
if ((شهر == 11)) {
return 1;
}
return 0;
}
int أيام_الشهر(int شهر, int سنة) {
شهر_31(شهر);
if ((شهر == 1)) {
return 31;
}
شهر_30(شهر);
if ((شهر == 1)) {
return 30;
}
if ((شهر == 2)) {
هل_كبيسة(سنة);
if ((سنة == 1)) {
return 29;
}
return 28;
}
return 30;
}
int اليوم = 26;
int الشهر = 6;
int السنة = 2026;
int يوم_الأسبوع = 5;
int الساعة = 10;
int الدقيقة = 30;
daad::runtime::daad_print("=== الساعة والتقويم ===");
daad::runtime::daad_print("الوقت: ");
daad::runtime::daad_print(الساعة);
daad::runtime::daad_print(":");
if ((الدقيقة < 10)) {
daad::runtime::daad_print("0");
}
daad::runtime::daad_print(الدقيقة);
daad::runtime::daad_print("التاريخ: ");
daad::runtime::daad_print(اليوم);
daad::runtime::daad_print("/");
daad::runtime::daad_print(الشهر);
daad::runtime::daad_print("/");
daad::runtime::daad_print(السنة);
daad::runtime::daad_print("اليوم: ");
تحديد_اليوم(يوم_الأسبوع);
daad::runtime::daad_print(يوم_الأسبوع);
daad::runtime::daad_print("الشهر: ");
تحديد_الشهر(الشهر);
daad::runtime::daad_print(الشهر);
أيام_الشهر(الشهر, السنة);
int أيام = السنة;
daad::runtime::daad_print("عدد أيام هذا الشهر: ");
daad::runtime::daad_print(أيام);
هل_كبيسة(السنة);
if ((السنة == 1)) {
daad::runtime::daad_print("السنة كبيسة");
}
 else {
daad::runtime::daad_print("السنة ليست كبيسة");
}
int متبقية = (أيام - اليوم);
daad::runtime::daad_print("الأيام المتبقية من الشهر: ");
daad::runtime::daad_print(متبقية);
