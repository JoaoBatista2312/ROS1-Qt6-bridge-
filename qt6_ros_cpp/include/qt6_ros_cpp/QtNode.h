#pragma once
#include <QObject>
#include <QImage>
#include <ros/ros.h>
#include <sensor_msgs/CompressedImage.h>
#include <std_msgs/String.h>

class QtNode : public QObject
{
    Q_OBJECT
public:
    explicit QtNode(QObject* parent = nullptr);
    void start();
    void stop();

signals:
    void updateImage(const QImage& image);
    void updateMessage(const QString& message);

private:
    void compressedImageCallback(const sensor_msgs::CompressedImage::ConstPtr& msg);
    void stringCallback(const std_msgs::String::ConstPtr& msg);

    ros::NodeHandle nh_;
    ros::Subscriber image_sub_;
    ros::Subscriber string_sub_;
    ros::AsyncSpinner spinner_;
};
