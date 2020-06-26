
# Person Segmentation

The goal of the project is to inference a deep learning semantic segmentation model on a webcam video stream in Linux in order to remove background and provide a clean video stream with a person only. The need for such a project was imposed by the lack of such tools in Linux and the popularity of video calls services in present times.

![Demo of the project output](readme_imgs/demo.gif "Project demo")
*The app output demo. Based on a video from [vimeo](https://vimeo.com/14802778), which is licenced under CC Attribution license.*

The main feature of the project in terms of computer vision is the usage of a lightweight neural network model which is able to run on CPU in real-time, which is quite challenging for the semantic segmentation task. The project is implemented in C++17.

*Note:* The project is in early alpha stage, so, it may contain bugs, and it is unreliable.

- [Person Segmentation](#person-segmentation)
  - [Dependances](#dependances)
      - [GCC](#gcc)
      - [Build system](#build-system)
      - [FFmpeg](#ffmpeg)
      - [OpenCV >= 4.3](#opencv--43)
      - [TensorFlow Lite](#tensorflow-lite)
      - [v4l2loopback](#v4l2loopback)
  - [How to build](#how-to-build)
  - [How to run](#how-to-run)
  - [All args](#all-args)
  - [Project structure](#project-structure)
  - [Performance](#performance)

## Dependances

All dependences are downloaded in `~/dependencies/`

#### GCC

The project was built and tested with gcc 9.3.0 and 10.1.1.

Gcc 9.3.0 can be installed and selected as default in Ubuntu by:

```
sudo apt-get update -y && 
sudo apt-get upgrade -y && 
sudo apt-get dist-upgrade -y && 
sudo apt-get install build-essential software-properties-common -y && 
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y && 
sudo apt-get update -y &&
sudo apt-get install gcc-9 g++-9 -y && 
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-9 && 
sudo update-alternatives --config gcc
```

#### Build system

The program requires `cmake` (>=3.11.3) and `make` to build.

#### FFmpeg

If you'd like to input video from a video file, you'll need to compile FFmpeg. See instructions [here](http://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu). Actually, it needs libx264 only, so, minimal build and installation of dependencies on Ubuntu may look like:

```
sudo apt-get update -qq && sudo apt-get -y install \
  autoconf \
  automake \
  build-essential \
  cmake \
  git-core \
  libass-dev \
  libfreetype6-dev \
  libgnutls28-dev \
  libsdl2-dev \
  libtool \
  libva-dev \
  libvdpau-dev \
  libvorbis-dev \
  libxcb1-dev \
  libxcb-shm0-dev \
  libxcb-xfixes0-dev \
  pkg-config \
  texinfo \
  wget \
  yasm \
  nasm \
  zlib1g-dev \
  libx264-dev


mkdir ~/dependencies/ffmpeg_sources
mkdir ~/dependencies/ffmpeg_build
mkdir ~/dependencies/bin
cd ~/dependencies/ffmpeg_sources
wget -O ffmpeg-snapshot.tar.bz2 https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2
tar xjvf ffmpeg-snapshot.tar.bz2 && cd ffmpeg

PATH="$HOME/dependencies/bin:$PATH" PKG_CONFIG_PATH="$HOME/dependencies/ffmpeg_build/lib/pkgconfig" ./configure  --prefix="$HOME/dependencies/ffmpeg_build"   --pkg-config-flags="--static"   --extra-cflags="-I$HOME/dependencies/ffmpeg_build/include"   --extra-ldflags="-L$HOME/dependencies/ffmpeg_build/lib"   --extra-libs=-lpthread   --extra-libs=-lm   --bindir="$HOME/dependencies/bin"   --enable-gpl     --enable-libfreetype --enable-libx264 --enable-nonfree
PATH="$HOME/dependencies/bin:$PATH" make -j$(nproc) && make install
```

Be patient, it may take a while.

#### OpenCV >= 4.3

Instullation [guide](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html).
Basic installation may look like:

```
mkdir ~/dependencies/opencv
cd ~/dependencies
wget https://github.com/opencv/opencv/archive/4.3.0.zip
unzip 4.3.0.zip
cd ~/dependencies/opencv-4.3.0
mkdir build && cd build
export LD_LIBRARY_PATH=$HOME/dependencies/ffmpeg_build/lib/:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$HOME/dependencies/ffmpeg_build/lib/pkgconfig:$PKG_CONFIG_PATH:
export PKG_CONFIG_LIBDIR=$HOME/dependencies/ffmpeg_build/lib/:$PKG_CONFIG_LIBDIR

sudo apt-get install -y libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev

cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=$HOME/dependencies/opencv/ \
-D INSTALL_PYTHON_EXAMPLES=OFF  -D INSTALL_C_EXAMPLES=OFF -D WITH_FFMPEG=1 \
-D BUILD_LIST=core,imgproc,imgcodecs,highgui,video,videoio ..
make -j$(nproc)
make install
```

The openCV should be compiled after ffmpeg in order to be build with it for ffmpeg backend support.

#### TensorFlow Lite

The current implementation was tested with TFLite v. 2.2

```
cd ~/dependencies
git clone --branch r2.2 --single-branch  https://github.com/tensorflow/tensorflow.git
cd tensorflow
./tensorflow/lite/tools/make/download_dependencies.sh
./tensorflow/lite/tools/make/build_lib.sh
```

Do not forget to update pathes of the libs in [CMakeLists.txt](CMakeLists.txt) (e.g., set TENSORFLOW variable) if you modified installation pathes.

The project uses deep learning model MobeleNetV3-segm from an open source [repo](https://github.com/OniroAI/Semantic-segmentation-with-MobileNetV3).
It was retrained for 256x256 px input resolution, and extra data was utilized for training.

#### v4l2loopback

Virtual video devices Linux kernel module is required to enable the program video output be accesable from other programs.

Some linux distros have it already preinstalled, or one can install it by following [documentation](https://github.com/umlaeute/v4l2loopback).

```
sudo apt-get install -y v4l2loopback-dkms
```

## How to build

```
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## How to run 

For test purposes, a test video is provided in `data/test.mp4`. The video is a random video from [vimeo](https://vimeo.com/14802778) with several speaking people in front of a camera, and it is licenced under CC Attribution license.

Before running the program, you have to create a virtual webcam with v4l2loopback for virtual system device output (you may skip the step if a debug visualization is required only):

```
sudo modprobe v4l2loopback devices=1 max_buffers=2 exclusive_caps=1 card_label="PersonMaskCam"
```

Note a video device name it creates, like  `/dev/videoX`. You may use the camera as input device for video calls apps.

Keep in mind, that in case you'd like to use different video resolution, you may have to restart the virtual video device.

To test it on a video file, just run
```
cd build
./person_mask -i ../data/test.mp4 -d
```

It will enable a debug visualization and run the program with a video file `../data/test.mp4` as input. Press ESC utton to stop the program.

You can redirect the output to a v4l2loopback virtual device by adding `-o /dev/videoX`, where /dev/videoX - a device created during the virtual video devices kernel module initialization.

## All args
```
  -i, --input arg   input camera or path to input video file. (default:
                    /dev/video0)
  -o, --output arg  Output virtual camera device, like /dev/video1, do not
                    provide the parameter to skip the results outputting to
                    virtual video device.
  -d, --debug       Draw a debug visualization with OpenCV. It also prints out
                    frame processing time into stdout.
  -h, --help        Print help
```

## Project structure

The main loop is defined in [src/main.cpp](src/main.cpp) file. It parses user input, initializes input virtual camera (frame grabbing from a video file or a webcam), applies predictor to the grabbed images and draws the results visualization.

The virtual camera [src/camera.cpp](src/camera.cpp) grab frames and put them into custom Buffer (see [include/buffer.h](include/buffer.h)).

Frames from the frame buffer are processed with a predictor [src/predictor.cpp](src/predictor.cpp), which run a TFLite model and all necessary image and predicted mask processings. The model parameters (like the path to the model weights, segmentation threshold and a number of used CPU cores) could be adjusted in the [src/predictor.cpp](src/predictor.cpp) file.

To output results as a system virtual camera, a video v4l2-loopback video  writer is created with a function from [src/video_writer.cpp](src/video_writer.cpp).

## Performance

It is able to run at ~50 fps at half of CPU cores and ~30 fps in single core mode on an Intel i5-8250U.
