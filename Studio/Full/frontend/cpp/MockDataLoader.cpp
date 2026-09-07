#include "MockDataLoader.hpp"

static const char* KEY_FIRST_RUN = "mock/loaded";
static const char* KEY_CHALLENGES = "mock/challenges";

MockDataLoader::MockDataLoader(QObject *parent)
    : QObject(parent)
{
    m_isFirstRun = !m_settings.value(KEY_FIRST_RUN, false).toBool();
}

void MockDataLoader::loadIfEmpty()
{
    if (m_isFirstRun) {
        loadMockData();
        markDataLoaded();
        m_isFirstRun = false;
        emit firstRunChanged();
    } else {
        QVariantList saved = m_settings.value(KEY_CHALLENGES).toList();
        if (!saved.isEmpty()) {
            m_challenges.clear();
            for (const QVariant &v : saved) {
                m_challenges.append(v);
            }
            emit mockChallengesChanged();
        }
    }
}

void MockDataLoader::loadMockData()
{
    m_challenges.clear();

    // Challenge 1: Basic Print
    QVariantMap c1;
    c1[QString::fromUtf8("id")] = QString::fromUtf8("mock-001");
    c1[QString::fromUtf8("title")] = QString::fromUtf8("\u0637\u0628\u0627\u0639\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628");
    c1[QString::fromUtf8("description")] = QString::fromUtf8("\u0627\u0643\u062A\u0628 \u0628\u0631\u0646\u0627\u0645\u062C \u0636\u0627\u0626\u0639 \u064a\u0637\u0628\u0639 \u062A\u0631\u062D\u064A\u0628\u0627\u064B \u0625\u0644\u0649 \u0627\u0644\u0634\u0627\u0634\u0629");
    c1[QString::fromUtf8("difficulty")] = QString::fromUtf8("EASY");
    c1[QString::fromUtf8("starterCode")] = QString::fromUtf8("\u062F\u0627\u0631\u0629 \u062A\u062C\u0631\u064A\u0628\u064A\u0629\n  \u0627\u0637\u0628\u0639(\u0645\u0631\u062D\u0628\u0627\u064B \u0628\u0627\u0644\u0639\u0627\u0644\u0645)\n\u0646\u0647\u0627\u064A\u0629");
    c1[QString::fromUtf8("expectedOutput")] = QString::fromUtf8("\u0645\u0631\u062D\u0628\u0627\u064B \u0628\u0627\u0644\u0639\u0627\u0644\u0645");
    c1[QString::fromUtf8("type")] = QString::fromUtf8("mock");
    m_challenges.append(c1);

    // Challenge 2: For Loop
    QVariantMap c2;
    c2[QString::fromUtf8("id")] = QString::fromUtf8("mock-002");
    c2[QString::fromUtf8("title")] = QString::fromUtf8("\u062D\u0644\u0642\u0629 \u062A\u0643\u0631\u0627\u0631");
    c2[QString::fromUtf8("description")] = QString::fromUtf8("\u0627\u0633\u062A\u062E\u062F\u0645 \u062D\u0644\u0642\u0629 \u062A\u0643\u0631\u0627\u0631 \u0644\u0637\u0628\u0627\u0639\u0629 \u0627\u0644\u0623\u0631\u0642\u0627\u0645 \u0645\u0646 1 \u0625\u0644\u0649 5");
    c2[QString::fromUtf8("difficulty")] = QString::fromUtf8("MEDIUM");
    c2[QString::fromUtf8("starterCode")] = QString::fromUtf8("\u062F\u0627\u0631\u0629 \u062A\u062C\u0631\u064A\u0628\u064A\u0629\n  \u0645\u0646 \u0633\u0631 \u0627\u0644\u0631\u0642\u0645 1 \u0625\u0644\u0649 5\n    \u0627\u0637\u0628\u0639(\u0627\u0644\u0631\u0642\u0645)\n  \u0646\u0647\u0627\u064A\u0629\n\u0646\u0647\u0627\u064A\u0629");
    c2[QString::fromUtf8("expectedOutput")] = QString::fromUtf8("1\n2\n3\n4\n5");
    c2[QString::fromUtf8("type")] = QString::fromUtf8("mock");
    m_challenges.append(c2);

    // Challenge 3: Math - Area of Square
    QVariantMap c3;
    c3[QString::fromUtf8("id")] = QString::fromUtf8("mock-003");
    c3[QString::fromUtf8("title")] = QString::fromUtf8("\u062D\u0633\u0627\u0628 \u0645\u0633\u0627\u062D\u0629 \u0627\u0644\u0645\u0631\u0628\u0639");
    c3[QString::fromUtf8("description")] = QString::fromUtf8("\u0627\u0643\u062A\u0628 \u0628\u0631\u0646\u0627\u0645\u062C \u064a\u062D\u0633\u0628 \u0645\u0633\u0627\u062D\u0629 \u0645\u0631\u0628\u0639 \u0628\u062D\u0630\u0641 \u0627\u0644\u0637\u0648\u0644");
    c3[QString::fromUtf8("difficulty")] = QString::fromUtf8("MEDIUM");
    c3[QString::fromUtf8("starterCode")] = QString::fromUtf8("\u062F\u0627\u0631\u0629 \u062A\u062C\u0631\u064A\u0628\u064A\u0629\n  \u0645\u062A\u0636\u0627\u0631 \u0627\u0644\u0637\u0648\u0644 = 5\n  \u0645\u0633\u0627\u062D\u0629 \u0627\u0644\u0645\u0631\u0628\u0639 = \u0627\u0644\u0637\u0648\u0644 * \u0627\u0644\u0637\u0648\u0644\n  \u0627\u0637\u0628\u0639(\u0645\u0633\u0627\u062D\u0629 \u0627\u0644\u0645\u0631\u0628\u0639)\n\u0646\u0647\u0627\u064A\u0629");
    c3[QString::fromUtf8("expectedOutput")] = QString::fromUtf8("25");
    c3[QString::fromUtf8("type")] = QString::fromUtf8("mock");
    m_challenges.append(c3);

    // Save to settings
    QVariantList savedList;
    for (const QVariant &c : m_challenges) {
        savedList.append(c);
    }
    m_settings.setValue(KEY_CHALLENGES, savedList);

    emit mockChallengesChanged();
    emit mockDataLoaded(m_challenges.size());
}

void MockDataLoader::markDataLoaded()
{
    m_settings.setValue(KEY_FIRST_RUN, true);
}

QVariantList MockDataLoader::getChallengesForStudent() const
{
    return m_challenges;
}

QVariantList MockDataLoader::getChallengesForTeacher() const
{
    return m_challenges;
}
