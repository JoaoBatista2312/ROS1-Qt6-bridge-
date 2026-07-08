#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QString>
#include <ros/ros.h>
#include <ros/package.h>

#include "qt6_ros_cpp/QtNode.h"
#include "qt6_ros_cpp/ImageProvider.h"
#include "qt6_ros_cpp/AppConnection.h"

int main(int argc, char* argv[]) {
    ros::init(argc, argv, "qt6_ros_cpp_node", ros::init_options::NoSigintHandler);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto* image_provider = new ImageProvider();
    engine.addImageProvider("myimageprovider", image_provider);
    engine.rootContext()->setContextProperty("imageProvider", image_provider);

    AppConnection connection;
    engine.rootContext()->setContextProperty("appConnection", &connection);

    QtNode ros_node;
    engine.rootContext()->setContextProperty("guiConfig", &ros_node);
    QObject::connect(&ros_node, &QtNode::updateImage, image_provider, &ImageProvider::updateImage);
    QObject::connect(&ros_node, &QtNode::updateMessage, &connection, &AppConnection::getRosData);

    std::string qml_file_path = ros_node.qmlFile();
    if (qml_file_path.empty()) {
        qml_file_path = ros::package::getPath("qt6_ros_cpp") + "/qml/main.qml";
    }
    engine.load(QUrl::fromLocalFile(QString::fromStdString(qml_file_path)));

    if (engine.rootObjects().isEmpty())
        return -1;

    // Enforce window geometry from code so a custom QML file works without
    // having to bind to guiConfig itself; the only requirement is a Window root.
    QObject* root_object = engine.rootObjects().constFirst();
    root_object->setProperty("width", ros_node.windowWidth());
    root_object->setProperty("height", ros_node.windowHeight());
    root_object->setProperty("visible", true);

    ros_node.start();

    int result = app.exec();

    ros_node.stop();

    return result;
}
