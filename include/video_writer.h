#ifndef VIDEO_WRITER_H
#define VIDEO_WRITER_H

#include <cassert>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

int v4l2_loopback(const std::string &device, int width, int height);

#endif
