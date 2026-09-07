#include "GuiDesignerModel.hpp"
#include <QSet>

GuiDesignerModel::GuiDesignerModel(QObject* parent) : QAbstractListModel(parent) {}
GuiDesignerModel::~GuiDesignerModel() { qDeleteAll(m_widgets); }

int GuiDesignerModel::rowCount(const QModelIndex&) const { return m_widgets.size(); }

QVariant GuiDesignerModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_widgets.size()) return {};
    auto* w = m_widgets.at(index.row());
    if (role == WidgetObjectRole) return QVariant::fromValue(w);
    return {};
}

QHash<int, QByteArray> GuiDesignerModel::roleNames() const {
    return {{WidgetObjectRole, "widget"}};
}

int GuiDesignerModel::count() const { return m_widgets.size(); }
int GuiDesignerModel::selectedIndex() const { return m_selectedIndex; }

void GuiDesignerModel::setSelectedIndex(int index) {
    if (m_selectedIndex == index) return;
    m_selectedIndex = index;
    emit selectedIndexChanged();
}

int GuiDesignerModel::addWidget(const QString& type, qreal x, qreal y) {
    int idx = m_widgets.size();
    beginInsertRows(QModelIndex(), idx, idx);
    auto* node = new WidgetNode(type, this);
    node->setX(x);
    node->setY(y);

    // Semantic naming based on widget type
    static const QMap<QString, QString> typePrefixes = {
        {"زر", "زر_أمر"},
        {"حقل_نص", "حقل_نص"},
        {"تسمية", "تسمية"},
        {"قائمة_خيارات", "قائمة_خيارات"},
        {"صورة", "صورة"},
        {"مربع_اختيار", "مربع_اختيار"},
        {"شريط_تمرير", "شريط_تمرير"},
        {"قائمة_منسدلة", "قائمة_منسدلة"},
        {"عدد", "عدد"},
        {"صفة", "صفة"},
        {"شبكية", "شبكية"},
        {"مجموعة", "مجموعة"},
        {"شريط_تقدم", "شريط_تقدم"},
        {"علامة_تبويب", "علامة_تبويب"},
        {"شريط_أدوات", "شريط_أدوات"},
        {"شريط_حالة", "شريط_حالة"},
        {"مربع_حوار", "مربع_حوار"}
    };

    QString prefix = typePrefixes.value(type, "عنصر");

    QSet<QString> names;
    for (auto* w : m_widgets) names.insert(w->variableName());

    int typeCount = 0;
    for (auto* w : m_widgets) {
        if (w->type() == type) typeCount++;
    }

    QString base = prefix + "_" + QString::number(typeCount + 1);
    QString name = base;
    int counter = 1;
    while (names.contains(name)) { name = base + "_" + QString::number(counter++); }
    node->setVariableName(name);

    m_widgets.append(node);
    endInsertRows();
    emit countChanged();
    regenerateCode();
    return idx;
}

void GuiDesignerModel::removeWidget(int index) {
    if (index < 0 || index >= m_widgets.size()) return;
    beginRemoveRows(QModelIndex(), index, index);
    delete m_widgets.takeAt(index);
    endRemoveRows();
    if (m_selectedIndex >= m_widgets.size()) m_selectedIndex = m_widgets.size() - 1;
    emit countChanged();
    emit selectedIndexChanged();
    regenerateCode();
}

void GuiDesignerModel::clear() {
    if (m_widgets.isEmpty()) return;
    beginResetModel();
    qDeleteAll(m_widgets);
    m_widgets.clear();
    endResetModel();
    m_selectedIndex = -1;
    emit countChanged();
    emit selectedIndexChanged();
    regenerateCode();
}

QVariantMap GuiDesignerModel::getWidget(int index) const {
    if (index < 0 || index >= m_widgets.size()) return {};
    return m_widgets.at(index)->toMap();
}

void GuiDesignerModel::updateWidgetProperty(int index, const QString& prop, const QVariant& value) {
    if (index < 0 || index >= m_widgets.size()) return;
    auto* w = m_widgets.at(index);

    if (prop == "x") w->setX(value.toReal());
    else if (prop == "y") w->setY(value.toReal());
    else if (prop == "width") w->setWidth(value.toReal());
    else if (prop == "height") w->setHeight(value.toReal());
    else if (prop == "text") w->setText(value.toString());
    else if (prop == "variableName") w->setVariableName(value.toString());
    else if (prop == "color") w->setColor(value.toString());
    else if (prop == "fontSize") w->setFontSize(value.toReal());

    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
    regenerateCode();
}

QString GuiDesignerModel::daadTypeKeyword(const QString& visualType) const {
    if (visualType == "زر") return "زر_أمر";
    if (visualType == "نص") return "حقل_نص";
    if (visualType == "عنوان") return "نص";
    if (visualType == "قائمة") return "قائمة_خيارات";
    if (visualType == "صورة") return "صورة";
    if (visualType == "مربع") return "مربع_اختيار";
    if (visualType == "شريط") return "شريط_تمرير";
    if (visualType == "قائمة_منسدلة") return "قائمة_منسدلة";
    return "عنصر";
}

QString GuiDesignerModel::generatedCode() const { return m_generatedCode; }

void GuiDesignerModel::regenerateCode() {
    QString code;
    code += "// === كود الواجهة المرئية ===\n";
    code += "// تم التوليد تلقائياً من مصمم الواجهات\n\n";

    for (auto* w : m_widgets) {
        QString varName = w->variableName();
        QString daadType = daadTypeKeyword(w->type());
        code += "عرّف " + varName + " = " + daadType + "()\n";

        // Position first (RTL logical order)
        code += varName + ".الموقع_س = " + QString::number(static_cast<int>(w->x())) + "\n";
        code += varName + ".الموقع_ص = " + QString::number(static_cast<int>(w->y())) + "\n";

        // Size
        code += varName + ".العرض = " + QString::number(static_cast<int>(w->width())) + "\n";
        code += varName + ".الارتفاع = " + QString::number(static_cast<int>(w->height())) + "\n";

        // Text content
        if (!w->text().isEmpty())
            code += varName + ".النص = \"" + w->text() + "\"\n";

        // Appearance
        code += varName + ".اللون = \"" + w->color() + "\"\n";

        if (w->fontSize() != 13)
            code += varName + ".حجم الخط = " + QString::number(w->fontSize(), 'f', 1) + "\n";

        code += "\n";
    }

    if (m_generatedCode != code) {
        m_generatedCode = code;
        emit generatedCodeChanged();
        emit designChanged();
    }
}

void GuiDesignerModel::importFromCode(const QString& code) {
    beginResetModel();
    qDeleteAll(m_widgets);
    m_widgets.clear();

    QStringList lines = code.split('\n');
    for (const auto& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.startsWith("عرّف ") && trimmed.contains(" = ")) {
            int eqIdx = trimmed.indexOf(" = ");
            QString varName = trimmed.mid(4, eqIdx - 4).trimmed();
            QString typeExpr = trimmed.mid(eqIdx + 3).trimmed();
            QString daadType;
            static const QMap<QString, QString> typeMap = {
                {"زر_أمر", "زر"}, {"حقل_نص", "نص"}, {"نص", "عنوان"},
                {"قائمة_خيارات", "قائمة"}, {"صورة", "صورة"},
                {"مربع_اختيار", "مربع"}, {"شريط_تمرير", "شريط"},
                {"قائمة_منسدلة", "قائمة_منسدلة"}
            };
            for (auto it = typeMap.begin(); it != typeMap.end(); ++it) {
                if (typeExpr.startsWith(it.key())) { daadType = it.value(); break; }
            }
            if (daadType.isEmpty()) continue;
            auto* node = new WidgetNode(daadType, this);
            node->setVariableName(varName);
            m_widgets.append(node);
        }
    }
    endResetModel();
    m_selectedIndex = -1;
    emit countChanged();
    emit selectedIndexChanged();
    regenerateCode();
}
