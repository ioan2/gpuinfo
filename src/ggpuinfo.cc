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

#include <iostream>
#include <map>
#include <vector>

#include <cstring>
#include <cstdlib>

#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <gtk/gtk.h>

#include "gpuinfo.h"
#include "utils.h"
#include "widgets.h"

#define APPNAME "ggpuinfo"


using namespace std;


/** structure to give information to callback */
struct cb_data {
  GpuInfo *gi;
  map<const char *, vector<GtkWidget *> >labels;
};




/** names of pieces information */
const char *infos[] = { "memory usage", "utilisation", "temperature", "fanspeed", "power usage", NULL };
const unsigned int infolen = 5;

/** used to update labels */
gboolean time_handler(CallBackDataGPU *cbdata) {
    // for each gpu device

    unsigned int x = 0;
    for (vector<FieldsGroup *>::iterator it = cbdata->gpu_fgs.begin(); it != cbdata->gpu_fgs.end(); ++it, ++x) {
	unsigned int val = cbdata->gi->getMemoryUsage(x)/10;
	int c = cbdata->gi->getMemory(x);
	(*it)->setValue('m', val, c);

	val = (int)cbdata->gi->getUtilisation(x);
	c = cbdata->gi->getComputeProcessNumber(x);
	int g = cbdata->gi->getGraphicsProcessNumber(x);
	(*it)->setValue('u', val, c, g);

	val = cbdata->gi->getTemperature(x);
	(*it)->setValue('t', val);

	val = cbdata->gi->getFanSpeed(x);
	(*it)->setValue('f', val);

	val = cbdata->gi->getPowerUsage(x);
	c = cbdata->gi->getPower(x);
	(*it)->setValue('p', val, c);

    }
    return TRUE;
}





int main(int argc, char *argv[]) {
    if (argc <= 1) {
	cerr << argv[0] << " options" << endl
	     << "  -d  update delay (default 500ms)" << endl
	     << "  -v vertical layout" << endl
	     << "  -p progressbar view" << endl
	     << "  -c compact view (no labels)" << endl
	     << "  -T  textual output" << endl
	     << "\n with -T" <<endl
	     << "  -a  show also devices with 0% utilisation" << endl
	     << "  -g  show also graphics processes" << endl
	     << "  -s  short output (id Fanspeed/Temperature/Memory Usage/GPU Utilisation)" << endl
	     << "  -t  add time stamp" << endl
	     << "  -l  log mode" << endl
	     << endl
	    ;
    }

    unsigned int delay = 500;
    bool all = false;
    bool graphics = false;
    bool shortoutput = false;
    bool logmode = false;
    bool timest = false;
    bool textonly = false;
    bool vertical = false;
    bool progressbar = false;
    bool compact = false;

    for (int i = 1; i<argc; ++i) {
	if (strcmp(argv[i], "-d") == 0 && i < argc-1) {
	    unsigned int d = atoi(argv[i+1]);
	    if (d < 1) {
		cerr << "invalid option: " << argv[i] << " " << argv[i+1] << endl;
	    } else {
		++i;
		delay = d;
	    }

	} else if (strcmp(argv[i], "-T") == 0) {
	    textonly = true;
	} else if (strcmp(argv[i], "-a") == 0) {
	    all = true;
	} else if (strcmp(argv[i], "-g") == 0) {
	    graphics = true;
	} else if (strcmp(argv[i], "-s") == 0) {
	    shortoutput = true;
	} else if (strcmp(argv[i], "-l") == 0) {
	    logmode = true;
	} else if (strcmp(argv[i], "-t") == 0) {
	    timest = true;
	} else if (strcmp(argv[i], "-p") == 0) {
	    progressbar = true;
	} else if (strcmp(argv[i], "-c") == 0) {
	    compact = true;
	} else if (strcmp(argv[i], "-v") == 0) {
	    vertical = true;
	} else {
	    cerr << "invalid option: " << argv[i] << endl;
	    break;
	}
    }


    GpuInfo gi;
    unsigned int devnum = gi.getDevices();    


    if (!textonly) {
	// initialise cbdata
	CallBackDataGPU cbdata;
	cbdata.gi = &gi;

	gtk_init(&argc, &argv);

	//printf("GTK+ version: %d.%d.%d\n", gtk_major_version, gtk_minor_version, gtk_micro_version);
	//printf("Glib version: %d.%d.%d\n", glib_major_version, glib_minor_version, glib_micro_version);

	// main window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
	gtk_window_set_title(GTK_WINDOW(window), APPNAME);
	//gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);

	// create table (a column per gpu device)
	GtkWidget *table;
	if (vertical) {
	    table = gtk_table_new(devnum, 1, TRUE); // rows, columns, homogenuous
	} else {
	    table = gtk_table_new(1, devnum, TRUE); // rows, columns, homogenuous
	}
	gtk_table_set_row_spacings(GTK_TABLE(table), 2);
	gtk_table_set_col_spacings(GTK_TABLE(table), 2);
	gtk_container_add(GTK_CONTAINER(window), table);

	char tmp[70];
	for (unsigned int i = 0; i < devnum; ++i) {
	    sprintf(tmp, "gpu: %d", i);
	    FieldsGroup *fg = new FieldsGroup(tmp, vertical);
	    cbdata.gpu_fgs.push_back(fg);

	    const char *name = 0;
	    if (!compact) {
		if (!vertical) name = "mem";
		else name = "memory usage";
	    }
	    fg->add('m', new InfoField(name, "%.f%% (%dMB)", vertical, progressbar));

	    if (!compact) {
		if (!vertical) name = "use";
		else name = "utilisation";
	    }
	    fg->add('u', new InfoField(name, "%.f%% (C:%d, G:%d)", vertical, progressbar));

	    if (!compact) {
		if (!vertical) name = "temp";
		else name = "temperature";
	    }
	    fg->add('t', new InfoField(name, "%.fC", vertical, progressbar));

	    if (!compact) {
		if (!vertical) name = "fan";
		else name = "fanspeed";
	    } 
	    fg->add('f', new InfoField(name, "%.f%%", vertical, progressbar));

	    if (!compact) {
		if (!vertical) name = "power";
		else name = "power usage";
	    }
	    fg->add('p', new InfoField(name, "%.f (%dW)", vertical, progressbar));

	    if (vertical) {
		// left, right, top, bottom
		gtk_table_attach_defaults(GTK_TABLE(table), fg->getWidget(),
					  0, 1, 
					  i, i+1);
	    } else {
		// left, right, top, bottom
		gtk_table_attach_defaults(GTK_TABLE(table), fg->getWidget(), 
					  i, i+1,
					  0, 1);
	    }
	}
      
	// destroy applicatation when window is closed
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));
	//g_signal_connect(window, "expose-event",  G_CALLBACK(on_expose_event), NULL);

	// set first values
	time_handler(&cbdata);

	// update values every <delay> milliseconds
	g_timeout_add(delay, (GSourceFunc) time_handler, &cbdata);

	// draw window
	gtk_widget_show_all(window);

	// loop
	gtk_main();
    }


    else {
	while(1) {
	    //cerr << "eee" << gi.getDevices() << endl;
	    if (!logmode) {
		cerr << "\r";
		for (unsigned int i = 0; i < gi.getDevices(); ++i) {
		    cerr << "                                            ";
		}
		cerr << "\r";
	    }

	    bool first = true;
	    for (unsigned int i = 0; i < gi.getDevices(); ++i) {
		unsigned int util = gi.getUtilisation(i);
		//cerr << "eee " << i << " " << util << endl;
		if (all || util > 1) {
		    if (! first) cerr << "\t";
		    else first = false;

		    if (timest){
			timestamp(cerr);	  
		    }
		    if (shortoutput)
			gi.out2(cerr, i, graphics);
		    else {
			//cerr << "lll" << endl;
			gi.out1(cerr, i, graphics);
		    }
		}
	    }
	    if (logmode) cerr << endl;
	    usleep(delay*1000);
	}

    }

    return 0;
}
