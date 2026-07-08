#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import CompressedImage
from std_msgs.msg import String

from PySide6.QtCore import QThread, Signal, Property
from PySide6.QtGui import QImage

from concurrent.futures import ThreadPoolExecutor
import cv2
import numpy as np

_MIN_WINDOW_WIDTH = 200
_MAX_WINDOW_WIDTH = 3840
_MIN_WINDOW_HEIGHT = 150
_MAX_WINDOW_HEIGHT = 2160


class QtNode(QThread):
    updateImage = Signal(QImage)
    updateMessage = Signal(str)

    def __init__(self):
        super().__init__()
        self._executor = ThreadPoolExecutor(max_workers=2)
        rospy.init_node("qt6_ros_node", anonymous=False, disable_signals=True)

        self._window_width = self._clamp_param(
            "window_width", rospy.get_param("~window_width", 640),
            _MIN_WINDOW_WIDTH, _MAX_WINDOW_WIDTH)
        self._window_height = self._clamp_param(
            "window_height", rospy.get_param("~window_height", 480),
            _MIN_WINDOW_HEIGHT, _MAX_WINDOW_HEIGHT)

        # Clamp frame size first, then confine its offset so the whole frame stays inside the window.
        self._image_width = self._clamp_param(
            "image_width", rospy.get_param("~image_width", 640),
            1, self._window_width)
        self._image_height = self._clamp_param(
            "image_height", rospy.get_param("~image_height", 480),
            1, self._window_height)
        self._image_x = self._clamp_param(
            "image_x", rospy.get_param("~image_x", 0),
            0, self._window_width - self._image_width)
        self._image_y = self._clamp_param(
            "image_y", rospy.get_param("~image_y", 0),
            0, self._window_height - self._image_height)

        self.qml_file = rospy.get_param("~qml_file", "")

        image_topic = rospy.get_param(
            "~image_topic", "/camera/color/image_rect_color/compressed")
        rospy.Subscriber(image_topic, CompressedImage, self.compressed_image_callback, queue_size=1)

        counter_example = rospy.get_param("~counter_example", False)
        if counter_example:
            chatter_topic = rospy.get_param("~chatter_topic", "chatter")
            rospy.Subscriber(chatter_topic, String, self.string_callback, queue_size=10)

    @Property(int, constant=True)
    def windowWidth(self):
        return self._window_width

    @Property(int, constant=True)
    def windowHeight(self):
        return self._window_height

    @Property(int, constant=True)
    def imageX(self):
        return self._image_x

    @Property(int, constant=True)
    def imageY(self):
        return self._image_y

    @Property(int, constant=True)
    def imageWidth(self):
        return self._image_width

    @Property(int, constant=True)
    def imageHeight(self):
        return self._image_height

    def _clamp_param(self, name, value, min_value, max_value):
        clamped = max(min_value, min(value, max_value))
        if clamped != value:
            rospy.logwarn(
                f"Parameter '{name}' value {value} is out of range "
                f"[{min_value}, {max_value}], clamped to {clamped}")
        return clamped

    def _decode_image(self, data: bytes):
        np_arr = np.frombuffer(data, np.uint8)
        cv_image = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        if cv_image is not None and cv_image.size > 0:
            height, width, _ = cv_image.shape
            bytes_per_line = 3 * width
            q_image = QImage(
                cv_image.data, width, height, bytes_per_line,
                QImage.Format.Format_RGB888
            ).rgbSwapped()
            self.updateImage.emit(q_image)
        else:
            print("Failed to decode image or empty image received")

    def compressed_image_callback(self, msg):
        self._executor.submit(self._decode_image, bytes(msg.data))

    def string_callback(self, msg):
        self.updateMessage.emit(msg.data)

    def run(self):
        rospy.spin()

    def stop(self):
        rospy.signal_shutdown("Shutting down ROS node...")
        self._executor.shutdown(wait=False)
