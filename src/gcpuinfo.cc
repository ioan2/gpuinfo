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

//#include <math.h>
//#include <time.h>
//#include <sys/time.h>

#include <gtk/gtk.h>

#include "cpuinfo.h"
#include "utils.h"

#define APPNAME "cgpuinfo"


using namespace std;

/** names of pieces information */
const char *infos[] = { "user", "niced", "system", NULL };
const unsigned int infolen = 3;

/** structure to give information to callback */
struct cb_data {
    CpuInfo *ci;
    map<const char *, vector<GtkWidget *> > labels;
};


/** used to update labels */
gboolean time_handler(struct cb_data *cbdata) {
    unsigned int u[MAX_CPU], s[MAX_CPU], n[MAX_CPU], i[MAX_CPU];
    cbdata->ci->getCPUtime(u, s, n, i);
    unsigned int cpus = cbdata->ci->getCpus();

    // for each gpu device
    for(unsigned int i = 0; i < cpus; ++i) {
	char tmp[100];

	// for each piece of information
	for (unsigned x = 0; x < infolen; ++x) {
	    // find label in cbdata
	    GtkWidget *label = cbdata->labels[infos[x]][i];


	    unsigned int val = 0;
	    // get correct values to display
	    switch(infos[x][0]) {
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
	cerr << argv[0] << " options" << endl
	     << "  -d  update delay (default 500ms)" << endl
	     << "  -T  textual output" << endl
	     << "\n with -T" <<endl
	     << "  -t  add time stamp" << endl
	     << "  -l  log mode" << endl
	    
	    ;
    }


    unsigned int delay = 500;
    bool textonly = false;
    bool logmode = false;
    bool timest = false;

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
	    /*    } else if (strcmp(argv[i], "-a") == 0) {
		  all = true;
		  } else if (strcmp(argv[i], "-g") == 0) {
		  graphics = true;
		  } else if (strcmp(argv[i], "-s") == 0) {
		  shortoutput = true; */
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
	gtk_init(&argc, &argv);

	//printf("GTK+ version: %d.%d.%d\n", gtk_major_version, gtk_minor_version, gtk_micro_version);
	//printf("Glib version: %d.%d.%d\n", glib_major_version, glib_minor_version, glib_micro_version);


	// main window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
	gtk_window_set_title(GTK_WINDOW(window), APPNAME);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);

	// create table (a column per gpu device)
	GtkWidget *table = gtk_table_new(1, cpus, TRUE); // rows, columns, homogenuous
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_container_add(GTK_CONTAINER(window), table);

	// initialise cbdata
	struct cb_data cbdata;
	cbdata.ci = &ci;
	
	char tmp[70];
	for (unsigned int i = 0; i < cpus; ++i) {
	    sprintf(tmp, "cpu: %d", i);
	    // frame with gpu id
	    GtkWidget *frame = gtk_frame_new(tmp);
	    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
	    // put gpu frame to table
	    gtk_table_attach_defaults(GTK_TABLE(table), frame, i, i+1, 0, 1); // left, right, top, bottom

	    // an inner table within the frame (the table keeps the frames+labels for "memory" etc
	    GtkWidget *innertable = gtk_table_new(infolen, 1, TRUE); // rows, columns, homogenuous
	    gtk_container_add(GTK_CONTAINER(frame), innertable);    

	    // we create inner frames with buttons for the information to be displayed
	    for (unsigned x = 0; x < infolen; ++x) {
		// frame with label like "memory"
		GtkWidget *iframe = gtk_frame_new(infos[x]);
		gtk_frame_set_shadow_type(GTK_FRAME(iframe), GTK_SHADOW_ETCHED_IN);

		gtk_table_attach_defaults(GTK_TABLE(innertable), iframe, 0, 1, x, x+1); // left, right, top, bottom
		// the button with the information (replaces a label which does not have background color)
		GtkWidget *label = gtk_button_new();
  

		cbdata.labels[infos[x]].push_back(label);
		gtk_container_add(GTK_CONTAINER(iframe), label); 
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