#include "DaadCompleter.h"
#include "Daad/Keywords.hpp"
#include "Daad/CodeGen.hpp"
#include <QStandardItem>
#include <QFileInfo>
#include <QDir>
#include <QIcon>

DaadCompleter::DaadCompleter(QObject* parent) : QCompleter(parent) {
    m_model = new QStandardItemModel(this);
    setModel(m_model);
    setCompletionMode(QCompleter::PopupCompletion);
    setCaseSensitivity(Qt::CaseSensitive);
    setFilterMode(Qt::MatchContains); // فازّي — يحتوي
    setWrapAround(false);
    setMaxVisibleItems(12);

    loadKeywords();
    loadStdlib();
    rebuildModel();
}

void DaadCompleter::loadKeywords() {
    // 118 كلمة من KeywordRegistry — قائمة ثابتة مطابقة للمرجع 01
    // (Registry لا يوفر تكراراً مباشراً، لذا نستخدم القائمة اليدوية)
    m_keywords = QStringList{
        "صحيح","عشري","منطقي","حرف","نص","ثابت","فراغ","تلقائي","صواب","خطأ","عدم","عرّف",
        "إذا","اذا","وإلا","طالما","بينما","افعل","لكل","كرر","في","اختر","حالة","افتراضي","توقف","اكسر","استمر","تابع","انتقل","س","ص","ارجع",
        "صنف","فئة","هيكل","تعداد","واجهة","نطاق","عام","خاص","محمي","يرث","ذاتي","هذا","الأصل","مجرّد",
        "دالة","جديد","احذف","مؤشر","مرجع","ساكن","مضمن","خارجي","قالب",
        "حاول","امسك","أخيراً","ارمِ","تأكد","استثناء","نوع_الـ","حجم_الـ","زد","انقص",
        "تزامن","انتظر","بانتظار","خيط","احجز","مشترك","فريد","استورد","صدّر","وحدة","بديل",
        "اطبع","طباعة","ادخل","و","أو",
        "زر_أمر","حقل_نص","قائمة_خيارات","صورة","مربع_اختيار","شريط_تمرير","قائمة_منسدلة","لوحة","تسمية","عمود","صف","شبكة","شريط_تلوين","علامة_تبويب",
        "حمّل_صورة","ارسم_صورة","حجم_صورة","احفظ_صورة","قص_صورة","غيّر_حجم","لف_صورة","قلب_صورة","شفافية","فلتر","تراكب","خلفية","بكسل","ارسم","ملء","مستطيل","دائرة","خط","نص_على_لوحة","مسح"
    };
}

void DaadCompleter::loadStdlib() {
    // 35 دالة من DaadStdlib + أسماء ملفات stdlib
    auto names = daad::CodeGenVisitor::stdlibExportedNames();
    for (auto& n : names) m_stdlib << QString::fromStdString(n);
    // ملفات stdlib/*.ض — 100+ وحدة
    QString base = "/home/mhmzeee/Downloads/daad-v1.00.00-main/Compiler/stdlib";
    QDir dir(base);
    if (dir.exists()) {
        auto subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (auto& sub : subdirs) {
            QDir sd(dir.filePath(sub));
            for (auto& f : sd.entryList(QStringList() << "*.ض", QDir::Files)) {
                QString name = QFileInfo(f).baseName(); // بدون .ض
                if (!m_stdlib.contains(name)) m_stdlib << name;
            }
        }
    }
    // أيضاً من stdlib_arabic
    QDir ar("/home/mhmzeee/Downloads/daad-v1.00.00-main/Compiler/stdlib_arabic");
    if (ar.exists()) {
        for (auto& sub : ar.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QDir sd(ar.filePath(sub));
            for (auto& f : sd.entryList(QStringList() << "*.ض", QDir::Files)) {
                QString name = QFileInfo(f).baseName();
                if (!m_stdlib.contains(name)) m_stdlib << name;
            }
        }
    }
}

void DaadCompleter::setDynamicWords(const QStringList& words) {
    m_dynamic = words;
    rebuildModel();
}

void DaadCompleter::rebuildModel() {
    m_model->clear();
    auto addList = [&](const QStringList& list, const QString& iconPath, const QString& type){
        for (auto& w : list) {
            QStandardItem* item = new QStandardItem(QIcon(iconPath), w);
            item->setData(type, Qt::UserRole);
            // tooltip عربي
            QString tip = w + " — " + type;
            if (type == "كلمة مفتاحية") {
                // حاول جلب المقابل C++
                try {
                    auto& reg = daad::getStandardKeywordRegistry();
                    auto kt = reg.findKeyword(w.toStdString());
                    if (kt) tip += " → " + QString::fromStdString(std::string(reg.getCppEquivalent(*kt)));
                } catch(...) {}
            }
            item->setToolTip(tip);
            m_model->appendRow(item);
        }
    };
    addList(m_keywords, ":/icons/keyword.png", "كلمة مفتاحية");
    addList(m_stdlib, ":/icons/stdlib.png", "مكتبة");
    addList(m_dynamic, ":/icons/variable.png", "متغير/دالة");
}

void DaadCompleter::updateForWord(const QString& prefix) {
    setCompletionPrefix(prefix);
    if (prefix.length() >= 2 || prefix == "ط" || prefix.length() >= 1) {
        // فازّي — إظهار حتى بحرف واحد عربي
        if (!prefix.isEmpty()) complete();
    }
}
