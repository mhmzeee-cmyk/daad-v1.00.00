#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int تحديد_المعدل(int مجموع, int عدد) {
if ((عدد == 0)) {
return 0;
}
return (مجموع / عدد);
}
std::string تحديد_التقدير(int درجة) {
if ((درجة >= 90)) {
return "ممتاز";
}
if ((درجة >= 80)) {
return "جيد جداً";
}
if ((درجة >= 70)) {
return "جيد";
}
if ((درجة >= 60)) {
return "مقبول";
}
return "راسب";
}
int طالب1 = 95;
int طالب2 = 82;
int طالب3 = 67;
int طالب4 = 78;
int طالب5 = 91;
int مجموع = ((((طالب1 + طالب2) + طالب3) + طالب4) + طالب5);
int عدد = 5;
تحديد_المجموع(مجموع, عدد);
int معدّل = عدد;
daad::runtime::daad_print("=== محوسب درجات الطلاب ===");
daad::runtime::daad_print("الطالب 1: ");
daad::runtime::daad_print(طالب1);
daad::runtime::daad_print("الطالب 2: ");
daad::runtime::daad_print(طالب2);
daad::runtime::daad_print("الطالب 3: ");
daad::runtime::daad_print(طالب3);
daad::runtime::daad_print("الطالب 4: ");
daad::runtime::daad_print(طالب4);
daad::runtime::daad_print("الطالب 5: ");
daad::runtime::daad_print(طالب5);
daad::runtime::daad_print("المجموع: ");
daad::runtime::daad_print(مجموع);
daad::runtime::daad_print("المعدّل: ");
daad::runtime::daad_print(معدّل);
int أعلى = طالب1;
if ((طالب2 > أعلى)) {
أعلى = طالب2;
}
if ((طالب3 > أعلى)) {
أعلى = طالب3;
}
if ((طالب4 > أعلى)) {
أعلى = طالب4;
}
if ((طالب5 > أعلى)) {
أعلى = طالب5;
}
int أدنى = طالب1;
if ((طالب2 < أدنى)) {
أدنى = طالب2;
}
if ((طالب3 < أدنى)) {
أدنى = طالب3;
}
if ((طالب4 < أدنى)) {
أدنى = طالب4;
}
if ((طالب5 < أدنى)) {
أدنى = طالب5;
}
daad::runtime::daad_print("أعلى درجة: ");
daad::runtime::daad_print(أعلى);
daad::runtime::daad_print("أدنى درجة: ");
daad::runtime::daad_print(أدنى);
