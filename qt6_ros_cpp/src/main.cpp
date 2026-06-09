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

int main(int argc, char* argv[])
{
    ros::init(argc, argv, "qt6_ros_cpp_node", ros::init_options::NoSigintHandler);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto* image_provider = new ImageProvider();
    engine.addImageProvider("myimageprovider", image_provider);
    engine.rootContext()->setContextProperty("imageProvider", image_provider);

    AppConnection connection;
    engine.rootContext()->setContextProperty("appConnection", &connection);

    std::string package_path = ros::package::getPath("qt6_ros_cpp");
    QString qml_file = QString::fromStdString(package_path + "/qml/main.qml");
    engine.load(QUrl::fromLocalFile(qml_file));

    if (engine.rootObjects().isEmpty())
        return -1;

    QtNode ros_node;
    QObject::connect(&ros_node, &QtNode::updateImage, image_provider, &ImageProvider::updateImage);
    QObject::connect(&ros_node, &QtNode::updateMessage, &connection, &AppConnection::getRosData);
    ros_node.start();

    int result = app.exec();

    ros_node.stop();

    return result;
}
