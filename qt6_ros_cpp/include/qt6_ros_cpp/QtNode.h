#pragma once
#include <QObject>
#include <QImage>
#include <ros/ros.h>
#include <sensor_msgs/CompressedImage.h>
#include <std_msgs/String.h>
#include <string>

class QtNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int windowWidth READ windowWidth CONSTANT)
    Q_PROPERTY(int windowHeight READ windowHeight CONSTANT)
    Q_PROPERTY(int imageX READ imageX CONSTANT)
    Q_PROPERTY(int imageY READ imageY CONSTANT)
    Q_PROPERTY(int imageWidth READ imageWidth CONSTANT)
    Q_PROPERTY(int imageHeight READ imageHeight CONSTANT)
public:
    explicit QtNode(QObject* parent = nullptr);
    void start();
    void stop();

    int windowWidth() const { return window_width_; }
    int windowHeight() const { return window_height_; }
    int imageX() const { return image_x_; }
    int imageY() const { return image_y_; }
    int imageWidth() const { return image_width_; }
    int imageHeight() const { return image_height_; }
    std::string qmlFile() const { return qml_file_; }

signals:
    void updateImage(const QImage& image);
    void updateMessage(const QString& message);

private:
    void compressedImageCallback(const sensor_msgs::CompressedImage::ConstPtr& msg);
    void stringCallback(const std_msgs::String::ConstPtr& msg);
    int clampParam(const std::string& name, int value, int min_value, int max_value);

    ros::NodeHandle nh_;
    ros::NodeHandle pnh_;
    ros::Subscriber image_sub_;
    ros::Subscriber string_sub_;
    ros::AsyncSpinner spinner_;

    int window_width_;
    int window_height_;
    int image_x_;
    int image_y_;
    int image_width_;
    int image_height_;
    std::string qml_file_;
};
