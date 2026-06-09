cmake_minimum_required(VERSION 3.10)
project(qt6_ros_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  std_msgs
  roslib
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Concurrent)
find_package(OpenCV REQUIRED)

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS roscpp sensor_msgs std_msgs roslib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(qt6_ros_cpp_node
  src/main.cpp
  src/QtNode.cpp
  src/ImageProvider.cpp
  src/AppConnection.cpp
)

target_link_libraries(qt6_ros_cpp_node
  Qt6::Core
  Qt6::Gui
  Qt6::Qml
  Qt6::Quick
  Qt6::Concurrent
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(increment_publisher_cpp
  src/increment_publisher.cpp
)

target_link_libraries(increment_publisher_cpp
  ${catkin_LIBRARIES}
)

install(TARGETS qt6_ros_cpp_node increment_publisher_cpp
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY qml/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/qml
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)
