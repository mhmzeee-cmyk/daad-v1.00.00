#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int أ = 10;
bool ص = true;
bool خ = false;
bool وعربي = (ص && خ);
daad::runtime::daad_print(وعربي);
