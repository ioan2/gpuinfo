# This library is under the Modified BSD License:
# 
# Copyright (c) 2017, Johannes Heinecke. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
# 
#   * Redistributions of source code must retain the above copyright notice, 
#     this list of conditions and the following disclaimer.
# 
#   * Redistributions in binary form must reproduce the above copyright notice, 
#     this list of conditions and the following disclaimer in the documentation 
#     and/or other materials provided with the distribution.
# 
#   * Neither the name of the copyright holder nor the names of its contributors may be used
#     to endorse or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.8)

project (puinfo)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
find_package(GTK2 REQUIRED)
if (NOT GTK2_FOUND)
  message(ERROR "GTK2 not found:")
else()
  include_directories(${GTK2_INCLUDE_DIRS})
endif()


find_package(CUDA)
if (CUDA_FOUND)
  # change here path to libnvidia-ml.so
  set(LIBNVIDIA_ML "/usr/local/cuda-8.0/lib64/stubs/libnvidia-ml.so")
  message(STATUS "Cuda found: ${CUDA_INCLUDE_DIRS}, using ${LIBNVIDIA_ML}")
  include_directories(${CUDA_INCLUDE_DIRS})
  if (NOT EXISTS "${CUDA_INCLUDE_DIRS}/nvml.h")
    set(CUDA_FOUND FALSE)
    message(STATUS "Cuda does not provide nvml.h, cannot compile ggpuinfo")
  endif()
else()
  message(STATUS "Cuda not found: cannot compile ggpuinfo")
endif()

add_subdirectory (src)

# change this in order to avoid /usr/local/
set(CMAKE_INSTALL_PREFIX /usr/local)


SET(MAJOR_VERSION "1")
SET(MINOR_VERSION "0")
SET(PATCH_VERSION "0")

set(CPACK_GENERATOR DEB)
cmake_minimum_required(VERSION 2.8.3)
find_program(DPKG_SHLIBDEPS dpkg-shlibdeps)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtk2.0-0 >= 2.24.23")          

set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")

set(CPACK_PACKAGE_NAME "gcpuinfo")

set(CPACK_PACKAGE_VENDOR "Johannes Heinecke")
set(CPACK_PACKAGE_CONTACT "johannes(dot)heinecke(at)orange(dot)fr")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CPU/GPU load monitor")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n  displays in a graphic current CPU/GPU load and temperature:\n") 

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)



