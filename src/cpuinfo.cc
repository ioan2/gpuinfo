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

#include "cpuinfo.h"

#include <iostream>
#include <fstream>

#include <cstring>
#include <cstdlib>
#include <unistd.h>


using namespace std;

CpuInfo::CpuInfo() :
    cpus(0) {
    read_proc_stat();
}

CpuInfo::~CpuInfo() {
}


void CpuInfo::out(ostream &out) {
    unsigned int u[MAX_CPU], s[MAX_CPU], n[MAX_CPU], i[MAX_CPU];
    getCPUtime(u, s, n, i);
    for (unsigned int x = 0; x <cpus; ++x) {
	if (x != 0) out << "\t";
	out << "cpu" << x << ": " << u[x]/10.0 << "%";
    }
}


void CpuInfo::getCPUtime(unsigned int *user, unsigned int *system, unsigned int *niced, unsigned int *idle) {
    read_proc_stat();

    for (unsigned int i = 0; i < cpus; ++i) {
	if (diff_sum[i] != 0) {
	    user[i] = 1000*diffUser[i]/diff_sum[i];
	    //cerr << "zzzz " << i << " " <<diffUser[i] << " " <<diff_sum[i] << endl;
	    system[i] = 1000*diffSystem[i]/diff_sum[i];
	    niced[i] = 1000*diffNiced[i]/diff_sum[i];
	    idle[i] = 1000*diffIdle[i]/diff_sum[i];
	} else {
	    user[i] = 0;
	    system[i] = 0;
	    niced[i] = 0;
	    idle[i] = 0;
	}
    }
}

void CpuInfo::read_proc_stat() {
    ifstream ifs("/proc/stat");
    string line;

    char tmp[100];
    unsigned int ct = 0;
    while(ifs) {
	getline(ifs, line);
	if (strncmp(line.c_str(), "cpu", 3) == 0 && line[3] != ' ') {
	    strcpy(tmp, &line.c_str()[5]);
	    char *saveptr;
	    char *token = strtok_r(tmp, " ", &saveptr);
	    unsigned val = atoi(token);
	    //cerr << "v:" << val << endl;
	    diffUser[ct] = val - lastUser[ct];
	    lastUser[ct] = val;
	    diff_sum[ct] = diffUser[ct];

	    token = strtok_r(0, " ", &saveptr);
	    val = atoi(token);
	    //cerr << "v:" << val << endl;
	    diffNiced[ct] = val - lastNiced[ct];
	    lastNiced[ct] = val;
	    diff_sum[ct] += diffNiced[ct];

	    token = strtok_r(0, " ", &saveptr);
	    val = atoi(token);
	    //cerr << "v:" << val << endl;
	    diffSystem[ct] = val - lastSystem[ct];
	    lastSystem[ct] = val;
	    diff_sum[ct] += diffSystem[ct];

	    token = strtok_r(0, " ", &saveptr);
	    val = atoi(token);	    
	    //cerr << "v:" << val << endl;
	    diffIdle[ct] = val - lastIdle[ct];
	    lastIdle[ct] = val;
	    diff_sum[ct] += diffIdle[ct];

	    //cerr << ct << ": " << diffUser[ct] << " " << diffSystem[ct] << " " << diffNiced[ct] << " " << diffIdle[ct] << " " << diff_sum[ct] << endl;
	    ct++;
	    if (ct >= MAX_CPU) break;
	}
	if (strncmp(line.c_str(), "cpu", 3) != 0) {
	    break;
	}
    }
    if (cpus == 0) cpus = ct;
    ifs.close();
}

