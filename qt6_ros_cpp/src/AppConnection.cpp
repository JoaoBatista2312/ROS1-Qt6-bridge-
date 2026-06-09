#include "qt6_ros_cpp/AppConnection.h"

AppConnection::AppConnection(QObject* parent)
    : QObject(parent), ros_data_("Hello World")
{}

QString AppConnection::rosData() const
{
    return ros_data_;
}

void AppConnection::getRosData(const QString& data)
{
    if (data != ros_data_) {
        ros_data_ = data;
        emit rosDataChanged();
    }
}
