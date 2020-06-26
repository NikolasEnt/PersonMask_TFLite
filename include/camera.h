#ifndef CAMERA_H
#define CAMERA_H
#include <thread>

#include "buffer.h"
#include "opencv2/opencv.hpp"

class Camera {
public:
  void init(int id);
  void init(std::string path); // Open video file
  ~Camera();
  void start();
  bool get_frame(cv::Mat &img);
  bool is_running = true;
  int width = 0;
  int height = 0;

private:
  std::thread _camera_thread;
  cv::Mat _raw_frame;
  cv::VideoCapture _cap;
  void start_grabbing();
  cv::Rect square_roi(cv::Mat frame);
  Buffer<cv::Mat> _frame_buffer = Buffer<cv::Mat>(3);
  double _frame_period = 1000 / 30;
};

#endif