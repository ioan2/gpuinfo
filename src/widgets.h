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
 Version:  1.1 as of 27th December 2017
*/

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include <gtk/gtk.h>
#include <iostream>
#include <map>
#include <vector>

#include "cpuinfo.h"
//#include "gpuinfo.h"

using std::vector;
using std::map;
using std::string;

/** displays an information with a label */
class InfoField {
 public:
    InfoField(const char *name, const string &format, bool vertical=false, bool pg=false);

    /** return widget which contains the information */
    GtkWidget *getInfoWidget() {
	return info;
    };

    /** return main widget in order to pack it somewhere */
    GtkWidget *getWidget() {
	return box;
    };

    /** set the text displayed */
    void setValue(float val);
    void setValue(float val, unsigned int val2);
    void setValue(float val, unsigned int val2, unsigned int val3);

 private:
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *info;
    string format;
    char tmp[50]; // to format info
    bool pg;
};

/** groups some fields of information */
class FieldsGroup {
 public:
    FieldsGroup(const char *name, bool vertical=false);
    ~FieldsGroup();
    /** add an infofield to the fieldsgroup */
    void add(int ix, InfoField *ifd);
    void add(int ix, GtkWidget *ifd);

    /** return main widget in order to pack it somewhere */
    GtkWidget *getWidget() {
	return box;
    };

    /** set the text displayed */
    void setValue(int ix, float val);
    void setValue(int ix, float val, unsigned int val2);
    void setValue(int ix, float val, unsigned int val2, unsigned int val3);
    
 private:
    GtkWidget *box;
    GtkWidget *label;
    map<int, InfoField *>ifds;
};


class CallBackDataCPU {
 public:
    CallBackDataCPU() {};
    ~CallBackDataCPU();

    CpuInfo *ci;
    vector<FieldsGroup *>cpu_fgs;
    vector<FieldsGroup *>core_fgs;

};




#endif
