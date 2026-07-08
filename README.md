# ROS1 Qt6 Bridge

> **Work in progress.** APIs, file layout, and package structure may still change.

A ROS1 (Noetic) bridge for Qt6/QML applications, available in both **C++** and **Python**. Displays ROS image streams in a hardware-accelerated QML window and exposes ROS data to the QML layer via Qt properties and signals.

## Packages

| Package | Language | Qt binding | Build system |
|---------|----------|------------|--------------|
| `qt6_ros_cpp` | C++ | Qt6 | catkin (CMake) |
| `qt6_ros` | Python | PySide6 | catkin (Python) |

Both packages provide the same functionality:
- **QtNode** &mdash; spins a ROS node in a background thread, subscribing to a `sensor_msgs/CompressedImage` topic and a `std_msgs/String` topic.
- **ImageProvider** &mdash; thread-safe `QQuickImageProvider` that feeds decoded frames to QML.
- **AppConnection** &mdash; exposes string data from ROS as a QML-bindable property.

The C++ package also builds a shared library (`libqt6_ros_cpp.so`) that downstream packages can link against.

## Dependencies

- ROS1 (tested with Noetic)
- Qt6 (Core, Gui, Qml, Quick, Concurrent)
- OpenCV
- PySide6 (Python package only)

## Install

```bash
cd ~/catkin_ws/src
git clone https://github.com/JoaoBatista2312/ROS1-Qt6-bridge.git
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

## Usage

### Launch the demo (C++)

```bash
roslaunch qt6_ros_cpp GUI_interface.launch
```

### Launch the demo (Python)

```bash
roslaunch qt6_ros GUI_interface.launch
```

By default, each launch file only starts the Qt6 GUI node. Pass `counter_example:=true` to also start a demo `increment_publisher` that publishes an incrementing string once per second, shown in the window title &mdash; see [Counter example](#counter-example) below.

### Subscribed topics

| Topic (default) | Type | Description |
|------------------|------|-------------|
| `/camera/color/image_rect_color/compressed` | `sensor_msgs/CompressedImage` | Compressed image stream displayed in the QML window |
| `chatter` | `std_msgs/String` | String data shown in the window title (only subscribed when `counter_example:=true`) |

Both topic names are configurable, see below.

### Image topic

The image topic can be overridden with the `image_topic` launch argument:

```bash
roslaunch qt6_ros_cpp GUI_interface.launch image_topic:=/my_camera/image/compressed
```

### Counter example

The `increment_publisher` demo (and the GUI's subscription to it) is disabled by default. Enable it with `counter_example:=true`, and optionally override its topic name with `counter_topic`:

```bash
roslaunch qt6_ros_cpp GUI_interface.launch counter_example:=true counter_topic:=my_counter
```

With `counter_example:=false` (the default), the window title stays at its static placeholder text and no counter topic is subscribed to or published.

### Customizing the GUI layout

The GUI's QML file can be swapped out entirely with the `qml_file` launch argument, so you can hand it your own layout without touching the package source or rebuilding:

```bash
roslaunch qt6_ros_cpp GUI_interface.launch qml_file:=/path/to/your/custom.qml
```

Left empty (the default), the bundled `qml/main.qml` is used.

The only requirement for a custom file is a `Window` as its root item &mdash; everything else is optional:

```qml
import QtQuick.Window 6.0

Window {
}
```

This alone is a fully working, minimal GUI: the window is automatically sized (from `window_width`/`window_height`) and made visible from code after the file loads, so you don't need to bind `width`/`height`/`visible` yourself, or know about `guiConfig` at all, just to get a correctly-sized window on screen.

Everything beyond that is opt-in, through the same context properties the bundled file uses:

- `guiConfig` &mdash; `imageX`, `imageY`, `imageWidth`, `imageHeight` (window size is applied automatically, see above, but these still need to be bound explicitly if you want the frame placed/sized per the launch arguments)
- `imageProvider` &mdash; `imageUpdateCount`, bumped on every new frame; pair with an `Image` whose `source` is `"image://myimageprovider/image?" + imageProvider.imageUpdateCount` to show the camera feed
- `appConnection` &mdash; `rosData`, the latest string from the counter topic (see [Counter example](#counter-example)); bind it to `title` or a `Text` if you want it shown

### GUI window and camera frame configuration

Both launch files accept arguments to control the GUI window size and the camera frame's size/placement within it:

| Argument | Default | Description |
|----------|---------|-------------|
| `window_width` | `640` | GUI window width, in pixels |
| `window_height` | `480` | GUI window height, in pixels |
| `image_width` | `640` | Camera frame width, in pixels |
| `image_height` | `480` | Camera frame height, in pixels |
| `image_x` | `0` | Camera frame horizontal offset from the window's left edge, in pixels |
| `image_y` | `0` | Camera frame vertical offset from the window's top edge, in pixels |

```bash
roslaunch qt6_ros_cpp GUI_interface.launch \
  window_width:=900 window_height:=700 \
  image_width:=400 image_height:=300 image_x:=250 image_y:=200
```

Values are validated on startup: the window is clamped to `200`-`3840` px wide and `150`-`2160` px tall, and the camera frame is always clamped to fit fully inside the window (size first, then offset). Any out-of-range value is adjusted automatically and logged as a warning rather than producing an invalid or off-screen layout.

The camera frame is currently the only visual item in the QML window, so there's nothing for it to cover. If you add more UI elements to `qml/main.qml`, keep QML's stacking order in mind: siblings with the same `z` (the default) render in declaration order, so anything declared *after* the `Image` block draws on top of it regardless of overlap, while anything declared *before* it will be covered wherever its bounds intersect the frame's `(image_x, image_y, image_width, image_height)` rectangle &mdash; `Image` with `PreserveAspectFit` is only transparent in its letterboxed margins, the picture area itself is fully opaque.

### Using the C++ library in your own package

In your `package.xml`:

```xml
<depend>qt6_ros_cpp</depend>
```

In your `CMakeLists.txt`:

```cmake
find_package(catkin REQUIRED COMPONENTS qt6_ros_cpp)
include_directories(${catkin_INCLUDE_DIRS})
target_link_libraries(my_node ${catkin_LIBRARIES})
```

This gives you access to `QtNode`, `ImageProvider`, and `AppConnection` headers and the linked library.

## License

This package is licensed under the GNU General Public License v3 (GPLv3). You are free to modify and redistribute this package under the same license terms. For more details, please refer to the [GPLv3 license](LICENSE).
