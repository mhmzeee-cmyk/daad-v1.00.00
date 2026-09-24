#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int مقارنة(int تخمين, int سري) {
if ((تخمين == سري)) {
return 0;
}
if ((تخمين < سري)) {
return 1;
}
return 2;
}
int الرقم_السري = 42;
int محاولات = 0;
int فاز = 0;
std::vector<int> تخمينات = 0;
daad::runtime::daad_print("=== لعبة تخمين الأرقام ===");
daad::runtime::daad_print("خمن رقم بين 1 و 100");
تخمينات[0] = 50;
تخمينات[1] = 25;
تخمينات[2] = 37;
تخمينات[3] = 42;
for (int i = 0; (i < 4); i = (i + 1)) {
محاولات += 1;
مقارنة(تخمينات[i], الرقم_السري);
int نتيجة = الرقم_السري;
daad::runtime::daad_print("محاولة ");
daad::runtime::daad_print(محاولات);
daad::runtime::daad_print(": ");
daad::runtime::daad_print(تخمينات[i]);
if ((نتيجة == 0)) {
daad::runtime::daad_print(" فزت!");
فاز = 1;
}
 else {
if ((نتيجة == 1)) {
daad::runtime::daad_print("↑ أعلى!");
}
 else {
daad::runtime::daad_print("↓ أدنى!");
}
}
}
if ((فاز == 1)) {
daad::runtime::daad_print("تهانينا! فزت في ");
daad::runtime::daad_print(محاولات);
daad::runtime::daad_print(" محاولات");
}
 else {
daad::runtime::daad_print("خسرت! الرقم كان ");
daad::runtime::daad_print(الرقم_السري);
}
