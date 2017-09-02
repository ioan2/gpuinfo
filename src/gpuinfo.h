/** This library is under the 3-Clause BSD License

Copyright (c) 2017, Johannes Heinecke

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  3. Neither the name of the copyright holder nor the names of its contributors 
     may be used to endorse or promote products derived from this software without
     specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Author: Johannes Heinecke
 Version:  1.0 as of 2nd September 2017
*/

#include <iostream>
#include <map>

#ifdef _cplusplus
extern "C" {
#endif
  #include <nvml.h>
#ifdef _cplusplus
}
#endif

using std::map;
using std::string;
using std::ostream;

/** class to get information about the present GPUs */
class GpuInfo {
 public:
  GpuInfo();
  ~GpuInfo();

  /** return fanspeed (% of max) */
  unsigned int getFanSpeed(unsigned int index);
  /** return gpu temperature in C */
  unsigned int getTemperature(unsigned int index);
  /** return memory usage in permille of total */
  unsigned int getMemoryUsage(unsigned int index);
  /** return used memory in MB */
  unsigned int getMemory(unsigned int index);
  /** return utilisation in % */
  unsigned int getUtilisation(unsigned int index);
  /** return power usage in % of total */
  unsigned int getPowerUsage(unsigned int index);
  /** return power usage in W */
  unsigned int getPower(unsigned int index);

  /** get number of compute processes on gpu device */
  int getComputeProcessNumber(unsigned int index);
  /** get names of compute processes */
  string getComputeProcess(unsigned int index);
  /** get number of graphics processes on gpu device */
  int getGraphicsProcessNumber(unsigned int index);
  /** get names of graphics processes */
  string getGraphicsProcess(unsigned int index);

  /** get number of gpu devices */
  unsigned int getDevices() const { return devicecount; } 

  /** verbose one-line output */
  void out1(ostream &out, unsigned int index, bool graphics);
  /** dense one line output */
  void out2(ostream &out, unsigned int index, bool graphics);



 private:
  // the device currently interrogated */
  nvmlDevice_t device;

  // map of device index with device name */
  map<unsigned int, string>devicenames;

  nvmlReturn_t rtc;

  // number of devices present
  unsigned int devicecount;
};



