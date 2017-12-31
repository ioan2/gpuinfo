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
#   * Neither the name of the copyright holder nor the names of its contributors 
#     may be used to endorse or promote products derived from this software
#     without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, 
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Author: Johannes Heinecke
# Version:  1.1 as of 27th December 2017


set(gcpuinfo2_src
  utils.cc
  utils.h
  cpuinfo.cc
  cpuinfo.h
  widgets.h
  widgets.cc
  )
add_executable (gcpuinfo ${gcpuinfo2_src} gcpuinfo.cc)
target_link_libraries(gcpuinfo ${GTK2_LIBRARIES})
install (TARGETS gcpuinfo DESTINATION bin)



if (CUDA_FOUND)
  set(ggpuinfo_src
    utils.cc
    gpuinfo.cc
    utils.h
    gpuinfo.h
    widgets.h
    widgets.cc
)
  add_executable (ggpuinfo ${ggpuinfo_src} ggpuinfo.cc)
  target_link_libraries(ggpuinfo ${GTK2_LIBRARIES} ${LIBNVIDIA_ML})
  install (TARGETS ggpuinfo DESTINATION bin)
endif()


