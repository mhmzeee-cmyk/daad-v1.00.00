#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int ع1 = 64;
int ع2 = 34;
int ع3 = 25;
int ع4 = 12;
int ع5 = 22;
int ع6 = 11;
int ع7 = 90;
int الأكبر = ع1;
if ((ع2 > الأكبر)) {
الأكبر = ع2;
}
if ((ع3 > الأكبر)) {
الأكبر = ع3;
}
if ((ع4 > الأكبر)) {
الأكبر = ع4;
}
if ((ع5 > الأكبر)) {
الأكبر = ع5;
}
if ((ع6 > الأكبر)) {
الأكبر = ع6;
}
if ((ع7 > الأكبر)) {
الأكبر = ع7;
}
int الأصغر = ع1;
if ((ع2 < الأصغر)) {
الأصغر = ع2;
}
if ((ع3 < الأصغر)) {
الأصغر = ع3;
}
if ((ع4 < الأصغر)) {
الأصغر = ع4;
}
if ((ع5 < الأصغر)) {
الأصغر = ع5;
}
if ((ع6 < الأصغر)) {
الأصغر = ع6;
}
if ((ع7 < الأصغر)) {
الأصغر = ع7;
}
int المجموع = ((((((ع1 + ع2) + ع3) + ع4) + ع5) + ع6) + ع7);
double المتوسط = (المجموع / 7);
daad::runtime::daad_print("الأكبر: ");
daad::runtime::daad_print(الأكبر);
daad::runtime::daad_print("الأصغر: ");
daad::runtime::daad_print(الأصغر);
daad::runtime::daad_print("المجموع: ");
daad::runtime::daad_print(المجموع);
daad::runtime::daad_print("المتوسط: ");
daad::runtime::daad_print(المتوسط);
int مرتب = 1;
if ((ع1 > ع2)) {
مرتب = 0;
}
if ((ع2 > ع3)) {
مرتب = 0;
}
if ((ع3 > ع4)) {
مرتب = 0;
}
if ((ع4 > ع5)) {
مرتب = 0;
}
if ((ع5 > ع6)) {
مرتب = 0;
}
if ((ع6 > ع7)) {
مرتب = 0;
}
if ((مرتب == 1)) {
daad::runtime::daad_print("الأعداد مرتبة تصاعدياً");
}
 else {
daad::runtime::daad_print("الأعداد غير مرتبة");
}
