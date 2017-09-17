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
#include <map>
#include <vector>

#include <cstring>
#include <cstdlib>


#include <gtk/gtk.h>

#include "cpuinfo.h"
#include "utils.h"

#define APPNAME "cgpuinfo"


using namespace std;


/** structure to give information to callback */
struct cb_data {
    CpuInfo *ci;
    vector<const char *>infos;
    map<const char *, vector<GtkWidget *> > labels;
};

/* TODO:
   process cpu - core correctly (which cpu on which core)
 */

/** used to update labels */
gboolean time_handler(struct cb_data *cbdata) {
    unsigned int u[MAX_CPU], s[MAX_CPU], n[MAX_CPU], i[MAX_CPU], t[MAX_CPU];
    cbdata->ci->getCPUtime(u, s, n, i);
    cbdata->ci->getCoreTemp(t);
    
    unsigned int cpus = cbdata->ci->getCpus();

    // for each gpu device
    for(unsigned int i = 0; i < cpus; ++i) {
	char tmp[100];

	// for each piece of information
	for (unsigned x = 0; x < cbdata->infos.size(); ++x) {
	    // find label in cbdata
	    if (cbdata->infos[x][0] == 't' && i >= cbdata->ci->getCores()) {
		// TODO can be much improved
		// temperature is by core not by cpu, so top after all cores have their temp
		continue;
	    }
	    GtkWidget *label = cbdata->labels[cbdata->infos[x]][i];

	    unsigned int val = 0;
	    // get correct values to display
	    switch (cbdata->infos[x][0]) {
	    case 'u':
		sprintf(tmp, "%.1f%%", u[i]/10.0);
		val = u[i];
		break;

	    case 'n':
		sprintf(tmp, "%.1f%%", n[i]/10.0);
		val = n[i];
		break;

	    case 's':
		sprintf(tmp, "%.1f%%", s[i]/10.0);
		val = s[i];
		break;

	    case 't':
		sprintf(tmp, "%dC", t[i]/1000);
		val = t[i]/100;
		break;

	    }

	    // update button label (we use buttons, since labels do not have background colors)
	    gtk_button_set_label((GtkButton*)label, tmp);

	    // update background colour
	    GdkColor color;
	    getHeatMapColor(val/1000.0, color);
	    gtk_widget_modify_bg(label, GTK_STATE_NORMAL, &color);

	    //GdkColor color;
	    //gdk_color_parse ("white", &color);
	    //gtk_widget_modify_text(label, GTK_STATE_NORMAL, &color);

	}
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
    unsigned int cpus = ci.getCpus();
   
    if (!textonly) {
	// initialise cbdata
	struct cb_data cbdata;
	cbdata.ci = &ci;
	cbdata.infos.push_back("user");
	if (showniced) cbdata.infos.push_back("niced");
	cbdata.infos.push_back("system");
	cbdata.infos.push_back("temperature");

	gtk_init(&argc, &argv);

	//printf("GTK+ version: %d.%d.%d\n", gtk_major_version, gtk_minor_version, gtk_micro_version);
	//printf("Glib version: %d.%d.%d\n", glib_major_version, glib_minor_version, glib_micro_version);


	// main window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
	gtk_window_set_title(GTK_WINDOW(window), APPNAME);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);

	// create main table (a column per core)
	GtkWidget *maintable;
	if (vertical)
	    maintable = gtk_table_new(ci.getCores(), 1, FALSE); // rows, columns, homogenuous
	else
	    maintable = gtk_table_new(1, ci.getCores(), FALSE); // rows, columns, homogenuous
	gtk_table_set_row_spacings(GTK_TABLE(maintable), 5);
	gtk_table_set_col_spacings(GTK_TABLE(maintable), 5);
	gtk_container_add(GTK_CONTAINER(window), maintable);


	
	char tmp[70];

	vector<GtkWidget *>coretables;
	unsigned int cpus_per_core = (cpus-1)/(ci.getCores()-1);

	// create a frame for each core and put it to the maintable
	for (unsigned int co = 0; co < ci.getCores(); ++co) {
	    if (co == 0) //sprintf(tmp, "total");
		tmp[0] = '\0';
	    else sprintf(tmp, "core: %d", co-1);
	    GtkWidget *coreframe = gtk_frame_new(tmp);
	    gtk_frame_set_shadow_type(GTK_FRAME(coreframe), GTK_SHADOW_ETCHED_IN);
	    // put core frame to main table
	    if (vertical)
		gtk_table_attach_defaults(GTK_TABLE(maintable), coreframe, 0, 1, co, co+1); // left, right, top, bottom
	    else
		gtk_table_attach_defaults(GTK_TABLE(maintable), coreframe, co, co+1, 0, 1); // left, right, top, bottom


	    // create coretable (n columns per core, except the first table, which has just one column)
	    GtkWidget *coretable;
	    if (co == 0) {
		coretable = gtk_table_new(2, 1, FALSE); // rows, columns, homogenuous
	    }
	    else {
		//cerr << "cpu/score " << cpus_per_core << endl;
		if (vertical) {
		    // second column for temperature (which is not in cpu frame)
		    coretable = gtk_table_new(cpus_per_core, 2, FALSE); // rows, columns, homogenuous
		} else {
		    // second row for temperature (which is not in cpu frame)
		    coretable = gtk_table_new(2, cpus_per_core, FALSE); // rows, columns, homogenuous
		}
	    }
	    
	    // put temperature frame in coretable
	    GtkWidget *labelframe = gtk_frame_new(NULL);
	    //gint w, h;
	    //gtk_widget_get_size_request (frame, &w, &h);
	    //cerr << w << " " << h << endl;
	    gtk_widget_set_size_request (labelframe, 80, -1); // we don't modify the frame's height

	    if (!compact) gtk_frame_set_label(GTK_FRAME(labelframe), "temperature");
	    gtk_frame_set_shadow_type(GTK_FRAME(labelframe), GTK_SHADOW_ETCHED_IN);
	    gtk_table_attach_defaults(GTK_TABLE(coretable), labelframe, 0, 2, 1, 2); // left, right, top, bottom

	    GtkWidget *label = gtk_button_new();
	    cbdata.labels["temperature"].push_back(label);
	    gtk_container_add(GTK_CONTAINER(labelframe), label); 


	    // stock pointer to coretables for later usage
	    coretables.push_back(coretable);
	    gtk_table_set_row_spacings(GTK_TABLE(coretable), 5);
	    gtk_table_set_col_spacings(GTK_TABLE(coretable), 5);
	    // add coretable to coreframe
	    gtk_container_add(GTK_CONTAINER(coreframe), coretable);
	}

	// create cpu frames
	for (unsigned int i = 0; i < cpus; ++i) {
	    if (i == 0) sprintf(tmp, "cpu: total");
	    else sprintf(tmp, "cpu: %d", i-1);
	    // frame with gpu id
	    GtkWidget *cpuframe = gtk_frame_new(tmp);
	    gtk_frame_set_shadow_type(GTK_FRAME(cpuframe), GTK_SHADOW_ETCHED_IN);

	    // put gpu frame to table
	    GtkWidget *coretable;
	    int startcol = 0;
	    if (i == 0) coretable = coretables[0];
	    else {
		//cerr << "aaaa " << i << " " << (1+((i-1)/cpus_per_core)) << endl;
		//coretable = coretables[1+((i-1)/cpus_per_core)];

		coretable = coretables[1+((i-1) % (ci.getCores()-1))];
		startcol = (i-1) / (ci.getCores()-1);
		//cerr << "aaaa " << i << " " << (1+((i-1) % (ci.getCores()-1))) << " " << startcol << endl;
	    }
	    //if (vertical)
	    //	gtk_table_attach_defaults(GTK_TABLE(coretable), cpuframe, 0, 1, startcol, startcol+1); // left, right, top, bottom
	    // else
		gtk_table_attach_defaults(GTK_TABLE(coretable), cpuframe, startcol, startcol+1, 0, 1); // left, right, top, bottom

	    // an inner table within the cpuframe (the table keeps the frames+labels for "memory" etc
	    GtkWidget *innertable;
	    // -1 beacause temperature is displayed in coreframe/table
	    if (vertical)
		innertable = gtk_table_new(1, cbdata.infos.size()-1, FALSE); // rows, columns, homogenuous
	    else
		innertable = gtk_table_new(cbdata.infos.size()-1, 1, FALSE); // rows, columns, homogenuous
	    gtk_container_add(GTK_CONTAINER(cpuframe), innertable);    

	    // we create inner frames with buttons for the information to be displayed
	    for (unsigned x = 0; x < cbdata.infos.size(); ++x) {
		if (cbdata.infos[x][0] == 't') continue;
		// frame with label like "memory"
		GtkWidget *labelframe = gtk_frame_new(NULL); //cbdata.infos[x]);
		//gint w, h;
		//gtk_widget_get_size_request (frame, &w, &h);
		//cerr << w << " " << h << endl;
		gtk_widget_set_size_request (labelframe, 80, -1); // we don't modify the frame's height

		if (!compact) gtk_frame_set_label(GTK_FRAME(labelframe), cbdata.infos[x]);

		gtk_frame_set_shadow_type(GTK_FRAME(labelframe), GTK_SHADOW_ETCHED_IN);

		//gtk_frame_set_label_align (GTK_FRAME(labelframe), 0.0, 0.5);

		//if (cbdata.infos[x][0] == 't' && 
		//    (i == 0)) {// || i >= ci.getCores())) {
		    // we do not need temperature for the first column (cpu total)
		//} else {
		//if (cbdata.infos[x][0] == 't') {
		    //} else {
		    if (vertical)
			gtk_table_attach_defaults(GTK_TABLE(innertable), labelframe, x, x+1, 0, 1); // left, right, top, bottom
		    else
			gtk_table_attach_defaults(GTK_TABLE(innertable), labelframe, 0, 1, x, x+1); // left, right, top, bottom
		    //}
		//}
		// the button with the information (replaces a label which does not have background color)
		GtkWidget *label = gtk_button_new();
		//PangoFontDescription *p = pango_font_description_from_string("Serif 10");
		//gtk_widget_modify_font(label, p);
		cbdata.labels[cbdata.infos[x]].push_back(label);
		gtk_container_add(GTK_CONTAINER(labelframe), label); 
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
