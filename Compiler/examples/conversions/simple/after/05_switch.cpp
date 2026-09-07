#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int اختيار = 2;
switch (اختيار) {
case 1:
daad::runtime::daad_print("اخترت الحرف أ");
break;
case 2:
daad::runtime::daad_print("اخترت الحرف ب");
break;
case 3:
daad::runtime::daad_print("اخترت الحرف ج");
break;
default:
daad::runtime::daad_print("اختيار غير صحيح");
break;
}
int رقم = 7;
int ناتج = (رقم / 2);
int باقي = (رقم - (ناتج * 2));
if ((باقي == 0)) {
daad::runtime::daad_print("الرقم زوجي");
}
 else {
daad::runtime::daad_print("الرقم فردي");
}
int س = 5;
if ((س < 0)) {
س = (0 - س);
}
daad::runtime::daad_print("القيمة المطلقة: ");
daad::runtime::daad_print(س);
