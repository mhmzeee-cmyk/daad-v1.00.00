#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QSettings>

class MockDataLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isFirstRun READ isFirstRun NOTIFY firstRunChanged)
    Q_PROPERTY(QVariantList mockChallenges READ mockChallenges NOTIFY mockChallengesChanged)

public:
    explicit MockDataLoader(QObject *parent = nullptr);

    bool isFirstRun() const { return m_isFirstRun; }
    QVariantList mockChallenges() const { return m_challenges; }

    Q_INVOKABLE void loadIfEmpty();
    Q_INVOKABLE QVariantList getChallengesForStudent() const;
    Q_INVOKABLE QVariantList getChallengesForTeacher() const;

signals:
    void firstRunChanged();
    void mockChallengesChanged();
    void mockDataLoaded(int count);

private:
    void loadMockData();
    void markDataLoaded();

    QSettings m_settings;
    bool m_isFirstRun = false;
    QVariantList m_challenges;
};
