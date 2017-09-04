# GPU and CPU monitor

This project provides to very simple GTK v2 based tools to monitor continously the CPU (user/nice/system)or GPU load (utilisation/temperature/fan speed/memory usage/power usage) in an widget.
An option allows to print load information on screen (or to an logfile, if needed with a time stamp)

`gcpuinfo` displays current CPU load by reading `/proc/stat`. `ggpuinfo` displays GPU load by using the API provided by the `libnvidia-ml.so` library, which comes with CUDA 8.0.

## License

This library is under the [3-Clause BSD Licence](https://opensource.org/licenses/BSD-3-Clause). See [LICENSE.md](LICENSE.md)


## Author

Johannes Heinecke


## Requirements

  * gcpuinfo: gtk-2 development libraries
  * ggpuinfo: gtk-2 development libraries, libnvidia-ml.so


## Compiling

    mkdir build
    cd build
    cmake  -DCMAKE_BUILD_TYPE=Release  [-DCMAKE_INSTALL_PREFIX=/usr/local]  ..	

## Run

Run the following

    gcpuinfo -h
    ggpuinfo -h

for help on any option

## Install

    cd build
    make install

By default, the tools will be installed in `/usr/local/bin`. To have them installed in a different location, uncomment modify the line `set(CMAKE_INSTALL_PREFIX ../testinstall)` in [CMakeLists.txt](CMakeLists.txt)

