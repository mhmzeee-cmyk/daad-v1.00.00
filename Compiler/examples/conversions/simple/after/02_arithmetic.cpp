#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int ص1 = 10;
int ص2 = 5;
int مجموع = (ص1 + ص2);
int فرق = (ص1 - ص2);
int حاصل_الضرب = (ص1 * ص2);
int حاصل_القسمة = (ص1 / ص2);
daad::runtime::daad_print("المجموع: ");
daad::runtime::daad_print(مجموع);
daad::runtime::daad_print("الفرق: ");
daad::runtime::daad_print(فرق);
daad::runtime::daad_print("حاصل الضرب: ");
daad::runtime::daad_print(حاصل_الضرب);
daad::runtime::daad_print("حاصل القسمة: ");
daad::runtime::daad_print(حاصل_القسمة);
