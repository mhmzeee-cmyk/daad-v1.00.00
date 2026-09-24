#include "WidgetNode.hpp"
#include <algorithm>

WidgetNode::WidgetNode(const QString& type, QObject* parent)
    : QObject(parent), m_type(type)
{
    if (type == "\u0632\u0631")                     { m_text = "\u0632\u0631"; m_width = 120; m_height = 40; }
    else if (type == "\u062D\u0642\u0644_\u0646\u0635")    { m_text = "\u062D\u0642\u0644 \u0646\u0635"; m_width = 200; m_height = 36; }
    else if (type == "\u062A\u0633\u0645\u064A\u0629")     { m_text = "\u062A\u0633\u0645\u064A\u0629"; m_width = 180; m_height = 32; m_fontSize = 16; }
    else if (type == "\u0642\u0627\u0626\u0645\u0629_\u062E\u064A\u0627\u0631\u0627\u062A") { m_text = "\u0642\u0627\u0626\u0645\u0629"; m_width = 180; m_height = 36; }
    else if (type == "\u0635\u0648\u0631\u0629")    { m_text = "\u0635\u0648\u0631\u0629"; m_width = 120; m_height = 120; }
    else if (type == "\u0645\u0628\u0631\u0632_\u0627\u062E\u062A\u064A\u0627\u0631") { m_text = "\u0645\u0628\u0631\u0632"; m_width = 140; m_height = 30; }
    else if (type == "\u0634\u0631\u0637_\u062A\u0645\u0631\u064A\u0631") { m_text = "\u0634\u0631\u0637"; m_width = 200; m_height = 30; m_borderRadius = 2; }
    else if (type == "\u0642\u0627\u0626\u0645\u0629_\u0645\u0646\u0633\u062F\u0644\u0629") { m_text = "\u0642\u0627\u0626\u0645\u0629"; m_width = 160; m_height = 36; }
    else if (type == "\u0639\u0645\u0648\u062F")    { m_text = "\u0639\u0645\u0648\u062F"; m_width = 60; m_height = 200; }
    else if (type == "\u0635\u0641")               { m_text = "\u0635\u0641"; m_width = 400; m_height = 40; }
    else if (type == "\u0634\u0628\u0643\u0629")   { m_text = "\u0634\u0628\u0643\u0629"; m_width = 300; m_height = 200; }
    else if (type == "\u0633\u062D\u0631")         { m_text = "\u0633\u062D\u0631"; m_width = 200; m_height = 150; }
    else if (type == "\u0645\u062C\u0645\u0648\u0639\u0629") { m_text = "\u0645\u062C\u0645\u0648\u0639\u0629"; m_width = 250; m_height = 150; }
    else if (type == "\u0634\u0631\u0637_\u062A\u0644\u0648\u064A\u0646") { m_text = "50%"; m_width = 200; m_height = 20; m_borderRadius = 2; }
    else if (type == "\u0639\u0644\u0627\u0645\u0629_\u062A\u0628\u0648\u064A\u0628") { m_text = "\u062A\u0628\u0648\u064A\u0628 1"; m_width = 240; m_height = 32; }
}

void WidgetNode::setX(qreal v) { if (qFuzzyCompare(m_x, v)) return; m_x = v; emit xChanged(); }
void WidgetNode::setY(qreal v) { if (qFuzzyCompare(m_y, v)) return; m_y = v; emit yChanged(); }
void WidgetNode::setWidth(qreal v) { if (qFuzzyCompare(m_width, v)) return; m_width = v; emit widthChanged(); }
void WidgetNode::setHeight(qreal v) { if (qFuzzyCompare(m_height, v)) return; m_height = v; emit heightChanged(); }
void WidgetNode::setText(const QString& v) { if (m_text == v) return; m_text = v; emit textChanged(); }
void WidgetNode::setVariableName(const QString& v) { if (m_variableName == v) return; m_variableName = v; emit variableNameChanged(); }
void WidgetNode::setColor(const QString& v) { if (m_color == v) return; m_color = v; emit colorChanged(); }
void WidgetNode::setFontSize(qreal v) { if (qFuzzyCompare(m_fontSize, v)) return; m_fontSize = v; emit fontSizeChanged(); }
void WidgetNode::setEnabled(bool v) { if (m_enabled == v) return; m_enabled = v; emit enabledChanged(); }
void WidgetNode::setTooltip(const QString& v) { if (m_tooltip == v) return; m_tooltip = v; emit tooltipChanged(); }
void WidgetNode::setOpacity(qreal v) { if (qFuzzyCompare(m_opacity, v)) return; m_opacity = v; emit opacityChanged(); }
void WidgetNode::setBorderRadius(qreal v) { if (qFuzzyCompare(m_borderRadius, v)) return; m_borderRadius = v; emit borderRadiusChanged(); }
void WidgetNode::setVisibleNode(bool v) { if (m_visible == v) return; m_visible = v; emit visibleChanged(); }

QVariantMap WidgetNode::toMap() const {
    return {
        {"type", m_type}, {"x", m_x}, {"y", m_y},
        {"width", m_width}, {"height", m_height},
        {"text", m_text}, {"variableName", m_variableName},
        {"color", m_color}, {"fontSize", m_fontSize},
        {"enabled", m_enabled}, {"tooltip", m_tooltip},
        {"opacity", m_opacity}, {"borderRadius", m_borderRadius},
        {"visible", m_visible}
    };
}

WidgetNode* WidgetNode::fromMap(const QVariantMap& map, QObject* parent) {
    auto* node = new WidgetNode(map["type"].toString(), parent);
    node->setX(map["x"].toReal());
    node->setY(map["y"].toReal());
    node->setWidth(map["width"].toReal());
    node->setHeight(map["height"].toReal());
    node->setText(map["text"].toString());
    node->setVariableName(map["variableName"].toString());
    node->setColor(map["color"].toString());
    node->setFontSize(map["fontSize"].toReal());
    node->setEnabled(map["enabled"].toBool());
    node->setTooltip(map["tooltip"].toString());
    node->setOpacity(map["opacity"].toReal());
    node->setBorderRadius(map["borderRadius"].toReal());
    node->setVisibleNode(map["visible"].toBool());
    return node;
}
