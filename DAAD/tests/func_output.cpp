#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int مجموع(int أ, int ب) {
return (أ + ب);
}
int مربع(int س) {
return (س * س);
}
int النتيجة1 = مجموع(5, 3);
daad::runtime::daad_print(النتيجة1);
int النتيجة2 = مربع(4);
daad::runtime::daad_print(النتيجة2);
