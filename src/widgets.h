#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include <gtk/gtk.h>
#include <iostream>
#include <map>
#include <vector>

#include "cpuinfo.h"

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
