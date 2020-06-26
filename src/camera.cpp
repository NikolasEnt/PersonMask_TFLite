#include "camera.h"

// Init camera with a divece id (webcam)
void Camera::init(int idx) {
  _cap = cv::VideoCapture(idx);
  _frame_period = 1000.0 / _cap.get(cv::CAP_PROP_FPS);
  width = _cap.get(cv::CAP_PROP_FRAME_WIDTH);
  height = _cap.get(cv::CAP_PROP_FRAME_HEIGHT);
}

void Camera::init(std::string path) {
  _cap = cv::VideoCapture(path);
  _frame_period = _cap.get(cv::CAP_PROP_FPS);
}

Camera::~Camera() {
  is_running = false;
  _camera_thread.join();
  if (_cap.isOpened()) {
    _cap.release();
  }
}

void Camera::start() {
  _camera_thread = std::thread(&Camera::start_grabbing, this);
}

void Camera::start_grabbing() {
  while (is_running) {
    auto start_time = cv::getTickCount();
    _cap >> _raw_frame;
    // std::cout << "Captured frame: " << _raw_frame.size() << std::endl;
    // The end of the video or the camera is disconected
    if (_raw_frame.empty()) {
      is_running = false;
      break;
    }
    // To predict on central square ROI part of the frame use Camera::square_roi
    _frame_buffer.push(_raw_frame);
    // Sleep remain time, given the FPS (required for reading videofiles with
    // original frame rate)
    int time2sleep = static_cast<int>(
        std::max(_frame_period - (cv::getTickCount() - start_time) /
                                     cv::getTickFrequency() * 1000.0,
                 1.0));
    std::this_thread::sleep_for(std::chrono::milliseconds(time2sleep));
  }
}

bool Camera::get_frame(cv::Mat &img) { return _frame_buffer.pop(img); }

cv::Rect Camera::square_roi(cv::Mat frame) {
  int h = frame.rows;
  int w = frame.cols;
  int shift = static_cast<int>((w - h) / 2.0);
  return cv::Rect(shift, 0, h, h);
}
