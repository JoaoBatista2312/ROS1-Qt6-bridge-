#include "qt6_ros_cpp/ImageProvider.h"

ImageProvider::ImageProvider()
    : QObject(nullptr), QQuickImageProvider(QQuickImageProvider::Image),
      update_count_(0)
{}

void ImageProvider::updateImage(const QImage& image)
{
    image_ = image;
    ++update_count_;
    emit imageCounterSignal();
}

QImage ImageProvider::requestImage(const QString& /*id*/, QSize* size, const QSize& /*requestedSize*/)
{
    if (image_.isNull()) {
        QImage red(300, 300, QImage::Format_RGBA8888);
        red.fill(QColor(255, 0, 0));
        if (size) *size = red.size();
        return red;
    }
    if (size) *size = image_.size();
    return image_;
}

int ImageProvider::imageUpdateCount() const
{
    return update_count_;
}
