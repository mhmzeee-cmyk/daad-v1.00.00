#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

bool ص = true;
bool خ = false;
bool وعربي = (ص && خ);
bool أوعربي = (ص || خ);
daad::runtime::daad_print(وعربي);
daad::runtime::daad_print(أوعربي);
