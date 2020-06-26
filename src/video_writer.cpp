#include "video_writer.h"

int v4l2_loopback(const std::string &device, int width, int height) {

  struct v4l2_format vid_format;
  int fdwr = open(device.c_str(), O_RDWR);
  assert(fdwr >= 0);

  vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  vid_format.fmt.pix.width = width;
  vid_format.fmt.pix.height = height;
  vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
  vid_format.fmt.pix.sizeimage = width * height * 3;
  vid_format.fmt.pix.field = V4L2_FIELD_NONE;
  vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

  int ret_code = ioctl(fdwr, VIDIOC_S_FMT, &vid_format);
  assert(ret_code != -1);

  return fdwr;
}