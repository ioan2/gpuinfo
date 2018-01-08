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
#include <sstream>

#include "gpuinfo.h"


using std::cerr;
using std::endl;
using std::ostringstream;


CallBackDataGPU::~CallBackDataGPU() {
    for (vector<FieldsGroup *>::iterator it = gpu_fgs.begin(); it != gpu_fgs.end(); ++it) {
	delete *it;
    }
}

GpuInfo::GpuInfo() {
  rtc = nvmlInit();
  devicecount = 0;
  if (rtc == NVML_SUCCESS) {
    rtc = nvmlDeviceGetCount(&devicecount);  

    cerr << devicecount << " gpu devices:" << endl;


    for(unsigned int i=0; i<devicecount;  ++i) {
      nvmlDevice_t device;
      nvmlDeviceGetHandleByIndex(i, &device);
      char name[ NVML_DEVICE_NAME_BUFFER_SIZE];
      nvmlDeviceGetName(device, name,  NVML_DEVICE_NAME_BUFFER_SIZE);
      devicenames[i] = string(name);
      cerr << "\t" << i << ": " << name << endl;
    }


    //unsigned unitcount;
    //nvmlUnitGetCount(&unitcount);
    //cerr << unitcount << endl;
  }
}



GpuInfo::~GpuInfo() {
  nvmlShutdown();
}


unsigned int GpuInfo::getFanSpeed(unsigned int index){
  unsigned int fanspeed;
  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  rtc = nvmlDeviceGetFanSpeed(device, &fanspeed);

  return fanspeed;
}

unsigned int GpuInfo::getTemperature(unsigned int index) {
  nvmlTemperatureSensors_t sensorType = NVML_TEMPERATURE_GPU;
  unsigned int temperature;
  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  rtc = nvmlDeviceGetTemperature(device, sensorType, &temperature);

  return temperature;
}

unsigned int GpuInfo::getMemoryUsage(unsigned int index) {
  nvmlMemory_t memory;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  rtc = nvmlDeviceGetMemoryInfo(device, &memory);
  
  return (1000*memory.used)/memory.total;
}

unsigned int GpuInfo::getMemory(unsigned int index) {
  nvmlMemory_t memory;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  rtc = nvmlDeviceGetMemoryInfo(device, &memory);
  
  return memory.used/1000000;
}



unsigned int GpuInfo::getUtilisation(unsigned int index) {
  nvmlUtilization_t util;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  rtc = nvmlDeviceGetUtilizationRates(device,  &util);
  
  return util.gpu;
}


#define INFOS_SIZE 10
#define PROCNAME_SIZE 100
string GpuInfo::getComputeProcess(unsigned int index) {
  nvmlProcessInfo_t infos[INFOS_SIZE];
  unsigned int infocount = INFOS_SIZE;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  //cerr << endl << "A##" << rtc << endl;
  if (rtc != 0) return ""; // TODO error

  rtc = nvmlDeviceGetComputeRunningProcesses(device, &infocount, infos);
  //cerr << endl << "B##" << rtc << endl;
  if (rtc != 0) return ""; // TODO error

  char processname[PROCNAME_SIZE];

  ostringstream msg;
  msg << "C[";
  for (unsigned int i=0; i < infocount; ++i) {
    if (i >= INFOS_SIZE) break;

    rtc = nvmlSystemGetProcessName (infos[i].pid, processname, PROCNAME_SIZE);
    //msg << "\n## " << i << " " << rtc << endl;
    if (rtc == NVML_SUCCESS) {

      msg //<< i << ": " 
	<< processname  << " (" << infos[i].pid
	<< ", " << (infos[i].usedGpuMemory/1000000) << "MB) ";
    }
  }
  msg << "]";
  return msg.str();
}


int GpuInfo::getComputeProcessNumber(unsigned int index) {
  nvmlProcessInfo_t infos[10];
  unsigned int infocount = INFOS_SIZE;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  //cerr << endl << "A##" << rtc << endl;
  if (rtc != 0) return -1; // TODO error

  rtc = nvmlDeviceGetComputeRunningProcesses(device, &infocount, infos);
  //cerr << endl << "B##" << rtc << endl;
  if (rtc != 0) return -1; // TODO error
  return infocount;
}

string GpuInfo::getGraphicsProcess(unsigned int index) {
  nvmlProcessInfo_t infos[INFOS_SIZE];
  unsigned int infocount = INFOS_SIZE;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  if (rtc != 0) return ""; // TODO error

  rtc = nvmlDeviceGetGraphicsRunningProcesses(device, &infocount, infos);
  if (rtc != 0) return ""; // TODO error

  char processname[PROCNAME_SIZE];

  ostringstream msg;
  msg << "G[";
  for (unsigned int i=0; i < infocount; ++i) {
    if (i >= INFOS_SIZE) break;

    rtc = nvmlSystemGetProcessName (infos[i].pid, processname, PROCNAME_SIZE);
    msg //<< i << ": " 
      << processname  << " (" << infos[i].pid << ", " << (infos[i].usedGpuMemory/1000000) << "MB) ";
  }
  msg << "]";
  return msg.str();
}


int GpuInfo::getGraphicsProcessNumber(unsigned int index) {
  nvmlProcessInfo_t infos[10];
  unsigned int infocount = INFOS_SIZE;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  //cerr << endl << "A##" << rtc << endl;
  if (rtc != 0) return -1; // TODO error

  rtc = nvmlDeviceGetGraphicsRunningProcesses(device, &infocount, infos);
  //cerr << endl << "B##" << rtc << endl;
  if (rtc != 0) return -1; // TODO error
  return infocount;
}


unsigned int GpuInfo::getPowerUsage(unsigned int index) {
  unsigned int power;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  
  unsigned int limit;
  rtc = nvmlDeviceGetPowerManagementLimit(device, &limit);
  rtc = nvmlDeviceGetPowerUsage(device,  &power);

  if (rtc == NVML_SUCCESS) return (100*power)/limit;
  return 0;
}

unsigned int GpuInfo::getPower(unsigned int index) {
  unsigned int power;

  rtc = nvmlDeviceGetHandleByIndex(index, &device);
  rtc = nvmlDeviceGetPowerUsage(device,  &power);

  if (rtc == NVML_SUCCESS) return power/1000;
  return 0;
}


void GpuInfo::out1(ostream &out, unsigned int index, bool graphics) {
  out << "gpu" << index <<":"
      << " fan: " << getFanSpeed(index) << "%"
      << ", temp: " << getTemperature(index) << "C"
      << ", mem: " <<  getMemoryUsage(index)/10.0 << "%"
      << ", util: " << getUtilisation(index) << "%"
      << ", process:  "<< getComputeProcess(index) << "%";
  if (graphics) {
    out << ", " << getGraphicsProcess(index) << "%";
  }
}


void GpuInfo::out2(ostream &out, unsigned int index, bool graphics) {
  out << index <<" "
      << getFanSpeed(index) << " "
      << getTemperature(index) << " "
      << getMemoryUsage(index)/10.0 << ""
      << getUtilisation(index)
    //      << ", process:  "<< getComputeProcess(index) << "%"
    ;
  //if (graphics) {
  //out << ", " << getGraphicsProcess(index) << "%";
  //  }
}

