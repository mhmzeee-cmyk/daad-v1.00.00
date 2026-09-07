#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int فائدة_بسيطة(int رأس_المال, int معدل, int سنوات) {
return (((رأس_المال * معدل) * سنوات) / 100);
}
int فائدة_مركبة(int رأس_المال, int معدل, int سنوات) {
int مجموع = رأس_المال;
for (int i = 0; (i < سنوات); i = (i + 1)) {
مجموع += ((مجموع * معدل) / 100);
}
return (مجموع - رأس_المال);
}
int إجمالي(int رأس_المال, int فائدة) {
return (رأس_المال + فائدة);
}
int رأس = 10000;
int معدل = 5;
int سنوات = 3;
فائدة_بسيطة(رأس, معدل, سنوات);
int ف_بسيطة = سنوات;
فائدة_مركبة(رأس, معدل, سنوات);
int ف_مركبة = سنوات;
daad::runtime::daad_print("=== محاسبة الفائدة ===");
daad::runtime::daad_print("رأس المال: ");
daad::runtime::daad_print(رأس);
daad::runtime::daad_print("معدل الفائدة: ");
daad::runtime::daad_print(معدل);
daad::runtime::daad_print("السنوات: ");
daad::runtime::daad_print(سنوات);
daad::runtime::daad_print("الفائدة البسيطة: ");
daad::runtime::daad_print(ف_بسيطة);
daad::runtime::daad_print("الإجمالي (بسيطة): ");
إجمالي(رأس, ف_بسيطة);
daad::runtime::daad_print(ف_بسيطة);
daad::runtime::daad_print("الفائدة المركبة: ");
daad::runtime::daad_print(ف_مركبة);
daad::runtime::daad_print("الإجمالي (مركبة): ");
إجمالي(رأس, ف_مركبة);
daad::runtime::daad_print(ف_مركبة);
int فارق = (ف_مركبة - ف_بسيطة);
daad::runtime::daad_print("الفارق بين المركبة والبسيطة: ");
daad::runtime::daad_print(فارق);
