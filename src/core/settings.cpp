/*
 * SPDX-FileCopyrightText: 2020 Benjamin Port <benjamin.port@enioka.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include "settingsimpl_p.h"

#include <QAbstractListModel>
#include <QLocale>

#include "loader_p.h"
#include "settings.h"
#include <QDebug>
#include <speller.h>

namespace Sonnet
{
class DictionaryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DictionaryModel(QObject *parent = nullptr)
        : QAbstractListModel(parent)
    {
        reload();
    }

    ~DictionaryModel()
    {
    }

    void reload()
    {
        beginResetModel();
        Sonnet::Speller speller;
        m_preferredDictionaries = speller.preferredDictionaries();
        m_availableDictionaries = speller.availableDictionaries();
        QMapIterator<QString, QString> i(m_preferredDictionaries);
        while (i.hasNext()) {
            i.next();
            m_availableDictionaries.remove(i.key());
        }
        endResetModel();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }

        const bool inPreferredDictionaries = index.row() < m_preferredDictionaries.count();
        const int row = inPreferredDictionaries ? index.row() : index.row() - m_preferredDictionaries.count();

        switch (role) {
        case Qt::DisplayRole:
            return inPreferredDictionaries ? m_preferredDictionaries.keys().at(row) : m_availableDictionaries.keys().at(row);
        case Settings::LanguageCodeRole:
            return inPreferredDictionaries ? m_preferredDictionaries.values().at(row) : m_availableDictionaries.values().at(row);
        case Qt::CheckStateRole:
        case Settings::PreferredRole:
            return inPreferredDictionaries;
        case Settings::DefaultRole:
            return data(index, Settings::LanguageCodeRole) == m_defaultDictionary;
        }
        return {};
    }

    int rowCount(const QModelIndex &parent) const override
    {
        Q_UNUSED(parent)
        return m_preferredDictionaries.count() + m_availableDictionaries.count();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return {{Qt::DisplayRole, QByteArrayLiteral("display")},
                {Qt::CheckStateRole, QByteArrayLiteral("checked")},
                {Settings::PreferredRole, QByteArrayLiteral("isPreferred")},
                {Settings::LanguageCodeRole, QByteArrayLiteral("languageCode")},
                {Settings::DefaultRole, QByteArrayLiteral("isDefault")}};
    }

private:
    QMap<QString, QString> m_preferredDictionaries;
    QMap<QString, QString> m_availableDictionaries;
    const QString m_defaultDictionary;
};

class SettingsPrivate
{
public:
    Loader *loader = nullptr;
    DictionaryModel *dictionaryModel = nullptr;
};

Settings::Settings(QObject *parent)
    : QObject(parent)
    , d(new SettingsPrivate)
{
    d->loader = Loader::openLoader();
}

Settings::~Settings()
{
    delete d;
}

void Settings::setDefaultLanguage(const QString &lang)
{
    if (defaultLanguage() == lang) {
        return;
    }
    d->loader->settings()->setDefaultLanguage(lang);
    Q_EMIT defaultLanguageChanged();
    Q_EMIT modifiedChanged();
}

QString Settings::defaultLanguage() const
{
    return d->loader->settings()->defaultLanguage();
}

void Settings::setPreferredLanguages(const QStringList &lang)
{
    if (preferredLanguages() == lang) {
        return;
    }
    d->loader->settings()->setPreferredLanguages(lang);
    Q_EMIT preferredLanguagesChanged();
    Q_EMIT modifiedChanged();
}

QStringList Settings::preferredLanguages() const
{
    return d->loader->settings()->preferredLanguages();
}

void Settings::setDefaultClient(const QString &client)
{
    if (defaultClient() == client) {
        return;
    }
    d->loader->settings()->setDefaultClient(client);
    Q_EMIT defaultClientChanged();
    Q_EMIT modifiedChanged();
}

QString Settings::defaultClient() const
{
    return d->loader->settings()->defaultClient();
}

void Settings::setSkipUppercase(bool skip)
{
    if (skipUppercase() == skip) {
        return;
    }
    d->loader->settings()->setCheckUppercase(!skip);
    Q_EMIT skipUppercaseChanged();
    Q_EMIT modifiedChanged();
}

bool Settings::skipUppercase() const
{
    return !d->loader->settings()->checkUppercase();
}

void Settings::setAutodetectLanguage(bool detect)
{
    if (autodetectLanguage() == detect) {
        return;
    }
    d->loader->settings()->setAutodetectLanguage(detect);
    Q_EMIT autodetectLanguage();
    Q_EMIT modifiedChanged();
}

bool Settings::autodetectLanguage() const
{
    return d->loader->settings()->autodetectLanguage();
}

void Settings::setSkipRunTogether(bool skip)
{
    if (skipRunTogether() == skip) {
        return;
    }
    d->loader->settings()->setSkipRunTogether(skip);
    Q_EMIT skipRunTogetherChanged();
    Q_EMIT modifiedChanged();
}

bool Settings::skipRunTogether() const
{
    return d->loader->settings()->skipRunTogether();
}

void Settings::setCheckerEnabledByDefault(bool check)
{
    if (checkerEnabledByDefault() == check) {
        return;
    }
    d->loader->settings()->setCheckerEnabledByDefault(check);
    Q_EMIT checkerEnabledByDefaultChanged();
    Q_EMIT modifiedChanged();
}

bool Settings::checkerEnabledByDefault() const
{
    return d->loader->settings()->checkerEnabledByDefault();
}

void Settings::setBackgroundCheckerEnabled(bool enable)
{
    if (backgroundCheckerEnabled() == enable) {
        return;
    }
    d->loader->settings()->setBackgroundCheckerEnabled(enable);
    Q_EMIT backgroundCheckerEnabledChanged();
    Q_EMIT modifiedChanged();
}

bool Settings::backgroundCheckerEnabled() const
{
    return d->loader->settings()->backgroundCheckerEnabled();
}

void Settings::setCurrentIgnoreList(const QStringList &ignores)
{
    if (currentIgnoreList() == ignores) {
        return;
    }
    d->loader->settings()->setCurrentIgnoreList(ignores);
    Q_EMIT currentIgnoreListChanged();
    Q_EMIT modifiedChanged();
}

QStringList Settings::currentIgnoreList() const
{
    return d->loader->settings()->currentIgnoreList();
}

QStringList Settings::clients() const
{
    return d->loader->clients();
}

void Settings::save()
{
    d->loader->settings()->save();
}

bool Settings::modified() const
{
    return d->loader->settings()->modified();
}

// default values
// A static list of KDE specific words that we want to recognize
QStringList Settings::defaultIgnoreList()
{
    QStringList l;
    l.append(QStringLiteral("KMail"));
    l.append(QStringLiteral("KOrganizer"));
    l.append(QStringLiteral("KAddressBook"));
    l.append(QStringLiteral("KHTML"));
    l.append(QStringLiteral("KIO"));
    l.append(QStringLiteral("KJS"));
    l.append(QStringLiteral("Konqueror"));
    l.append(QStringLiteral("Sonnet"));
    l.append(QStringLiteral("Kontact"));
    l.append(QStringLiteral("Qt"));
    l.append(QStringLiteral("Okular"));
    l.append(QStringLiteral("KMix"));
    l.append(QStringLiteral("Amarok"));
    l.append(QStringLiteral("KDevelop"));
    l.append(QStringLiteral("Nepomuk"));
    return l;
}

bool Settings::defaultSkipUppercase()
{
    return false;
}

bool Settings::defaultAutodetectLanguage()
{
    return true;
}

bool Settings::defaultBackgroundCheckerEnabled()
{
    return true;
}

bool Settings::defaultCheckerEnabledByDefault()
{
    return false;
}

bool Settings::defauktSkipRunTogether()
{
    return true;
}

QString Settings::defaultDefaultLanguage()
{
    return QLocale::system().name();
}

QStringList Settings::defaultPreferredLanguages()
{
    return QStringList();
}

QObject *Settings::dictionaryModel()
{
    // Lazy loading
    if (d->dictionaryModel) {
        return d->dictionaryModel;
    }

    d->dictionaryModel = new DictionaryModel(this);
    return d->dictionaryModel;
}
}

#include "settings.moc"
