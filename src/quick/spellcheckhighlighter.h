// SPDX-FileCopyrightText: 2004 Zack Rusin <zack@kde.org>
// SPDX-FileCopyrightText: 2013 Martin Sandsmark <martin.sandsmark@kde.org>
// SPDX-FileCopyrightText: 2013 Aurélien Gâteau <agateau@kde.org>
// SPDX-FileCopyrightText: 2020 Christian Mollekopf <mollekopf@kolabsystems.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QQuickTextDocument>
#include <QSyntaxHighlighter>

class HighlighterPrivate;

class SpellcheckHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    /// This property holds the underneath document from a QML TextEdit.
    Q_PROPERTY(QQuickTextDocument *document READ quickDocument WRITE setQuickDocument NOTIFY documentChanged)

    /// This property holds the current cursor position.
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)

    /// This property holds the start of the selection.
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)

    /// This property holds the end of the selection.
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

    /// This property holds whether the current word under the mouse is misspelled.
    Q_PROPERTY(bool wordIsMisspelled READ wordIsMisspelled NOTIFY wordIsMisspelledChanged)

    /// This property holds the current word under the mouse.
    Q_PROPERTY(QString wordUnderMouse READ wordUnderMouse NOTIFY wordUnderMouseChanged)

    /// This property holds the spell color. By default, it's read.
    Q_PROPERTY(QColor misspelledColor READ misspelledColor WRITE setMisspelledColor NOTIFY misspelledColorChanged)

    /// This property holds the current language used for spell checking.
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY currentLanguageChanged)

    /// This property holds whether a spell checking backend with support for the
    /// \ref currentLanguage was found.
    Q_PROPERTY(bool spellCheckerFound READ spellCheckerFound CONSTANT)

    /// \brief This property holds whether spell checking is enabled.
    ///
    /// If \p active is true then spell checking is enabled; otherwise it
    /// is disabled. Note that you have to disable automatic (de)activation
    /// with \ref automatic before you change the state of spell
    /// checking if you want to persistently enable/disable spell
    /// checking.
    ///
    /// \see automatic
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

    /// This property holds whether spell checking is automatically disabled
    /// if there's too many errors.
    Q_PROPERTY(bool automatic READ automatic WRITE setAutomatic NOTIFY automaticChanged)

    /// This property holds whether the automatic language detection is disabled
    /// overriding the Sonnet global settings.
    Q_PROPERTY(bool autoDetectLanguageDisabled READ autoDetectLanguageDisabled WRITE setAutoDetectLanguageDisabled NOTIFY autoDetectLanguageDisabledChanged)

public:
    SpellcheckHighlighter(QObject *parent = nullptr);

    /// Returns a list of suggested replacements for the given misspelled word.
    /// If the word is not misspelled, the list will be empty.
    ///
    /// \param word the misspelled word
    /// \param max at most this many suggestions will be returned. If this is
    ///            -1, as many suggestions as the spell backend supports will
    ///            be returned.
    /// \return a list of suggested replacements for the word
    Q_INVOKABLE QStringList suggestions(int position, int max = 5);

    /// Ignores the given word. This word will not be marked misspelled for
    /// this session. It will again be marked as misspelled when creating
    /// new highlighters.
    ///
    /// \param word the word which will be ignored
    Q_INVOKABLE void ignoreWord(const QString &word);

    /// Adds the given word permanently to the dictionary. It will never
    /// be marked as misspelled again, even after restarting the application.
    ///
    /// \param word the word which will be added to the dictionary
    Q_INVOKABLE void addWordToDictionary(const QString &word);

    Q_INVOKABLE void replaceWord(const QString &word);

    /// Checks if a given word is marked as misspelled by the highlighter.
    ///
    /// \param word the word to be checked
    /// \return true if the given word is misspelled.
    Q_INVOKABLE bool isWordMisspelled(const QString &word);

    [[nodiscard]] QQuickTextDocument *quickDocument() const;
    void setQuickDocument(QQuickTextDocument *document);
    [[nodiscard]] int cursorPosition() const;
    void setCursorPosition(int position);
    [[nodiscard]] int selectionStart() const;
    void setSelectionStart(int position);
    [[nodiscard]] int selectionEnd() const;
    void setSelectionEnd(int position);
    [[nodiscard]] bool wordIsMisspelled() const;
    [[nodiscard]] QString wordUnderMouse() const;
    [[nodiscard]] bool spellCheckerFound() const;
    [[nodiscard]] QString currentLanguage() const;
    void setActive(bool active);
    [[nodiscard]] bool active() const;
    void setAutomatic(bool automatic);
    [[nodiscard]] bool automatic() const;
    void setAutoDetectLanguageDisabled(bool autoDetectDisabled);
    [[nodiscard]] bool autoDetectLanguageDisabled() const;
    void setMisspelledColor(const QColor &color);
    [[nodiscard]] QColor misspelledColor() const;
    void setQuoteColor(const QColor &color);
    [[nodiscard]] QColor quoteColor() const;

    /// Return true if checker is enabled by default
    bool checkerEnabledByDefault() const;

    /// Set a new @ref QTextDocument for this highlighter to operate on.
    /// \param document the new document to operate on.
    void setDocument(QTextDocument *document);

Q_SIGNALS:
    void documentChanged();
    void cursorPositionChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void wordIsMisspelledChanged();
    void wordUnderMouseChanged();
    void changeCursorPosition(int start, int end);
    void activeChanged();
    void misspelledColorChanged();
    void autoDetectLanguageDisabledChanged();
    void automaticChanged();
    void currentLanguageChanged();

    /// Emitted when as-you-type spell checking is enabled or disabled.
    ///
    /// \param description is a i18n description of the new state,
    ///        with an optional reason
    void activeChanged(const QString &description);

protected:
    void highlightBlock(const QString &text) override;
    virtual void setMisspelled(int start, int count);
    virtual void unsetMisspelled(int start, int count);

    bool intraWordEditing() const;
    void setIntraWordEditing(bool editing);

public Q_SLOTS:
    /**
     * Set language to use for spell checking.
     *
     * @param language the language code for the new language to use.
     */
    void setCurrentLanguage(const QString &language);

    /**
     * Run auto detection, disabling spell checking if too many errors are found.
     */
    void slotAutoDetection();

    /**
     * Force a new highlighting.
     */
    void slotRehighlight();

private:
    [[nodiscard]] QTextCursor textCursor() const;
    [[nodiscard]] QTextDocument *textDocument() const;
    void contentsChange(int pos, int add, int rem);

    void autodetectLanguage(const QString &sentence);

    HighlighterPrivate *const d;
    Q_DISABLE_COPY(SpellcheckHighlighter)
};
