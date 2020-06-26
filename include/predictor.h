#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <thread>
#include <vector>

#include "opencv2/opencv.hpp"
#include <opencv2/core/ocl.hpp>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"

#include "buffer.h"
#include "camera.h"

struct Config {
  static const int INPUT_SIZE;
  static const std::string MODEL_PATH;
  static const int THRESHOLD;
  static const unsigned int N_THREADS;
};

class Predictor {
public:
  Predictor();
  void start();
  cv::Mat predict(cv::Mat img);

private:
  std::unique_ptr<tflite::FlatBufferModel> _model;
  tflite::ops::builtin::BuiltinOpResolver _resolver;
  std::unique_ptr<tflite::Interpreter> _interpreter;
  cv::Mat _input;
  cv::Mat _output;
  cv::Mat _res_img;
  cv::Mat _resized_img;
  cv::Mat _mask;
  cv::Mat _mask_resized;
  cv::Size _input_size = cv::Size(Config::INPUT_SIZE, Config::INPUT_SIZE);
  void postprocessing(cv::Mat &mask);
};

#endif
