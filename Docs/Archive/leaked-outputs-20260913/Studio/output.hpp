#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int إنشاء_tcp();
int اربط_tcp(int tcp, std::string عنوان, int منفذ);
int اتصل_tcp(int tcp, std::string عنوان, int منفذ);
int أرسل_tcp(int tcp, std::string بيانات);
std::string استقبل_tcp(int tcp, int حجم);
void اغلق_tcp(int tcp);
int إنشاء_udp();
int أرسل_udp(int udp, std::string بيانات, std::string عنوان, int منفذ);
std::string استقبل_udp(int udp, int حجم);
void اغلق_udp(int udp);
int الحد_الأعلى_0(int أ, int ب);
int الحد_الأدنى_0(int أ, int ب);
int الفارق_0(int أ, int ب);
int الحد_الأعلى_1(int أ, int ب);
int الحد_الأدنى_1(int أ, int ب);
int الفارق_1(int أ, int ب);
int الحد_الأعلى_2(int أ, int ب);
int الحد_الأدنى_2(int أ, int ب);
int الفارق_2(int أ, int ب);
int الحد_الأعلى_3(int أ, int ب);
int الحد_الأدنى_3(int أ, int ب);
int الفارق_3(int أ, int ب);
int الحد_الأعلى_4(int أ, int ب);
int الحد_الأدنى_4(int أ, int ب);
int الفارق_4(int أ, int ب);
int الحد_الأعلى_5(int أ, int ب);
int الحد_الأدنى_5(int أ, int ب);
int الفارق_5(int أ, int ب);
int الحد_الأعلى_6(int أ, int ب);
int الحد_الأدنى_6(int أ, int ب);
int الفارق_6(int أ, int ب);
int الحد_الأعلى_7(int أ, int ب);
int الحد_الأدنى_7(int أ, int ب);
int الفارق_7(int أ, int ب);
int الحد_الأعلى_8(int أ, int ب);
int الحد_الأدنى_8(int أ, int ب);
