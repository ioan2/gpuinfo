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


#ifndef __CPUINFO_H__
#define __CPUINFO_H__

#include <iostream>

using std::ostream;
using std::endl;

#define MAX_CPU 128

/** class to read and process cpu information from /proc/stat */
class CpuInfo {
 public:
    CpuInfo();
    ~CpuInfo();

    /** get number of available CPUs */
    unsigned int getCpus() { return cpus; }

    /** get CPU time for user, system, nice and idle in per mille
	@param user array of unsigned int of at least the number of cpus available
	@param system array of unsigned int of at least the number of cpus available
	@param niced array of unsigned int of at least the number of cpus available
	@param idle array of unsigned int of at least the number of cpus available
    */
    void getCPUtime(unsigned int *user, unsigned int *system, unsigned int *niced, unsigned int *idle);

    /** simple text output of cpu status */
    void out(ostream &out);

 private:
    void read_proc_stat();
    /// number of CPUs available
    unsigned int cpus; 

    /// stocks last /proc/stat entry
    unsigned int lastUser[MAX_CPU];
    unsigned int lastSystem[MAX_CPU];
    unsigned int lastNiced[MAX_CPU];
    unsigned int lastIdle[MAX_CPU];

    /// stocks delta between the last and current /proc/stat entry
    unsigned int diffUser[MAX_CPU];
    unsigned int diffSystem[MAX_CPU];
    unsigned int diffNiced[MAX_CPU];
    unsigned int diffIdle[MAX_CPU];
    unsigned int diff_sum[MAX_CPU];
};

#endif
