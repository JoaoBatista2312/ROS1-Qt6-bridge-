#!/usr/bin/env python3

import rospy
from std_msgs.msg import String


def main():
    rospy.init_node('increment_publisher', anonymous=False)
    chatter_topic = rospy.get_param('~chatter_topic', 'chatter')
    pub = rospy.Publisher(chatter_topic, String, queue_size=10)
    rate = rospy.Rate(1)
    count = 0

    while not rospy.is_shutdown():
        message = f"Count: {count}"
        rospy.loginfo(message)
        pub.publish(String(data=message))
        count += 1
        rate.sleep()


if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass
