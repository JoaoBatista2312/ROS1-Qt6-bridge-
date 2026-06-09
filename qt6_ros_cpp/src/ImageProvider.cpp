#include "qt6_ros_cpp/ImageProvider.h"

ImageProvider::ImageProvider()
    : QObject(nullptr), QQuickImageProvider(QQuickImageProvider::Image),
      update_count_(0)
{}

void ImageProvider::updateImage(const QImage& image)
{
    {
        QWriteLocker locker(&lock_);
        image_ = image;
        ++update_count_;
    }
    emit imageCounterSignal();
}

QImage ImageProvider::requestImage(const QString& /*id*/, QSize* size, const QSize& /*requestedSize*/)
{
    QReadLocker locker(&lock_);
    if (image_.isNull()) {
        locker.unlock();
        QImage red(300, 300, QImage::Format_RGBA8888);
        red.fill(QColor(255, 0, 0));
        if (size) *size = red.size();
        return red;
    }
    QImage copy = image_;
    if (size) *size = copy.size();
    return copy;
}

int ImageProvider::imageUpdateCount() const
{
    QReadLocker locker(&lock_);
    return update_count_;
}
