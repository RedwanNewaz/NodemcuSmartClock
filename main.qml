import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQml 2.12


Window {
    width: 1080
    height: 2160
    visible: true
    title: qsTr("Azan Clock V5")
    Image {
         source: "resources/backend.png"
         width: parent.width
         height: parent.height
//         scale: Qt.KeepAspectRatio
    }
    Rectangle
    {
        id: xiaomi
        property int offsetX: 10
        property int offsetY: 20
        property int offsetY1: 10
    }

    // add setting functionality
    Rectangle
    {
        id: settings
        width: parent.width /5
        height: parent.height /8
        x: 10 + xiaomi.offsetX
        y: 10 + xiaomi.offsetY
        color: "transparent"
        Dialog {
            visible: false
            title: "MQTT Settings"
            id: confMqtt
            property string ipAddr: "192.168.1.1"
            property string port: "1883"
            width: 300
            height: 300

            onRejected: console.log("Cancel clicked")
            standardButtons: Dialog.Ok | Dialog.Cancel

            Rectangle
            {
                color: "lightskyblue"
                width: parent.width
                height: parent.height
                Label{
                    text: "<b> IP Address: </b>"
                }
                TextInput {
                    x: 50
                    y: 30
                    id: ipAddr
                    text: backend.ipAddr
                    color: "white"
                    cursorVisible: true
                }
                Label{
                    x: 0
                    y: 60
                    text: "<b>Port: </b>"
                }
                TextInput {
                    x: 50
                    y: 90
                    id: port
                    text: backend.portAddr
                    color: "white"
                    cursorVisible: true
                }
            }

            onAccepted:{
                console.log("Ok clicked " + ipAddr.text);
                backend.ipAddr = ipAddr.text;
                backend.port = port.text;
            }
        }

        MouseArea {
              anchors.fill: parent
              onClicked: {
                console.log("mouse clicked");
                  confMqtt.visible = true;
              }
          }
    }

    // add infotainment functionality
    Rectangle{
        Text {
            // 280 0.4286
            x: parent.width + 120 + xiaomi.offsetX
            y: parent.height + 150 + xiaomi.offsetY1
            text: {
                const zeroPad = (num, places) => String(num).padStart(places, '0');
                zeroPad(backend.hourClock, 2)
            }

            font.family: "Helvetica"
            font.pointSize: 50
            color: "white"
        }

        Text {
            // 410 140
            x: parent.width + 220 + xiaomi.offsetX
            y: parent.height + 150 + xiaomi.offsetY1
            text: {
                const zeroPad = (num, places) => String(num).padStart(places, '0');
                zeroPad(backend.minuteClock, 2)
            }
            font.family: "Helvetica"
            font.pointSize: 50
            color: "white"
        }

        // add alarm time
        Text {
            x: parent.width + 150 + xiaomi.offsetX
            y: parent.height + 290 + xiaomi.offsetY1
            text: {
                const zeroPad = (num, places) => String(num).padStart(places, '0');
                zeroPad(backend.hourAlarm, 2)
            }
            font.family: "Helvetica"
            font.pointSize: 30
            color: "white"
        }
        Text {
            x: parent.width + 210 + xiaomi.offsetX
            y: parent.height + 290 + xiaomi.offsetY1
            text: {
                const zeroPad = (num, places) => String(num).padStart(places, '0');
                zeroPad(backend.minuteAlarm, 2)
            }
            font.family: "Helvetica"
            font.pointSize: 30
            color: "white"
        }
    }

    // add azan info
    Rectangle{
        id: azanInfo
        property string  azanTime: "06:35"
        property string  azanName: "Asr"
        Text {
            x: parent.width + 12 + xiaomi.offsetX
            y: parent.height + 600 + xiaomi.offsetY
            text: backend.azanTime
            font.family: "Helvetica"
            font.pointSize: 75
            color: "white"
        }
        Text {
            x: parent.width + 250 + xiaomi.offsetX
            y: parent.height + 710 + xiaomi.offsetY
            text: backend.azanName
            font.family: "Helvetica"
            font.pointSize: 35
            color: "green"
        }
    }

    // set alarm
    Rectangle{
        id: setAlarm
        x: 90 + xiaomi.offsetX
        y: 290 + xiaomi.offsetY
        property string hourVal: "00"
        property string minuteVal: "00"

        TextInput {
            x: 0
            y: 95
            id: setMinutes
            text: "00"
            color: "navy"
            cursorVisible: false
            font.pointSize: 50
        }
        TextInput {
            x: 0
            y: 190
            id: setHours
            text: "00"
            color: "navy"
            cursorVisible: false
            font.pointSize: 50
        }


        Dialog {
            visible: false
            title: "Alarm Settings"
            id: confAlarm
            width: 300
            height: 300
            standardButtons: Dialog.Ok | Dialog.Cancel
            Label{
                x: 50
                y: 20
                text: "<b> Hours: " + setHours.text + "</b>"
                font.pointSize: 20
            }


            Label{
                x: 50
                y: 90
                text: "<b> Minutes: " + setMinutes.text +"</b>"
                font.pointSize: 20
            }

            onAccepted:{
                console.log("Alarm clicked");
                backend.hourVal = setHours.text;
                backend.minuteVal = setMinutes.text;
                backend.sendAlarm = setHours.text + ":" + setMinutes.text;
            }

            onRejected:
            {
                console.log("Alarm clicked");
                backend.hourVal = "00";
                backend.minuteVal = "00";
                backend.sendAlarm = "00:00";
            }
        }
        Rectangle
        {

            x: 140 + xiaomi.offsetX
            y: 100 + xiaomi.offsetY
            width: 130
            height: 160
            color: "transparent"
            MouseArea {
                  anchors.fill: parent
                  onClicked: {
                    console.log("alarm clicked");
                      confAlarm.visible = true;
                  }
              }

        }
    }

}
