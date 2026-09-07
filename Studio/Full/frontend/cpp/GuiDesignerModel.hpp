#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QList>
#include <QVariantMap>
#include "WidgetNode.hpp"

class GuiDesignerModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
    Q_PROPERTY(QString generatedCode READ generatedCode NOTIFY generatedCodeChanged)

public:
    enum Roles { WidgetObjectRole = Qt::UserRole + 1 };

    explicit GuiDesignerModel(QObject* parent = nullptr);
    ~GuiDesignerModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    int selectedIndex() const;
    void setSelectedIndex(int index);
    QString generatedCode() const;

    Q_INVOKABLE int addWidget(const QString& type, qreal x, qreal y);
    Q_INVOKABLE void removeWidget(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantMap getWidget(int index) const;
    Q_INVOKABLE void updateWidgetProperty(int index, const QString& prop, const QVariant& value);
    Q_INVOKABLE void importFromCode(const QString& code);
    Q_INVOKABLE QString daadTypeKeyword(const QString& visualType) const;

signals:
    void countChanged();
    void selectedIndexChanged();
    void generatedCodeChanged();
    void designChanged();

private:
    void regenerateCode();
    QList<WidgetNode*> m_widgets;
    int m_selectedIndex = -1;
    QString m_generatedCode;
};
