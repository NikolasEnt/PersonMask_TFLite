#include "predictor.h"

// Model input resolution
const int Config::INPUT_SIZE = 256;
// Model path
const std::string Config::MODEL_PATH =
    "../data/models/mobilenet_v3_segm_256.tflite";
// Segmentation binarization threshold. Should be in [0..255]
const int Config::THRESHOLD = 80;
// Numper of cpu threads to use by TFLite and OpenCV.
const unsigned int Config::N_THREADS =
    std::max(static_cast<int>(std::thread::hardware_concurrency() / 2), 1);

Predictor::Predictor() {
  cv::setNumThreads(Config::N_THREADS);
  _model = tflite::FlatBufferModel::BuildFromFile(Config::MODEL_PATH.c_str());
  tflite::InterpreterBuilder(*_model, _resolver)(&_interpreter);
  _interpreter->AllocateTensors();
  _interpreter->SetNumThreads(Config::N_THREADS);
  _interpreter->SetAllowFp16PrecisionForFp32(true);

  _input = cv::Mat(Config::INPUT_SIZE, Config::INPUT_SIZE, CV_32FC3,
                   _interpreter->typed_input_tensor<float>(0));
  _output = cv::Mat(Config::INPUT_SIZE, Config::INPUT_SIZE, CV_32FC1,
                    _interpreter->typed_output_tensor<float>(0));
}

cv::Mat Predictor::predict(cv::Mat img) {
  if (_res_img.empty()) {
    _res_img = cv::Mat(img.size(), CV_8UC3, cv::Scalar(255, 255, 255));
  }
  cv::resize(img, _resized_img, _input_size, 0, 0, cv::INTER_LINEAR);
  cv::cvtColor(_resized_img, _resized_img, cv::COLOR_BGR2RGB);
  _resized_img.convertTo(_input, CV_32FC3, 1.0 / 255.0, 0.0);
  _interpreter->Invoke(); // Make prediction
  _output.convertTo(_mask, CV_8UC1, 255, 0);
  cv::threshold(_mask, _mask, Config::THRESHOLD, 255, cv::THRESH_BINARY);
  Predictor::postprocessing(_mask);
  cv::resize(_mask, _mask_resized, img.size(), 0, 0, cv::INTER_NEAREST);
  _res_img.setTo(cv::Scalar(255, 255, 255));
  img.copyTo(_res_img, _mask_resized);
  return _res_img;
}

void Predictor::postprocessing(cv::Mat &mask) {
  std::vector<std::vector<cv::Point>> cnts; // Vector of contours
  cv::findContours(mask, cnts, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
  // Find and draw the biggest contour
  if (cnts.size() > 0) {
    int idx = 0;
    double area;
    double max_area = 0;
    for (size_t i = 0; i < cnts.size(); i++) {
      area = cv::contourArea(cnts[i]);
      if (area > max_area) {
        max_area = area;
        idx = i;
      }
    }
    mask.setTo(0); // Zero out mask to draw the contour only
    drawContours(mask, cnts, idx, 255, -1);
  }
}