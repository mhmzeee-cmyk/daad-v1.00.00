#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

std::string مسار = "test.txt";
افتح(مسار, 1);
int ملف = 1;
if ((ملف == 0)) {
اكتب_سطر(ملف, "مرحبا بالعالم");
"مرحبا بالعالم";
أغلق(ملف);
ملف;
daad::runtime::daad_print("تم الكتابة بنجاح");
}
 else {
daad::runtime::daad_print("خطأ في فتح الملف");
}
افتح(مسار, 0);
int ملف2 = 0;
if ((ملف2 == 0)) {
اقرأ_سطر(ملف2);
std::string محتوى = ملف2;
daad::runtime::daad_print("المحتوى المقروء:");
daad::runtime::daad_print(المحتوى);
أغلق(ملف2);
ملف2;
}
 else {
daad::runtime::daad_print("خطأ في قراءة الملف");
}
