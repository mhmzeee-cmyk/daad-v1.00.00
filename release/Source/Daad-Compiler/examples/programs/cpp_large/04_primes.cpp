#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int هل_أولي(int ن) {
if ((ن <= 1)) {
return 0;
}
if ((ن <= 3)) {
return 1;
}
if (((ن % 2) == 0)) {
return 0;
}
int i = 3;
while (((i * i) <= ن)) {
if (((ن % i) == 0)) {
return 0;
}
i += 2;
}
return 1;
}
int عدّ_الأولوية(int حد) {
int عدّ = 0;
for (int i = 2; (i <= حد); i = (i + 1)) {
هل_أولي(i);
if ((i == 1)) {
عدّ += 1;
}
}
return عدّ;
}
daad::runtime::daad_print("=== الأعداد الأولية حتى 50 ===");
int عدد = 0;
for (int i = 2; (i <= 50); i = (i + 1)) {
هل_أولي(i);
if ((i == 1)) {
daad::runtime::daad_print(i);
عدد += 1;
}
}
daad::runtime::daad_print("عدد الأعداد الأولية حتى 50:");
daad::runtime::daad_print(عدد);
daad::runtime::daad_print("عدد الأعداد الأولية حتى 100:");
عدّ_الأولوية(100);
daad::runtime::daad_print(100);
