#pragma once
#include <QObject>
#include <QString>

class AppConnection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rosData READ rosData NOTIFY rosDataChanged)
public:
    explicit AppConnection(QObject* parent = nullptr);
    QString rosData() const;

public slots:
    void getRosData(const QString& data);

signals:
    void rosDataChanged();

private:
    QString ros_data_;
};
