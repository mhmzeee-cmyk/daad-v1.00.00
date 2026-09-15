#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

daad::runtime::daad_print("الأعداد من 1 إلى 10:");
for (int i = 1; (i <= 10); i = (i + 1)) {
daad::runtime::daad_print(i);
}
daad::runtime::daad_print("جدول ضرب 7:");
for (int i = 1; (i <= 10); i = (i + 1)) {
daad::runtime::daad_print("7 × ");
daad::runtime::daad_print(i);
daad::runtime::daad_print(" = ");
daad::runtime::daad_print((7 * i));
}
int ع = 1;
int مجموع = 0;
while ((ع <= 100)) {
مجموع = (مجموع + ع);
ع = (ع + 1);
}
daad::runtime::daad_print("مجموع الأعداد من 1 إلى 100:");
daad::runtime::daad_print(مجموع);
