import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

ApplicationWindow {
    id: mainWin
    visible: true

    title: qsTr("POTalk")

    height: minimumHeight

    minimumWidth: 500; maximumWidth: minimumWidth
    minimumHeight: 95; maximumHeight: minimumHeight

    property bool controlsEnabled: !socket.bound && !socket.lookupInProgress

    RowLayout {
        id: deviceSelection

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5

        ComboBox {
            id: inputDevicesCombo
            enabled: count
            focusPolicy: Qt.NoFocus

            property bool deviceValid: true //assume it is

            Layout.fillWidth: true

            wheelEnabled: true

            model: inputDevices
            textRole: "display"

            onCurrentTextChanged: {
                deviceValid = audioIn.changeDevice(currentText)
            }

            background: Rectangle {
                implicitWidth: 140
                implicitHeight: 40

                color: parent.down ? parent.palette.mid : (parent.deviceValid ? parent.palette.button : "tomato")
                border.color: parent.palette.highlight
                border.width: !parent.editable && parent.visualFocus ? 2 : 0
                visible: !parent.flat || parent.down
            }

            //hide actual control
            MouseArea { anchors.fill: parent }
        }

        ComboBox {
            id: outputDevicesCombo
            enabled: count
            focusPolicy: Qt.NoFocus

            property bool deviceValid: true //assume it is

            Layout.fillWidth: true

            wheelEnabled: true

            model: outputDevices
            textRole: "display"

            onCurrentTextChanged: {
                deviceValid = audioOut.changeDevice(currentText)
            }

            background: Rectangle {
                implicitWidth: 140
                implicitHeight: 40

                color: parent.down ? parent.palette.mid : (parent.deviceValid ? parent.palette.button : "tomato")
                border.color: parent.palette.highlight
                border.width: !parent.editable && parent.visualFocus ? 2 : 0
                visible: !parent.flat || parent.down
            }

            //hide actual control
            MouseArea { anchors.fill: parent }
        }
    }

    RowLayout {
        id: controls

        height: 40

        anchors.top: deviceSelection.bottom;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5

        TextField {
            id: hostTxt
            enabled: controlsEnabled

            Layout.preferredWidth: inputDevicesCombo.width
            Layout.fillHeight: true

            selectByMouse: true

            placeholderText: "localhost"
        }

        TextField {
            id: portTxt
            enabled: controlsEnabled

            Layout.fillWidth: true
            Layout.fillHeight: true

            selectByMouse: true

            placeholderText: "50000"
            validator: RegExpValidator {
                regExp: /^(6553[0-5]|655[0-2]\d|65[0-4]\d\d|6[0-4]\d{3}|5\d{4}|49[2-9]\d\d|491[6-9]\d|4915[2-9])$/
            }
        }

        Switch {
            id: bindSw
            enabled: (!portTxt.length || portTxt.acceptableInput) && !socket.lookupInProgress
            focusPolicy: Qt.NoFocus

            checked: socket.bound

            Layout.preferredWidth: 100
            Layout.fillHeight: true

            //hide actual control
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (socket.bound) {
                        socket.unbind()
                        waveformRecv.updateWaveformSeries("")
                    } else {
                        socket.bind(hostTxt.text || hostTxt.placeholderText,
                                    portTxt.text || portTxt.placeholderText)
                    }
                }
            }
        }
    }

    ColumnLayout {
        id: waveViews
        visible: false

        anchors.top: controls.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        WaveView {
            id: waveViewIn

            Layout.fillWidth: true
            Layout.fillHeight: true

            waveformProvider: waveformIn
            color: "forestgreen"
        }

        WaveView {
            id: waveViewOut

            Layout.fillWidth: true
            Layout.fillHeight: true

            waveformProvider: waveformRecv
            color: "crimson"
        }
    }

    Shortcut {
        sequence: "Ctrl+W"
        onActivated: {
            waveViews.visible ^= 1
            mainWin.minimumHeight = waveViews.visible ? 500 : 95
        }
    }
}
