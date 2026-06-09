import QtQuick 6.0
import QtQuick.Window 6.0
import QtQuick.Controls 6.0

Window {
    visible: true
    width: 640
    height: 480
    title: appConnection.rosData
    property int loadCounter: 0
    property double interval: 1
    property int totalLoadingTime: 0
    property double startTime: 0
    property double endTime: 0
    property real lastRenderTime: 0

    Text {
        id: statusText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        font.pixelSize: 20
        color: "black"
        z: 1
    }

    Image {
        id: rosImageDisplay
        anchors.fill: parent
        source: "image://myimageprovider/image?" + imageProvider.imageUpdateCount
        fillMode: Image.PreserveAspectFit
        smooth: false

        onSourceChanged: {
            let startTime = Date.now();

            var timer = Qt.createQmlObject('import QtQuick 6.0; Timer {}', rosImageDisplay);
            timer.interval = 1;
            timer.repeat = true;

            timer.onTriggered.connect(() => {
                if (rosImageDisplay.width > 0 && rosImageDisplay.height > 0) {
                    timer.stop();
                    let endTime = Date.now();
                    lastRenderTime = endTime - startTime - interval;
                    console.log("Image rendered in: " + lastRenderTime + " ms");
                    statusText.text = "Last Render Time: " + lastRenderTime.toFixed(2) + " ms";
                }
            });

            timer.start();
        }
    }
}
