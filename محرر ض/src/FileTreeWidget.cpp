#include "FileTreeWidget.h"
#include <QFileInfo>
#include <QDir>

DaadFilterProxy::DaadFilterProxy(QObject* parent) : QSortFilterProxyModel(parent) {}

bool DaadFilterProxy::filterAcceptsRow(int row, const QModelIndex& parent) const {
    QModelIndex idx = sourceModel()->index(row, 0, parent);
    if (!idx.isValid()) return false;
    QFileSystemModel* fs = qobject_cast<QFileSystemModel*>(sourceModel());
    if (!fs) return true;
    QFileInfo info = fs->fileInfo(idx);
    QString name = info.fileName();
    // إخفاء build, .git, .daadproject مؤقتاً
    if (name == "build" || name == ".git" || name == ".vscode" || name == "build-test") return false;
    if (info.isDir()) return true;
    // إظهار فقط .ض و .daad و .ضasm و .daadproject
    if (name.endsWith(".ض") || name.endsWith(".daad") || name.endsWith(".ضasm") || name == ".daadproject") return true;
    // إخفاء الباقي لكن إبقاء المجلدات
    // للمرحلة 2: إظهار كل الملفات مع تفضيل .ض — نبقي الكل لتجنب إخفاء ملفات المشروع
    // نعيد true لل smoth
    return false;
}

FileTreeWidget::FileTreeWidget(QWidget* parent) : QTreeView(parent) {
    m_model = new QFileSystemModel(this);
    m_model->setRootPath("");
    m_model->setNameFilters(QStringList() << "*.ض" << "*.daad" << "*.ضasm");
    m_model->setNameFilterDisables(false);
    m_model->setOption(QFileSystemModel::DontWatchForChanges, false);

    m_proxy = new DaadFilterProxy(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseSensitive);

    setModel(m_proxy);
    setHeaderHidden(true);
    setLayoutDirection(Qt::RightToLeft);
    setAnimated(true);
    setIndentation(18);
    // إظهار عمود الاسم فقط
    for (int i = 1; i < m_model->columnCount(); ++i) hideColumn(i);

    connect(this, &QTreeView::doubleClicked, this, &FileTreeWidget::onDoubleClicked);
}

void FileTreeWidget::setProjectRoot(const QString& path) {
    m_root = path;
    QModelIndex srcIdx = m_model->index(path);
    QModelIndex proxyIdx = m_proxy->mapFromSource(srcIdx);
    setRootIndex(proxyIdx);
    expand(proxyIdx);
}

void FileTreeWidget::onDoubleClicked(const QModelIndex& idx) {
    QModelIndex src = m_proxy->mapToSource(idx);
    QFileInfo info = m_model->fileInfo(src);
    if (info.isFile()) emit fileDoubleClicked(info.absoluteFilePath());
}
