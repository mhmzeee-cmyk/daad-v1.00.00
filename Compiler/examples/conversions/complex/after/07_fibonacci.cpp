#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int فيبوناتشي(int ن) {
if ((ن <= 0)) {
return 0;
}
if ((ن == 1)) {
return 1;
}
int أ = 0;
int ب = 1;
int مؤقت = 0;
for (int i = 2; (i <= ن); i = (i + 1)) {
مؤقت = (أ + ب);
أ = ب;
ب = مؤقت;
}
return ب;
}
daad::runtime::daad_print("أول 15 عدد فيبوناتشي:");
for (int i = 0; (i < 15); i = (i + 1)) {
فيبوناتشي(i);
daad::runtime::daad_print(i);
}
int مجموع = 0;
for (int i = 0; (i < 10); i = (i + 1)) {
فيبوناتشي(i);
مجموع += i;
}
daad::runtime::daad_print("مجموع أول 10 أعداد فيبوناتشي:");
daad::runtime::daad_print(مجموع);
