#include "camera.h"
#include "cxxopts.h"
#include "predictor.h"
#include "video_writer.h"

#include "opencv2/opencv.hpp"

int main(int argc, char **argv) {
  // Define CLI interface
  cxxopts::Options options(
      "PersonMask",
      "A program for inference a deep learning sematic segmentation model on a "
      "webcam videostream in order to remove background and provide a clean "
      "videostream with a person only");
  options.positional_help("[optional args]").show_positional_help();
  options.add_options()(
      "i,input", "input camera or path to input video file.",
      cxxopts::value<std::string>()->default_value("/dev/video0"))(
      "o,output",
      "Output virtual camera device, like /dev/video1, do not provide the "
      "parameter to skip the results outputting to virtual video device.",
      cxxopts::value<std::string>())(
      "d,debug", "Draw a debug visualization with OpenCV",
      cxxopts::value<bool>()->implicit_value("true")->default_value("false"))(
      "h,help", "Print help");

  auto args = options.parse(argc, argv);

  if (args.count("help")) {
    std::cout << options.help({""}) << std::endl;
    exit(0);
  }

  Camera cam = Camera();

  cam.init(args["input"].as<std::string>());

  cam.start();
  Predictor predictor = Predictor();
  cv::Mat img, masked_img;
  bool ret = false;

  int v4l2lo = 0;
  if (args.count("output"))
    v4l2lo =
        v4l2_loopback(args["output"].as<std::string>(), cam.height, cam.height);

  while (cam.is_running) {
    ret = cam.get_frame(img);
    if (!ret) {
      break;
    }
    int start_time = cv::getTickCount();
    masked_img = predictor.predict(img);
    int end_time = cv::getTickCount();
    if (args["debug"].as<bool>())
      std::cout << "Frame process time: "
                << (end_time - start_time) / cv::getTickFrequency()
                << std::endl;
    if (!img.empty()) {
      if (args["debug"].as<bool>())
        cv::imshow("Img", masked_img);
      if (v4l2lo != 0) {
        // If video writer of v4l2loopback was created
        int framesize = masked_img.total() * masked_img.elemSize();
        int write_res = write(v4l2lo, masked_img.data, framesize);
        if (write_res <= 0)
          break;
      }
    }
    if (cv::waitKey(1) == 27) // ESC
      break;
  }
  cv::destroyAllWindows();
}