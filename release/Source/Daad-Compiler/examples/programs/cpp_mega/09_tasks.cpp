#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

std::string تحديد_الأولوية(int أولوية) {
if ((أولوية == 1)) {
return "عالية";
}
if ((أولوية == 2)) {
return "متوسطة";
}
if ((أولوية == 3)) {
return "منخفضة";
}
return "غير محددة";
}
std::string تحديد_الحالة_رقم(int رقم) {
if ((رقم == 0)) {
return "قيد التنفيذ";
}
if ((رقم == 1)) {
return "مكتملة";
}
if ((رقم == 2)) {
return "متأخرة";
}
return "غير محددة";
}
std::vector<int> مهام_الأولوية = 1;
std::vector<int> مهام_الحالة = 0;
int عدد_الهام = 5;
مهام_الأولوية[0] = 1;
مهام_الأولوية[1] = 2;
مهام_الأولوية[2] = 1;
مهام_الأولوية[3] = 3;
مهام_الأولوية[4] = 2;
مهام_الحالة[0] = 1;
مهام_الحالة[1] = 0;
مهام_الحالة[2] = 0;
مهام_الحالة[3] = 2;
مهام_الحالة[4] = 1;
int مكتملة = 0;
int قيد_التنفيذ = 0;
int متأخرة = 0;
for (int i = 0; (i < عدد_الهام); i = (i + 1)) {
if ((مهام_الحالة[i] == 1)) {
مكتملة += 1;
}
if ((مهام_الحالة[i] == 0)) {
قيد_التنفيذ += 1;
}
if ((مهام_الحالة[i] == 2)) {
متأخرة += 1;
}
}
daad::runtime::daad_print("=== نظام إدارة المهام ===");
daad::runtime::daad_print("عدد المهام: ");
daad::runtime::daad_print(عدد_الهام);
daad::runtime::daad_print("مكتملة: ");
daad::runtime::daad_print(مكتملة);
daad::runtime::daad_print("قيد التنفيذ: ");
daad::runtime::daad_print(قيد_التنفيذ);
daad::runtime::daad_print("متأخرة: ");
daad::runtime::daad_print(متأخرة);
int نسبة = ((مكتملة * 100) / عدد_الهام);
daad::runtime::daad_print("نسبة الإنجاز: ");
daad::runtime::daad_print(نسبة);
daad::runtime::daad_print("%");
daad::runtime::daad_print("=== المهام عالية الأولوية ===");
for (int i = 0; (i < عدد_الهام); i = (i + 1)) {
if ((مهام_الأولوية[i] == 1)) {
daad::runtime::daad_print("مهمة ");
daad::runtime::daad_print((i + 1));
daad::runtime::daad_print(" - الحالة: ");
تحديد_الحالة_رقم(مهام_الحالة[i]);
daad::runtime::daad_print(مهام_الحالة[i]);
}
}
