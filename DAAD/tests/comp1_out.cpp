#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int عددي = 42;
double كسري = 3.141590;
bool صحيح_أ = true;
bool خطأ_أ = false;
int حرف_أ = 65;
std::string رسالتي = "مرحبا بالعالم";
daad::runtime::daad_print("═══ اختبار الأنواع ═══");
daad::runtime::daad_print(عددي);
daad::runtime::daad_print(كسري);
daad::runtime::daad_print(صحيح_أ);
daad::runtime::daad_print(خطأ_أ);
daad::runtime::daad_print(حرف_أ);
daad::runtime::daad_print(رسالتي);
int مجموع = 15;
int طرح = 7;
int ضرب = 28;
int قسمة = 5;
int باقي = 2;
daad::runtime::daad_print("═══ العمليات الحسابية ═══");
daad::runtime::daad_print(مجموع);
daad::runtime::daad_print(طرح);
daad::runtime::daad_print(ضرب);
daad::runtime::daad_print(قسمة);
daad::runtime::daad_print(باقي);
double مجموع_عشري = 3;
double ضرب_عشري = 10;
daad::runtime::daad_print("═══ العمليات العشرية ═══");
daad::runtime::daad_print(مجموع_عشري);
daad::runtime::daad_print(ضرب_عشري);
int أ = 10;
int ب = 20;
bool أكبر = (أ > ب);
bool أصغر = (أ < ب);
bool يساوي = (أ == 10);
bool لا_يساوي = (أ != ب);
bool أكبر_أو_يساوي = (أ >= 10);
bool أصغر_أو_يساوي = (أ <= 5);
daad::runtime::daad_print("═══ عمليات المقارنة ═══");
daad::runtime::daad_print(أكبر);
daad::runtime::daad_print(أصغر);
daad::runtime::daad_print(يساوي);
daad::runtime::daad_print(لا_يساوي);
daad::runtime::daad_print(أكبر_أو_يساوي);
daad::runtime::daad_print(أصغر_أو_يساوي);
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
int أس_2 = std::pow(2, 10);
daad::runtime::daad_print("قوة 2 أس 10");
daad::runtime::daad_print(أس_2);
int قيمة = 3;
قيمة = std::pow(قيمة, 2);
daad::runtime::daad_print(قيمة);
bool ص = true;
bool خ = false;
bool و_المنطقي = (ص && خ);
bool أو_المنطقي = (ص || خ);
bool ليس = (!ص);
daad::runtime::daad_print("═══ العمليات المنطقية ═══");
daad::runtime::daad_print(و_المنطقي);
daad::runtime::daad_print(أو_المنطقي);
daad::runtime::daad_print(ليس);
bool وعربي = (ص && خ);
bool أوعربي = (ص || خ);
daad::runtime::daad_print("═══ المنطق العربي ═══");
daad::runtime::daad_print(وعربي);
daad::runtime::daad_print(أوعربي);
int أكبر_قيمة = أ;
if ((ب > أكبر_قيمة)) {
أكبر_قيمة = ب;
}
daad::runtime::daad_print("أكبر قيمة");
daad::runtime::daad_print(أكبر_قيمة);
daad::runtime::daad_print("═══ اكتمل اختبار الأنواع والعمليات ═══");
