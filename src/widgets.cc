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


#include "widgets.h"
#include "utils.h"

using namespace std;

InfoField::InfoField(const char *name, const string &format, bool vertical, bool pg) {
    if (vertical)
	box = gtk_vbox_new(FALSE, 1);
    else
	box = gtk_hbox_new(FALSE, 1);
    this->format = format;
    this->pg = pg;

    if (name != 0) {
	label = gtk_label_new(name);
	if (!vertical) {
	    gtk_label_set_angle(GTK_LABEL(label), 90);
	    gtk_widget_set_size_request(label, -1, 50);
	} else {
	}
	// parent, widget, expand, fill, padding
	gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 1);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_widget_show (label);
    }

    if (pg) {
	info = gtk_progress_bar_new ();
	gtk_widget_set_size_request(info, 50, -1);
	if (!vertical)
	    gtk_progress_bar_set_orientation((GtkProgressBar *)info, GTK_PROGRESS_BOTTOM_TO_TOP);

	//gtk_progress_bar_set_bar_style((GtkProgressBar *)info, GTK_PROGRESS_DISCRETE);
    }
    else {
	info = gtk_button_new();
	if (vertical)
	    gtk_widget_set_size_request(info, 100, -1);
	else
	    gtk_widget_set_size_request(info, 100, -1);
    }

    gtk_box_pack_start (GTK_BOX(box), info, TRUE, TRUE, 1);
    gtk_widget_show(info);

    setValue(0);
}

void InfoField::setValue(float val) {
    sprintf(tmp, format.c_str(), val);
    if (pg) {
	gtk_progress_bar_set_text((GtkProgressBar *)info, tmp);
	gtk_progress_bar_set_fraction((GtkProgressBar *)info, val/100.0);
    } else {
	gtk_button_set_label((GtkButton*)info, tmp);

	GdkColor color;
	getHeatMapColor(val/100.0, color);
	gtk_widget_modify_bg(info, GTK_STATE_NORMAL, &color);
    }
}

void InfoField::setValue(float val, unsigned int val2) {
    sprintf(tmp, format.c_str(), val, val2);
    if (pg) {
	gtk_progress_bar_set_text((GtkProgressBar *)info, tmp);
	gtk_progress_bar_set_fraction((GtkProgressBar *)info, val/100.0);
    } else {
	gtk_button_set_label((GtkButton*)info, tmp);

	GdkColor color;
	getHeatMapColor(val/100.0, color);
	gtk_widget_modify_bg(info, GTK_STATE_NORMAL, &color);
    }
}


void InfoField::setValue(float val, unsigned int val2, unsigned int val3) {
    sprintf(tmp, format.c_str(), val, val2, val3);
    if (pg) {
	gtk_progress_bar_set_text((GtkProgressBar *)info, tmp);
	gtk_progress_bar_set_fraction((GtkProgressBar *)info, val/100.0);
    } else {
	gtk_button_set_label((GtkButton*)info, tmp);

	GdkColor color;
	getHeatMapColor(val/100.0, color);
	gtk_widget_modify_bg(info, GTK_STATE_NORMAL, &color);
    }
}





FieldsGroup::FieldsGroup(const char *name, bool vertical) {
    box = (vertical ? gtk_hbox_new (FALSE, 1) : gtk_vbox_new (FALSE, 1));
    if (name != 0) {
	label = gtk_label_new(name);
	// parent, widget, expand, fill, padding
	gtk_box_pack_start(GTK_BOX (box), label, FALSE, FALSE, 1);
	gtk_widget_show(label);
    }
}

FieldsGroup::~FieldsGroup() {
    for (map<int, InfoField *>::iterator it = ifds.begin(); it != ifds.end(); ++ it) {
	delete it->second;
    }
}

void FieldsGroup::add(int ix, InfoField *ifd) {
    ifds[ix] = ifd;
    gtk_box_pack_start (GTK_BOX (box), ifd->getWidget(), TRUE, TRUE, 1);    
}

void FieldsGroup::add(int ix, GtkWidget *ifd) {
    gtk_box_pack_start (GTK_BOX (box), ifd, FALSE, FALSE, 1);    
}


void FieldsGroup::setValue(int ix, float val) {
    map<int, InfoField *>::iterator it = ifds.find(ix);
    if (it != ifds.end()) {
	it->second->setValue(val);
    }
}
void FieldsGroup::setValue(int ix, float val, unsigned int val2) {
    map<int, InfoField *>::iterator it = ifds.find(ix);
    if (it != ifds.end()) {
	it->second->setValue(val, val2);
    }
}
void FieldsGroup::setValue(int ix, float val, unsigned int val2, unsigned int val3) {
    map<int, InfoField *>::iterator it = ifds.find(ix);
    if (it != ifds.end()) {
	it->second->setValue(val, val2, val3);
    }
}



CallBackDataCPU::~CallBackDataCPU() {
    for (vector<FieldsGroup *>::iterator it = cpu_fgs.begin(); it != cpu_fgs.end(); ++it) {
	delete *it;
    }
    for (vector<FieldsGroup *>::iterator it = core_fgs.begin(); it != core_fgs.end(); ++it) {
	delete *it;
    }
}

CallBackDataGPU::~CallBackDataGPU() {
    for (vector<FieldsGroup *>::iterator it = gpu_fgs.begin(); it != gpu_fgs.end(); ++it) {
	delete *it;
    }
}
