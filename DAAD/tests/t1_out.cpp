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
int س = 100;
س += 50;
daad::runtime::daad_print(س);
س -= 30;
daad::runtime::daad_print(س);
س *= 2;
daad::runtime::daad_print(س);
س /= 4;
daad::runtime::daad_print(س);
س %= 7;
daad::runtime::daad_print(س);
int عداد = 10;
عداد += 1;
daad::runtime::daad_print(عداد);
عداد -= 1;
daad::runtime::daad_print(عداد);
int أس = std::pow(2, 10);
daad::runtime::daad_print(أس);
int ق = 3;
ق = std::pow(ق, 2);
daad::runtime::daad_print(ق);
bool ص = true;
bool خ = false;
bool و_المنطقي = (ص && خ);
bool أو_المنطقي = (ص || خ);
bool ليس = (!ص);
daad::runtime::daad_print(و_المنطقي);
daad::runtime::daad_print(أو_المنطقي);
daad::runtime::daad_print(ليس);
bool وعربي = (ص && خ);
bool أوعربي = (ص || خ);
daad::runtime::daad_print(وعربي);
daad::runtime::daad_print(أوعربي);
daad::runtime::daad_print("انتهى اختبار 1");
