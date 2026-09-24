#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

std::string تحديد_الصلاة(int ساعة, int دقيقة) {
int وقت = ((ساعة * 60) + دقيقة);
if ((وقت >= 300)) {
if ((وقت < 360)) {
return "الفجر";
}
}
if ((وقت >= 720)) {
if ((وقت < 780)) {
return "الظهر";
}
}
if ((وقت >= 870)) {
if ((وقت < 930)) {
return "العصر";
}
}
if ((وقت >= 1080)) {
if ((وقت < 1140)) {
return "المغرب";
}
}
if ((وقت >= 1200)) {
if ((وقت < 1260)) {
return "العشاء";
}
}
return "خارج وقت الصلاة";
}
void طباعة_أوقات_الصلاة() {
daad::runtime::daad_print("=== أوقات الصلاة اليومية ===");
daad::runtime::daad_print("الفجر: 05:00");
daad::runtime::daad_print("الظهر: 12:00");
daad::runtime::daad_print("العصر: 14:30");
daad::runtime::daad_print("المغرب: 18:00");
daad::runtime::daad_print("العشاء: 20:00");
}
int معرفة_الوقت_التالي(int ساعة) {
if ((ساعة < 5)) {
return 5;
}
if ((ساعة < 12)) {
return 12;
}
if ((ساعة < 14)) {
return 14;
}
if ((ساعة < 18)) {
return 18;
}
if ((ساعة < 20)) {
return 20;
}
return 5;
}
int عدّ_الصلاة(int ساعة) {
if ((ساعة < 5)) {
return 0;
}
if ((ساعة < 12)) {
return 1;
}
if ((ساعة < 14)) {
return 2;
}
if ((ساعة < 18)) {
return 3;
}
if ((ساعة < 20)) {
return 4;
}
return 5;
}
int الساعة = 15;
int الدقيقة = 30;
daad::runtime::daad_print("=== نظام تنبيه الصلاة ===");
daad::runtime::daad_print("الوقت الحالي: ");
daad::runtime::daad_print(الساعة);
daad::runtime::daad_print(":");
if ((الدقيقة < 10)) {
daad::runtime::daad_print("0");
}
daad::runtime::daad_print(الدقيقة);
تحديد_الصلاة(الساعة, الدقيقة);
std::string صلاة_الآن = الدقيقة;
daad::runtime::daad_print("الصلاة الحالية: ");
daad::runtime::daad_print(صلاة_الآن);
عدّ_الصلاة(الساعة);
int صلوات_اليوم = الساعة;
int صلوات_متبقية = (5 - صلوات_اليوم);
daad::runtime::daad_print("الصلوات المصلية اليوم: ");
daad::runtime::daad_print(صلوات_اليوم);
daad::runtime::daad_print("الصلوات المتبقية: ");
daad::runtime::daad_print(صلوات_متبقية);
معرفة_الوقت_التالي(الساعة);
int وقت_التالي = الساعة;
daad::runtime::daad_print("أقرب صلاة: ");
daad::runtime::daad_print(وقت_التالي);
daad::runtime::daad_print(":00");
طباعة_أوقات_الصلاة();
":00";
