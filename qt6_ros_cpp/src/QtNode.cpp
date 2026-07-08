#include "qt6_ros_cpp/QtNode.h"
#include <opencv2/opencv.hpp>
#include <QtConcurrent/QtConcurrent>
#include <algorithm>
#include <vector>

namespace {
constexpr int kMinWindowWidth = 200;
constexpr int kMaxWindowWidth = 3840;
constexpr int kMinWindowHeight = 150;
constexpr int kMaxWindowHeight = 2160;
}  // namespace

int QtNode::clampParam(const std::string& name, int value, int min_value, int max_value) {
    int clamped = std::clamp(value, min_value, max_value);
    if (clamped != value) {
        ROS_WARN("Parameter '%s' value %d is out of range [%d, %d], clamped to %d",
                 name.c_str(), value, min_value, max_value, clamped);
    }
    return clamped;
}

QtNode::QtNode(QObject* parent)
    : QObject(parent), pnh_("~"), spinner_(2) {
    int window_width, window_height, image_width, image_height, image_x, image_y;
    pnh_.param("window_width", window_width, 640);
    pnh_.param("window_height", window_height, 480);
    window_width_ = clampParam("window_width", window_width, kMinWindowWidth, kMaxWindowWidth);
    window_height_ = clampParam("window_height", window_height, kMinWindowHeight, kMaxWindowHeight);

    // Clamp frame size first, then confine its offset so the whole frame stays inside the window.
    pnh_.param("image_width", image_width, 640);
    pnh_.param("image_height", image_height, 480);
    image_width_ = clampParam("image_width", image_width, 1, window_width_);
    image_height_ = clampParam("image_height", image_height, 1, window_height_);

    pnh_.param("image_x", image_x, 0);
    pnh_.param("image_y", image_y, 0);
    image_x_ = clampParam("image_x", image_x, 0, window_width_ - image_width_);
    image_y_ = clampParam("image_y", image_y, 0, window_height_ - image_height_);

    pnh_.param<std::string>("qml_file", qml_file_, "");

    std::string image_topic;
    pnh_.param<std::string>("image_topic", image_topic, "/camera/color/image_rect_color/compressed");
    image_sub_ = nh_.subscribe(image_topic, 1, &QtNode::compressedImageCallback, this);

    bool counter_example = false;
    pnh_.param("counter_example", counter_example, false);
    if (counter_example) {
        std::string chatter_topic;
        pnh_.param<std::string>("chatter_topic", chatter_topic, "chatter");
        string_sub_ = nh_.subscribe(chatter_topic, 10, &QtNode::stringCallback, this);
    }
}

void QtNode::start() {
    spinner_.start();
}

void QtNode::stop() {
    spinner_.stop();
    ros::shutdown();
}

void QtNode::compressedImageCallback(const sensor_msgs::CompressedImage::ConstPtr& msg) {
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

void QtNode::stringCallback(const std_msgs::String::ConstPtr& msg) {
    emit updateMessage(QString::fromStdString(msg->data));
}
