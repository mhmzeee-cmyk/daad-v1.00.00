#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int جمع(int أ, int ب) {
return (أ + ب);
}
int طرح(int أ, int ب) {
return (أ - ب);
}
int ضرب(int أ, int ب) {
return (أ * ب);
}
int قسمة(int أ, int ب) {
if ((ب == 0)) {
return 0;
}
return (أ / ب);
}
int باقي_القسمة(int أ, int ب) {
if ((ب == 0)) {
return 0;
}
return (أ % ب);
}
int س1 = 20;
int س2 = 6;
daad::runtime::daad_print("المجموع: ");
جمع(س1, س2);
daad::runtime::daad_print(س2);
daad::runtime::daad_print("الفرق: ");
طرح(س1, س2);
daad::runtime::daad_print(س2);
daad::runtime::daad_print("الحاصل: ");
ضرب(س1, س2);
daad::runtime::daad_print(س2);
daad::runtime::daad_print("الناتج: ");
قسمة(س1, س2);
daad::runtime::daad_print(س2);
daad::runtime::daad_print("الباقي: ");
باقي_القسمة(س1, س2);
daad::runtime::daad_print(س2);
