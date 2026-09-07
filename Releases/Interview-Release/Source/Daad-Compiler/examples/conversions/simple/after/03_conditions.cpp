#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int العمر = 20;
if ((العمر >= 18)) {
daad::runtime::daad_print("أنت بالغ");
}
 else {
daad::runtime::daad_print("أنت قاصر");
}
int الدرجة = 85;
if ((الدرجة >= 90)) {
daad::runtime::daad_print("ممتاز");
}
 else {
if ((الدرجة >= 80)) {
daad::runtime::daad_print("جيد جداً");
}
 else {
if ((الدرجة >= 70)) {
daad::runtime::daad_print("جيد");
}
 else {
daad::runtime::daad_print("مقبول");
}
}
}
