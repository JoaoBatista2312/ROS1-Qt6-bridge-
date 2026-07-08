cmake_minimum_required(VERSION 3.0.2)
project(qt6_ros)

find_package(catkin REQUIRED COMPONENTS
  rospy
  sensor_msgs
  std_msgs
)

catkin_python_setup()

catkin_package()

catkin_install_python(PROGRAMS
  scripts/qml_level
  scripts/increment_publisher
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY qml/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/qml
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)
