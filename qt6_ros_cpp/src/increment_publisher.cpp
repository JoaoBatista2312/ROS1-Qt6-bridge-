#include <ros/ros.h>
#include <std_msgs/String.h>
#include <string>

int main(int argc, char* argv[])
{
    ros::init(argc, argv, "increment_publisher");
    ros::NodeHandle nh;

    ros::Publisher pub = nh.advertise<std_msgs::String>("/chatter", 10);
    ros::Rate rate(80);

    int count = 0;
    while (ros::ok()) {
        std_msgs::String msg;
        msg.data = "Count: " + std::to_string(count);
        ROS_INFO("%s", msg.data.c_str());
        pub.publish(msg);
        ++count;
        rate.sleep();
    }

    return 0;
}
