#!/usr/bin/env python3

import sys
import os

import rospkg

from PySide6.QtCore import Signal, Slot, QObject, Property, QUrl
from PySide6.QtGui import QImage, QGuiApplication, QColor
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtQuick import QQuickImageProvider

from qt6_ros.QtNode import QtNode


class ImageProvider(QQuickImageProvider):
    imageCounterSignal = Signal()

    def __init__(self):
        super().__init__(QQuickImageProvider.ImageType.Image)
        self.image = QImage()
        self._update_count = 0

    @Slot(QImage)
    def updateImage(self, new_image):
        if isinstance(new_image, QImage):
            self.image = new_image
            self._update_count += 1
            self.imageCounterSignal.emit()
        else:
            print("Received image is not of type QImage.")

    def requestImage(self, id, size, requestedSize):
        if self.image.isNull():
            img = QImage(300, 300, QImage.Format.Format_RGBA8888)
            img.fill(QColor(255, 0, 0))
            print("Image is null, returning red image")
            return img
        return self.image

    @Property(int, notify=imageCounterSignal)
    def imageUpdateCount(self):
        return self._update_count


class MyApplicationConnection(QObject):
    rosDataChanged = Signal()

    def __init__(self):
        super().__init__()
        self._ros_data = "Hello World"

    @Slot(str)
    def getRosData(self, data):
        if data != self._ros_data:
            self._ros_data = data
            self.rosDataChanged.emit()

    @Property(str, notify=rosDataChanged)
    def rosData(self):
        return self._ros_data


def main():
    app = QGuiApplication(sys.argv)
    engine = QQmlApplicationEngine()

    image_provider = ImageProvider()
    engine.addImageProvider("myImageProvider", image_provider)
    engine.rootContext().setContextProperty("imageProvider", image_provider)

    connection = MyApplicationConnection()
    engine.rootContext().setContextProperty("appConnection", connection)

    ros_node = QtNode()
    engine.rootContext().setContextProperty("guiConfig", ros_node)
    ros_node.updateImage.connect(image_provider.updateImage)
    ros_node.updateMessage.connect(connection.getRosData)

    qml_file = ros_node.qml_file
    if not qml_file:
        rospack = rospkg.RosPack()
        package_path = rospack.get_path('qt6_ros')
        qml_file = os.path.join(package_path, "qml/main.qml")
    engine.load(QUrl.fromLocalFile(qml_file))

    if not engine.rootObjects():
        sys.exit(-1)

    # Enforce window geometry from code so a custom QML file works without
    # having to bind to guiConfig itself; the only requirement is a Window root.
    root_object = engine.rootObjects()[0]
    root_object.setProperty("width", ros_node.windowWidth)
    root_object.setProperty("height", ros_node.windowHeight)
    root_object.setProperty("visible", True)

    ros_node.start()

    def cleanup():
        ros_node.stop()
        ros_node.wait()

    app.aboutToQuit.connect(cleanup)
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
