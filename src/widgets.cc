#include "widgets.h"
#include "utils.h"

using namespace std;

//#define PROGRESSBAR

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

    //#ifdef PROGRESSBAR
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
	    gtk_widget_set_size_request(info, 80, -1);
	else
	    gtk_widget_set_size_request(info, 80, -1);
    }
    //#endif

    gtk_box_pack_start (GTK_BOX(box), info, TRUE, TRUE, 1);
    gtk_widget_show(info);

    setValue(0);
}

void InfoField::setValue(float val) {
    sprintf(tmp, format.c_str(), val);
    //cerr << "zzz " << val << " " << tmp << endl;
    //#ifdef PROGRESSBAR
    if (pg) {
    gtk_progress_bar_set_text((GtkProgressBar *)info, tmp);
    gtk_progress_bar_set_fraction((GtkProgressBar *)info, val/100.0);
    //#else
    } else {
    gtk_button_set_label((GtkButton*)info, tmp);

    GdkColor color;
    getHeatMapColor(val/100.0, color);
    gtk_widget_modify_bg(info, GTK_STATE_NORMAL, &color);
    //#endif
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



CallBackDataCPU::~CallBackDataCPU() {
    for (vector<FieldsGroup *>::iterator it = cpu_fgs.begin(); it != cpu_fgs.end(); ++it) {
	delete *it;
    }
    for (vector<FieldsGroup *>::iterator it = core_fgs.begin(); it != core_fgs.end(); ++it) {
	delete *it;
    }
}
