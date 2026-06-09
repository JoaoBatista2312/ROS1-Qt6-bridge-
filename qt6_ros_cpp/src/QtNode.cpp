#include "qt6_ros_cpp/QtNode.h"
#include <opencv2/opencv.hpp>
#include <QtConcurrent/QtConcurrent>
#include <vector>

QtNode::QtNode(QObject* parent)
    : QObject(parent), spinner_(2)
{
    image_sub_ = nh_.subscribe("/camera/color/image_rect_color/compressed", 1,
                               &QtNode::compressedImageCallback, this);
    string_sub_ = nh_.subscribe("chatter", 10, &QtNode::stringCallback, this);
}

void QtNode::start()
{
    spinner_.start();
}

void QtNode::stop()
{
    spinner_.stop();
    ros::shutdown();
}

void QtNode::compressedImageCallback(const sensor_msgs::CompressedImage::ConstPtr& msg)
{
    std::vector<uchar> buf(msg->data.begin(), msg->data.end());

    QtConcurrent::run([this, buf = std::move(buf)]() {
        cv::Mat cv_image = cv::imdecode(buf, cv::IMREAD_COLOR);
        if (cv_image.empty()) {
            ROS_WARN("Failed to decode image or empty image received");
            return;
        }
        cv::cvtColor(cv_image, cv_image, cv::COLOR_BGR2RGB);
        QImage q_image(cv_image.data, cv_image.cols, cv_image.rows,
                       static_cast<int>(cv_image.step),
                       QImage::Format_RGB888);
        emit updateImage(q_image.copy());
    });
}

void QtNode::stringCallback(const std_msgs::String::ConstPtr& msg)
{
    emit updateMessage(QString::fromStdString(msg->data));
}
