#pragma once

#include <QTreeView>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

class DaadFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit DaadFilterProxy(QObject* parent = nullptr);
protected:
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
};

// FileTreeWidget — شجرة VS Code-like لمشروع ض — RTL
class FileTreeWidget : public QTreeView {
    Q_OBJECT
public:
    explicit FileTreeWidget(QWidget* parent = nullptr);
    void setProjectRoot(const QString& path);
    QString projectRoot() const { return m_root; }

signals:
    void fileDoubleClicked(const QString& filePath);

private slots:
    void onDoubleClicked(const QModelIndex& idx);

private:
    QFileSystemModel* m_model = nullptr;
    DaadFilterProxy* m_proxy = nullptr;
    QString m_root;
};
