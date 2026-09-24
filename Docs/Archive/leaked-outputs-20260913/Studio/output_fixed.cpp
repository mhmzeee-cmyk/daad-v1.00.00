#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int main() {
    int س = 10;
    int ص = 20;
    int النتيجة = (س + ص);
    daad::runtime::daad_print("النتيجة: ", النتيجة);
    return 0;
}
