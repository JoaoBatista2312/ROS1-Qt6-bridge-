#pragma once
#include <QObject>
#include <QImage>
#include <QColor>
#include <QQuickImageProvider>
#include <QReadWriteLock>

class ImageProvider : public QQuickImageProvider
{
    Q_OBJECT
    Q_PROPERTY(int imageUpdateCount READ imageUpdateCount NOTIFY imageCounterSignal)
public:
    explicit ImageProvider();

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
    int imageUpdateCount() const;

public slots:
    void updateImage(const QImage& image);

signals:
    void imageCounterSignal();

private:
    QImage image_;
    int update_count_;
    mutable QReadWriteLock lock_;
};
