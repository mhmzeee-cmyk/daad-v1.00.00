#pragma once

#include <QObject>
#include <QString>
#include <QPointF>
#include <QSizeF>
#include <QVariantMap>

class WidgetNode : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString variableName READ variableName WRITE setVariableName NOTIFY variableNameChanged)
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString tooltip READ tooltip WRITE setTooltip NOTIFY tooltipChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(qreal borderRadius READ borderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged)
    Q_PROPERTY(bool visible READ visibleNode WRITE setVisibleNode NOTIFY visibleChanged)

public:
    explicit WidgetNode(const QString& type, QObject* parent = nullptr);

    QString type() const { return m_type; }
    qreal x() const { return m_x; }
    qreal y() const { return m_y; }
    qreal width() const { return m_width; }
    qreal height() const { return m_height; }
    QString text() const { return m_text; }
    QString variableName() const { return m_variableName; }
    QString color() const { return m_color; }
    qreal fontSize() const { return m_fontSize; }
    bool enabled() const { return m_enabled; }
    QString tooltip() const { return m_tooltip; }
    qreal opacity() const { return m_opacity; }
    qreal borderRadius() const { return m_borderRadius; }
    bool visibleNode() const { return m_visible; }

    void setX(qreal v);
    void setY(qreal v);
    void setWidth(qreal v);
    void setHeight(qreal v);
    void setText(const QString& v);
    void setVariableName(const QString& v);
    void setColor(const QString& v);
    void setFontSize(qreal v);
    void setEnabled(bool v);
    void setTooltip(const QString& v);
    void setOpacity(qreal v);
    void setBorderRadius(qreal v);
    void setVisibleNode(bool v);

    QVariantMap toMap() const;
    static WidgetNode* fromMap(const QVariantMap& map, QObject* parent = nullptr);

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void textChanged();
    void variableNameChanged();
    void colorChanged();
    void fontSizeChanged();
    void enabledChanged();
    void tooltipChanged();
    void opacityChanged();
    void borderRadiusChanged();
    void visibleChanged();

private:
    QString m_type;
    qreal m_x = 0;
    qreal m_y = 0;
    qreal m_width = 120;
    qreal m_height = 40;
    QString m_text;
    QString m_variableName;
    QString m_color = "#14B8A6";
    qreal m_fontSize = 13;
    bool m_enabled = true;
    QString m_tooltip;
    qreal m_opacity = 1.0;
    qreal m_borderRadius = 4;
    bool m_visible = true;
};
