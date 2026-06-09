#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import CompressedImage
from std_msgs.msg import String

from PySide6.QtCore import QThread, Signal
from PySide6.QtGui import QImage

from concurrent.futures import ThreadPoolExecutor
import cv2
import numpy as np

class QtNode(QThread):
    updateImage = Signal(QImage)
    updateMessage = Signal(str)

    def __init__(self):
        super(QtNode, self).__init__()
        self._executor = ThreadPoolExecutor(max_workers=2)
        rospy.init_node("listener", anonymous=True, disable_signals=True)
        rospy.Subscriber("chatter", String, self.string_callback)
        rospy.Subscriber("/camera/color/image_rect_color/compressed", CompressedImage, self.compressed_image_callback)

    def _decode_image(self, data: bytes):
        np_arr = np.frombuffer(data, np.uint8)
        cv_image = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        if cv_image is not None and cv_image.size > 0:
            height, width, _ = cv_image.shape
            bytes_per_line = 3 * width
            q_image = QImage(cv_image.data, width, height, bytes_per_line, QImage.Format.Format_RGB888).rgbSwapped()
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
