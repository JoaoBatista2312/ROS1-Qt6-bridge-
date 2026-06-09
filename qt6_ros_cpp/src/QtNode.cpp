#include "qt6_ros_cpp/QtNode.h"
#include <opencv2/opencv.hpp>
#include <vector>

QtNode::QtNode(QObject* parent)
    : QThread(parent), running_(true)
{
    ros::NodeHandle nh;
    image_sub_ = nh.subscribe("/camera/color/image_rect_color/compressed", 1,
                               &QtNode::compressedImageCallback, this);
    string_sub_ = nh.subscribe("chatter", 10, &QtNode::stringCallback, this);
}

void QtNode::compressedImageCallback(const sensor_msgs::CompressedImage::ConstPtr& msg)
{
    std::vector<uchar> buf(msg->data.begin(), msg->data.end());
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
}

void QtNode::stringCallback(const std_msgs::String::ConstPtr& msg)
{
    emit updateMessage(QString::fromStdString(msg->data));
}

void QtNode::run()
{
    ros::Rate rate(100);
    while (ros::ok() && running_) {
        ros::spinOnce();
        rate.sleep();
    }
}

void QtNode::stop()
{
    running_ = false;
    ros::shutdown();
}
