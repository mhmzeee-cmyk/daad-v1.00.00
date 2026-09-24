/**
 * @file FileSystemModel.hpp
 * @brief نموذج نظام الملفات لمستكشف الملفات
 */

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QFileSystemWatcher>
#include <memory>

namespace daad {

class FileSystemModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
    Q_PROPERTY(QVariantList files READ files NOTIFY filesChanged)
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(bool hasUnsavedChanges READ hasUnsavedChanges NOTIFY hasUnsavedChangesChanged)

public:
    explicit FileSystemModel(QObject* parent = nullptr);
    ~FileSystemModel() = default;

    QString rootPath() const { return m_rootPath; }
    void setRootPath(const QString& path);

    QVariantList files() const { return m_files; }

    QString currentFile() const { return m_currentFile; }
    void setCurrentFile(const QString& path);

    bool hasUnsavedChanges() const { return m_hasUnsaved; }

    Q_INVOKABLE void openFolder();
    Q_INVOKABLE void openFile(const QString& path);
    Q_INVOKABLE bool saveFile(const QString& path, const QString& content);
    Q_INVOKABLE bool saveCurrentFile(const QString& content);
    Q_INVOKABLE QString readFile(const QString& path);
    Q_INVOKABLE QVariantList getFolderContents(const QString& path);
    Q_INVOKABLE void createNewFile(const QString& dirPath, const QString& fileName);
    Q_INVOKABLE void createNewFolder(const QString& dirPath, const QString& folderName);
    Q_INVOKABLE void deleteItem(const QString& path);
    Q_INVOKABLE void renameItem(const QString& oldPath, const QString& newName);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void markSaved() { m_hasUnsaved = false; emit hasUnsavedChangesChanged(); }

signals:
    void rootPathChanged();
    void filesChanged();
    void currentFileChanged();
    void hasUnsavedChangesChanged();
    void fileOpened(const QString& path, const QString& content);
    void errorOccurred(const QString& message);
    void folderChanged();

private:
    void scanDirectory(const QString& path);
    QString getFileContent(const QString& path) const;

    QString m_rootPath;
    QString m_currentFile;
    QVariantList m_files;
    bool m_hasUnsaved = false;
    std::unique_ptr<QFileSystemWatcher> m_watcher;
};

} // namespace daad
