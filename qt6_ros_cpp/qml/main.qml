import QtQuick 6.0
import QtQuick.Window 6.0
import QtQuick.Controls 6.0

Window {
    visible: true
    width: guiConfig.windowWidth
    height: guiConfig.windowHeight
    title: appConnection.rosData
    property double interval: 1
    property real lastRenderTime: 0

    Image {
        id: rosImageDisplay
        x: guiConfig.imageX
        y: guiConfig.imageY
        width: guiConfig.imageWidth
        height: guiConfig.imageHeight
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
                }
            });

            timer.start();
        }
    }
}
