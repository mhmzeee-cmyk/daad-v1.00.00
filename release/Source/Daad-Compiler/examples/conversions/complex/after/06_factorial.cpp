#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int مضروب(int ن) {
int نتيجة = 1;
for (int i = 1; (i <= ن); i = (i + 1)) {
نتيجة *= i;
}
return نتيجة;
}
int مجموع(int ن) {
int م = 0;
for (int i = 1; (i <= ن); i = (i + 1)) {
م += i;
}
return م;
}
int قوة(int أس, int أصل) {
int نتيجة = 1;
for (int i = 0; (i < أس); i = (i + 1)) {
نتيجة *= أصل;
}
return نتيجة;
}
daad::runtime::daad_print("مضروب 5 = ");
مضروب(5);
daad::runtime::daad_print(5);
daad::runtime::daad_print("المجموع من 1 إلى 10:");
مجموع(10);
daad::runtime::daad_print(10);
daad::runtime::daad_print("2 أس 10 = ");
قوة(10, 2);
daad::runtime::daad_print(2);
