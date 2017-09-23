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
#include "utils.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstring>
#include <cstdlib>
#include <unistd.h>


using namespace std;

CpuInfo::CpuInfo() :
    cpus(0) {
    read_proc_stat();
    for (unsigned int i = 0; i < cpus; ++i) {
	lastTemp[i] = 0;
    }

    processor = 0;
    cores = 0;

    read_proc_cpuinfo();
    read_sys();
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

void CpuInfo::read_proc_cpuinfo() {
    ifstream ifs("/proc/cpuinfo");
    string line;

    processor = 0;
    cores = 0;
    const char *lastcoreID = 0;

    //char tmp[100];
    //unsigned int ct = 0;
    while(ifs) {
	getline(ifs, line);
	//cerr << "line: " << line << endl;
	if (strncmp(line.c_str(), "processor", 9) == 0) {
	    processor++;
	    //cerr << "P " << processor << endl;
	}
	else if (strncmp(line.c_str(), "core id", 7) == 0) {
	    const char *coreID = &line.c_str()[18];
	    if (lastcoreID == 0 || strcmp(lastcoreID, coreID) != 0) {
		cores++;
		//cerr << "C " << cores << endl;
	    }
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
	if (strncmp(line.c_str(), "cpu", 3) == 0// && line[3] != ' '
	    ) {
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


void CpuInfo::read_sys() {
    int i = 0;

    if (path.empty()) {
	// find the correct directory with core temperature information
	for (; i < 4; ++i) {
	    ostringstream msg;
	    msg << "/sys/class/hwmon/hwmon" << i << "/device/name";

	    string line;
	    if (!readSingleLineFile(msg.str(), line)) {
		msg.str("");
		msg << "/sys/class/hwmon/hwmon" << i << "/name";

		if (!readSingleLineFile(msg.str(), line)) continue;
	    }

	    if (line == "coretemp") {
		path = msg.str().substr(0, msg.str().size()-4); // do not take the name bit
		break;
	    }
	} 
    }

    if (!path.empty()) {
	// we found the directory, now we read the temperature for cores
	int j = 1;
	countCores = 1; // first cpu column is the total, so no equivalent for temperature
	while(true) {
	    ostringstream msg;
	    msg << path << "temp" <<j << "_label";

	    string line;
	    if (!readSingleLineFile(msg.str(), line)) break; // no more files expected

	    if (line.compare(0, 4, "Core") == 0) {
		ostringstream msg2;
		msg2 << path << "temp" <<j << "_input";

		if (readSingleLineFile(msg2.str(), line)) {
		    lastTemp[countCores] = atoi(line.c_str());
		}
		countCores++;
	    }
	    j++;
	    //if (countCores == 3) break; ///////
	}
    }
}

void CpuInfo::getCoreTemp(unsigned int *temp) {
    read_sys();
    for (unsigned int i = 0; i < cpus; ++i) {
	temp[i] = lastTemp[i];
    }    
}
