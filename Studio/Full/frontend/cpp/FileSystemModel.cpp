/**
 * @file FileSystemModel.cpp
 * @brief تنفيذ نموذج نظام الملفات
 */

#include "FileSystemModel.hpp"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include <algorithm>

static QString normalizePath(const QString& path) {
    QString p = path;
    if (p.startsWith("file://")) {
        p = QUrl(p).toLocalFile();
    }
    p = QDir::cleanPath(p);
    return p;
}

namespace daad {

FileSystemModel::FileSystemModel(QObject* parent)
    : QObject(parent)
    , m_watcher(std::make_unique<QFileSystemWatcher>(this))
{
    connect(m_watcher.get(), &QFileSystemWatcher::directoryChanged,
            this, &FileSystemModel::refresh);
}

void FileSystemModel::setRootPath(const QString& path) {
    QString normalizedPath = normalizePath(path);
    if (m_rootPath == normalizedPath) return;

    if (!m_rootPath.isEmpty()) {
        m_watcher->removePath(m_rootPath);
    }

    m_rootPath = normalizedPath;
    if (!normalizedPath.isEmpty()) {
        m_watcher->addPath(normalizedPath);
    }

    scanDirectory(normalizedPath);
    emit rootPathChanged();
    emit folderChanged();
}

void FileSystemModel::setCurrentFile(const QString& path) {
    if (m_currentFile == path) return;
    m_currentFile = path;
    emit currentFileChanged();
}

void FileSystemModel::openFolder() {
    // This will be called from QML - uses platform path
}

void FileSystemModel::openFile(const QString& path) {
    QString filePath = normalizePath(path);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred("\u0644\u0627 \u064A\u0645\u0643\u0646 \u0641\u062A\u062D \u0627\u0644\u0645\u0644\u0641: " + file.errorString());
        return;
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    m_currentFile = filePath;
    emit currentFileChanged();
    emit fileOpened(filePath, content);
}

bool FileSystemModel::saveFile(const QString& path, const QString& content) {
    QString filePath = normalizePath(path);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("لا يمكن حفظ الملف: " + file.errorString());
        return false;
    }

    QTextStream stream(&file);
    stream << content;
    file.close();

    m_hasUnsaved = false;
    emit hasUnsavedChangesChanged();
    return true;
}

bool FileSystemModel::saveCurrentFile(const QString& content) {
    if (m_currentFile.isEmpty()) return false;
    return saveFile(m_currentFile, content);
}

QString FileSystemModel::readFile(const QString& path) {
    QString filePath = normalizePath(path);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    return content;
}

QVariantList FileSystemModel::getFolderContents(const QString& path) {
    QVariantList result;
    QString cleanPath = normalizePath(path);
    QDir dir(cleanPath);

    if (!dir.exists()) return result;

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    for (const QFileInfo& info : entries) {
        QVariantMap item;
        item["name"] = info.fileName();
        item["path"] = info.absoluteFilePath();
        item["isDir"] = info.isDir();
        item["size"] = info.size();
        item["modified"] = info.lastModified().toString("yyyy-MM-dd HH:mm");

        if (info.isDir()) {
            item["icon"] = QStringLiteral("\U0001F4C1");
            item["type"] = "folder";
        } else {
            QString ext = info.suffix().toLower();
            if (ext == "daad" || ext == QString::fromUtf8("\u0636"))
                item["icon"] = QStringLiteral("\U0001F4DD");
            else if (ext == "cpp" || ext == "hpp" || ext == "h")
                item["icon"] = QStringLiteral("\U0001F4BB");
            else
                item["icon"] = QStringLiteral("\U0001F4C4");
            item["type"] = "file";
        }

        result.append(item);
    }

    return result;
}

void FileSystemModel::scanDirectory(const QString& path) {
    m_files = getFolderContents(path);
    emit filesChanged();
}

void FileSystemModel::createNewFile(const QString& dirPath, const QString& fileName) {
    QDir dir(dirPath);
    QString filePath = dirPath + "/" + fileName;

    QFile file(filePath);
    if (file.exists()) {
        emit errorOccurred("الملف موجود مسبقاً");
        return;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("لا يمكن إنشاء الملف");
        return;
    }

    file.close();
    scanDirectory(dirPath);
    emit folderChanged();
}

void FileSystemModel::createNewFolder(const QString& dirPath, const QString& folderName) {
    QDir dir(dirPath);
    if (!dir.mkdir(folderName)) {
        emit errorOccurred("لا يمكن إنشاء المجلد");
        return;
    }

    scanDirectory(dirPath);
    emit folderChanged();
}

void FileSystemModel::deleteItem(const QString& path) {
    QFileInfo info(path);

    if (info.isDir()) {
        QDir dir(path);
        if (!dir.removeRecursively()) {
            emit errorOccurred("لا يمكن حذف المجلد");
            return;
        }
    } else {
        if (!QFile::remove(path)) {
            emit errorOccurred("لا يمكن حذف الملف");
            return;
        }
    }

    scanDirectory(info.absolutePath());
    emit folderChanged();
}

void FileSystemModel::renameItem(const QString& oldPath, const QString& newName) {
    QFileInfo info(oldPath);
    QString newPath = info.absolutePath() + "/" + newName;

    if (QFileInfo::exists(newPath)) {
        emit errorOccurred("الاسم موجود مسبقاً");
        return;
    }

    if (!QFile::rename(oldPath, newPath)) {
        emit errorOccurred("لا يمكن تغيير الاسم");
        return;
    }

    if (m_currentFile == oldPath) {
        m_currentFile = newPath;
        emit currentFileChanged();
    }

    scanDirectory(info.absolutePath());
    emit folderChanged();
}

void FileSystemModel::refresh() {
    if (!m_rootPath.isEmpty()) {
        scanDirectory(m_rootPath);
    }
}

} // namespace daad
