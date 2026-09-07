#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int عددي = 42;
double كسري = 3.140000;
bool صحيح_أ = true;
std::string رسالتي = "مرحبا";
daad::runtime::daad_print(عددي);
daad::runtime::daad_print(كسري);
daad::runtime::daad_print(صحيح_أ);
daad::runtime::daad_print(رسالتي);
int مجموع = 15;
int ضرب = 28;
int قسمة = 5;
int باقي = 2;
daad::runtime::daad_print(مجموع);
daad::runtime::daad_print(ضرب);
daad::runtime::daad_print(قسمة);
daad::runtime::daad_print(باقي);
