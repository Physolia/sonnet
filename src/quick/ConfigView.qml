// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.sonnet 1.0 as Sonnet

Kirigami.Page {
    id: page

    /**
     * This property holds whether the setting on that page are automatically
     * applied or whether the user can apply then manually. By default, false.
     */
    property bool instantApply: false

    /**
     * This property holds whether the ListViews inside the page should get
     * extra padding and a background. By default, use the Kirigami.ApplicationWindow
     * wideMode value.
     */
    property bool wideMode: applicationWindow().wideMode
    onWideModeChanged: {
        scroll.background.visible = wideMode
        scroll2.background.visible = wideMode
    }

    leftPadding: pageSettingStack.wideMode ? Kirigami.Units.gridUnit : 0
    topPadding: pageSettingStack.wideMode ? Kirigami.Units.gridUnit : 0
    bottomPadding: pageSettingStack.wideMode ? Kirigami.Units.gridUnit : 0
    rightPadding: pageSettingStack.wideMode ? Kirigami.Units.gridUnit : 0

    Sonnet.Settings {
        id: settings
    }

    ColumnLayout {
        anchors.fill: parent

        Kirigami.FormLayout {
            Layout.fillWidth: true

            QQC2.CheckBox {
                checked: settings.autodetectLanguage
                text: i18n("Enable autodection of language")
                onCheckedChanged: {
                    settings.autodetectLanguage = checked;
                    if (instantApply) {
                        settings.save();
                    }
                }
            }

            QQC2.CheckBox {
                checked: settings.checkerEnabledByDefault
                text: i18n("Automatic spell checking enabled by default")
                onCheckedChanged: {
                    settings.checkerEnabledByDefault = checked;
                    if (instantApply) {
                        settings.save();
                    }
                }
            }

            QQC2.CheckBox {
                checked: settings.skipUppercase
                text: i18n("Skip all uppercase words")
                onCheckedChanged: {
                    settings.skipUppercase = checked;
                    if (instantApply) {
                        settings.save();
                    }
                }
            }

            QQC2.CheckBox {
                checked: settings.skipRunTogether
                text: i18n("Skip run-together words")
                onCheckedChanged: {
                    settings.skipRunTogether = checked;
                    if (instantApply) {
                        settings.save();
                    }
                }
            }
            QQC2.Button {
                text: i18n("Open Personal Dictionary")
                onClicked: dictionarySheet.open()
            }
            Item {
                Kirigami.FormData.label: i18n("Preferred languages")
                Kirigami.FormData.isSection: true
            }
        }

        QQC2.ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            Component.onCompleted: background.visible = wideMode
            ListView {
                clip: true
                model: settings.dictionaryModel
                delegate: Kirigami.CheckableListItem {
                    label: model.display
                    checked: model.checked
                    onCheckedChanged: model.checked = checked
                    trailing: QQC2.ToolButton {
                        icon.name: "favorite"
                        checked: model.isDefault
                        checkable: true
                        onCheckedChanged: model.isDefault = true;
                        enabled: !model.isDefault && model.checked
                        QQC2.ToolTip {
                            text: i18n("Make Default Language")
                        }
                    }
                }
            }
        }
    }

    Kirigami.OverlaySheet {
        id: dictionarySheet
        header: RowLayout {
            QQC2.TextField {
                Layout.fillWidth: true
                placeholderText: i18n("Add word to personal dictionary")
            }
            QQC2.Button {
                text: i18n("Add")
                icon.name: "list-add"
                Layout.rightMargin: Kirigami.Units.largeSpacing
            }
        }

        ListView {
            implicitWidth: Kirigami.Units.gridUnit * 15
            model: settings.currentIgnoreList
            delegate: Kirigami.BasicListItem {
                label: modelData
                trailing: QQC2.ToolButton {
                    icon.name: "delete"
                    QQC2.ToolTip {
                        text: i18n("Delete word")
                    }
                }
            }
        }
    }

    footer: QQC2.ToolBar {
        visible: !instantApply
        height: visible ? implicitHeight : 0
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button  {
                text: i18nc("@action:button", "Apply")
                enabled: settings.modified
                onClicked: settings.save();
            }
        }
    }

}
