#include "output.hpp"
#include "Daad/Runtime/DaadRuntime.hpp"
#include "stdlib/Math.hpp"
#include "stdlib/DaadStdlib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int عمر = 25;
daad::runtime::daad_print("═══ اختبار إذا/وإلا ═══");
if ((عمر >= 18)) {
daad::runtime::daad_print("بالغ");
}
 else {
daad::runtime::daad_print("قاصر");
}
int درجة = 85;
std::string تقدير = "";
if ((درجة >= 90)) {
تقدير = "ممتاز";
}
 else {
if ((درجة >= 80)) {
تقدير = "جيد جداً";
}
 else {
if ((درجة >= 70)) {
تقدير = "جيد";
}
 else {
if ((درجة >= 60)) {
تقدير = "مقبول";
}
 else {
تقدير = "راسب";
}
}
}
}
daad::runtime::daad_print("التقدير");
daad::runtime::daad_print(تقدير);
daad::runtime::daad_print("═══ اختبار حلقةطالما ═══");
int عداد1 = 0;
while ((عداد1 < 5)) {
daad::runtime::daad_print(عداد1);
عداد1 += 1;
}
daad::runtime::daad_print("═══ اختبار افعل/طالما ═══");
int عداد2 = 0;
do {
daad::runtime::daad_print(عداد2);
عداد2 += 1;
} while ((عداد2 < 3));
daad::runtime::daad_print("═══ اختبار حلقةلكل ═══");
for (ا = 0; (ا < 10); ا += 1;
1) {
if (((ا % 2) == 0)) {
daad::runtime::daad_print(ا);
}
}
daad::runtime::daad_print("═══ اختبار حلقةبينما ═══");
int ع = 5;
while ((ع > 0)) {
daad::runtime::daad_print(ع);
ع -= 1;
}
daad::runtime::daad_print("═══ اختبار حلقةكرر ═══");
int عداد3 = 0;
for (; (عداد3 < 3); ) {
daad::runtime::daad_print(عداد3);
عداد3 += 1;
}
daad::runtime::daad_print("═══ اختبار اختر ═══");
int يوم = 3;
switch (يوم) {
case 1:
break;
case 2:
break;
case 3:
break;
case 4:
break;
case 5:
break;
default:
daad::runtime::daad_print("عطلة");
break;
}
daad::runtime::daad_print("═══ اختبار حلقات متداخلة ═══");
for (ص = 0; (ص < 3); ص += 1;
1) {
for (س = 0; (س < 3); س += 1;
1) {
int نتيجة = (((ص * 3) + س) + 1);
daad::runtime::daad_print(نتيجة);
}
}
daad::runtime::daad_print("═══ اختبار توقف واستمر ═══");
for (ا = 0; (ا < 10); ا += 1;
1) {
if ((ا == 3)) {
continue;
}
if ((ا == 7)) {
break;
}
daad::runtime::daad_print(ا);
}
daad::runtime::daad_print("═══ اختبار فرز ═══");
int مصفوفة_0 = 5;
int مصفوفة_1 = 3;
int مصفوفة_2 = 8;
int مصفوفة_3 = 1;
int مصفوفة_4 = 9;
if ((مصفوفة_0 > مصفوفة_1)) {
int مؤقت = مصفوفة_0;
مصفوفة_0 = مصفوفة_1;
مصفوفة_1 = مؤقت;
}
if ((مصفوفة_1 > مصفوفة_2)) {
int مؤقت = مصفوفة_1;
مصفوفة_1 = مصفوفة_2;
مصفوفة_2 = مؤقت;
}
if ((مصفوفة_2 > مصفوفة_3)) {
int مؤقت = مصفوفة_2;
مصفوفة_2 = مصفوفة_3;
مصفوفة_3 = مؤقت;
}
if ((مصفوفة_3 > مصفوفة_4)) {
int مؤقت = مصفوفة_3;
مصفوفة_3 = مصفوفة_4;
مصفوفة_4 = مؤقت;
}
daad::runtime::daad_print(مصفوفة_0);
daad::runtime::daad_print(مصفوفة_1);
daad::runtime::daad_print(مصفوفة_2);
daad::runtime::daad_print(مصفوفة_3);
daad::runtime::daad_print(مصفوفة_4);
daad::runtime::daad_print("═══ اكتمل اختبار جمل التحكم ═══");
