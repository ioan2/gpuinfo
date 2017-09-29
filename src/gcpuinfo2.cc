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
 Version:  1.0 as of 24th September 2017
*/

#include <iostream>
#include <map>
#include <vector>

#include <cstring>
#include <cstdlib>


#include <gtk/gtk.h>

#include "cpuinfo.h"
#include "utils.h"
#include "widgets.h"

#define APPNAME "cgpuinfo"


using namespace std;


/** structure to give information to callback */
//struct cb_data {
//    CpuInfo *ci;
//   vector<FieldsGroup *>fgs;
    //vector<const char *>infos;
    //map<const char *, vector<GtkWidget *> > labels;
//};

/* TODO:
   process cpu - core correctly (which cpu on which core)
 */

/** used to update labels */
gboolean time_handler(CallBackDataCPU *cbdata) {
    unsigned int u[MAX_CPU], s[MAX_CPU], n[MAX_CPU], i[MAX_CPU], t[MAX_CPU];
    cbdata->ci->getCPUtime(u, s, n, i);
    cbdata->ci->getCoreTemp(t);
    
    unsigned int x = 0;
    for (vector<FieldsGroup *>::iterator it = cbdata->cpu_fgs.begin(); it != cbdata->cpu_fgs.end(); ++it, ++x) {
	// pour tous les FieldsGroup des CPU
	(*it)->setValue('u', u[x]/10.0);
	(*it)->setValue('n', n[x]/10.0);
	(*it)->setValue('s', s[x]/10.0);
    }

    x = 1;
    for (vector<FieldsGroup *>::iterator it = cbdata->core_fgs.begin(); it != cbdata->core_fgs.end(); ++it, ++x) {

	(*it)->setValue('t', t[x]/1000.0);
    }

    return TRUE;
}


int main(int argc, char *argv[]) {
    if (argc <= 1) {
	cerr << argv[0] << " [options]" << endl
	     << "options:" << endl
	     << "  -d  update delay (default 500ms)" << endl
	     << "  -T  textual output" << endl
	     << "\n without -T" <<endl
	     << "  --nn do not show niced" << endl
	     << "  -v vertical layout" << endl
	     << "  -c compact view (no labels)" << endl
	     << " -p progressbar view" << endl
	     << "\n with -T" <<endl
	     << "  -t  add time stamp" << endl
	     << "  -l  log mode" << endl
	     << endl;
	;
    }


    unsigned int delay = 1000;
    bool textonly = false;
    bool logmode = false;
    bool timest = false;
    bool showniced = true;
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
	} else if (strcmp(argv[i], "-c") == 0) {
	    compact = true;
	} else if (strcmp(argv[i], "-p") == 0) {
	    progressbar = true; 
	} else if (strcmp(argv[i], "-v") == 0) {
	    vertical = true; 
	} else if (strcmp(argv[i], "--nn") == 0) {
	    showniced = false;
	} else if (strcmp(argv[i], "-l") == 0) {
	    logmode = true;
	} else if (strcmp(argv[i], "-t") == 0) {
	    timest = true;
	} else {
	    cerr << "invalid option: " << argv[i] << endl;
	    break;
	}
    }


    CpuInfo ci;
    unsigned int cpus = ci.getCpus(); // cpu number + 1 (total)
    unsigned int cores = ci.getCores(); // core number + 1

    unsigned int cpus_par_core = (cpus-1)/(cores-1);

    if (!textonly) {
	// initialise cbdata
	CallBackDataCPU cbdata;
	cbdata.ci = &ci;

	gtk_init(&argc, &argv);

	//printf("GTK+ version: %d.%d.%d\n", gtk_major_version, gtk_minor_version, gtk_micro_version);
	//printf("Glib version: %d.%d.%d\n", glib_major_version, glib_minor_version, glib_micro_version);

	//PangoFontDescription *pfd = pango_font_description_from_string("Serif 12");


	// main window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
	gtk_window_set_title(GTK_WINDOW(window), APPNAME);
	//gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 1);

	// create main table (a column per core)
	GtkWidget *maintable;
	if (vertical)
	    maintable = gtk_table_new(cpus, 2, FALSE); // rows, columns, homogenuous
	else {
	    // first column for headers
	    maintable = gtk_table_new(2, cpus+1, FALSE); // rows, columns, homogenuous
	}
	gtk_table_set_row_spacings(GTK_TABLE(maintable), 2);
	gtk_table_set_col_spacings(GTK_TABLE(maintable), 2);
	gtk_container_add(GTK_CONTAINER(window), maintable);

	char tmp[70];

// 	// create headers
//   	if (!compact) {
//   	    GtkWidget *label = gtk_label_new("temperature");
// 	    gtk_label_set_angle(GTK_LABEL(label), 90);
//   	    // left, right, top, bottom
//   	    gtk_table_attach_defaults(GTK_TABLE(maintable), label, 
//   				      0, 1,
//   				      0, 1);
//   	}

	// create core infos
	int offset = 2; // headers, total column
	for (unsigned int i = 0; i < cores-1; ++i) {
	    sprintf(tmp, "core %d:", i);
	    FieldsGroup *fg = new FieldsGroup(tmp, vertical);
	    cbdata.core_fgs.push_back(fg);
	    const char *name = 0;
	    if (!compact && i == 0) name = "temperature";

	    fg->add('t', new InfoField(name, "%.fC", vertical, progressbar));
	    fg->setValue('t', 40);

	    if (vertical) {
		// left, right, top, bottom
		gtk_table_attach_defaults(GTK_TABLE(maintable), fg->getWidget(),
					  0, 1, 
					  offset + i*cpus_par_core, offset + (i+1)*cpus_par_core);
	    } else {
		// left, right, top, bottom

		gtk_table_attach_defaults(GTK_TABLE(maintable), fg->getWidget(), 
					  offset + i*cpus_par_core, offset + (i+1)*cpus_par_core,
					  0, 1);
	    }
	    
	}

	//cerr << "cpc " << cpus_par_core << endl;
	// create cpu infos
	for (unsigned int i = 0; i < cpus; ++i) {
	    if (i == 0) sprintf(tmp, "total:");
	    else {
		sprintf(tmp, "cpu %d:", i-1);
		//cerr << i << " core: " << ci.getCore(i-1) << endl;
	    }
	    int currentcore = ci.getCore(i-1);
	    int offset = 1;
	    int column = offset; // with headers
	    if (i != 0) column = offset + 1 + currentcore*cpus_par_core +  (i-1)/(cores-1);

	    //cerr << i << " " << (i-1) % 2<< " xxx: " << " " << currentcore << " " << column << endl;
	    FieldsGroup *fg = new FieldsGroup(tmp, vertical);
	    cbdata.cpu_fgs.push_back(fg);
	    if (vertical) {
		// left, right, top, bottom
		gtk_table_attach_defaults(GTK_TABLE(maintable), fg->getWidget(), 1, 2, column, column+1);
	    } else {
		// left, right, top, bottom
		gtk_table_attach_defaults(GTK_TABLE(maintable), fg->getWidget(), column, column+1, 1, 2);
	    }
	    const char *name = 0;
	    if (!compact && i == 0) name = "user";
	    fg->add('u', new InfoField(name, "%.1f%%", vertical, progressbar));
	    //fg->setValue('u', 20.0);

	    if (showniced) {
		if (!compact && i == 0) name = "niced";
		fg->add('n', new InfoField(name, "%.1f%%", vertical, progressbar));
		fg->setValue('n', 40);
	    }
	    if (!compact && i == 0) name = "system";
	    fg->add('s', new InfoField(name, "%.1f%%", vertical, progressbar));
	    fg->setValue('s', 30);
	}

	//pango_font_description_free(pfd);
	
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
	while(true) {
	    if (timest) {
		timestamp(cerr);	  
	    }

	    if (!logmode) {
		cerr << '\r';
		for (unsigned int i = 0; i < cpus; ++i) {
		    cerr << "          \t";
		}
		cerr << '\r';
	    }
	    ci.out(cerr);
	    if (logmode)
		cerr <<endl;
	    usleep(delay*1000);
	}

    }
}
