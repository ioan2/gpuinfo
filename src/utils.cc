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

#include "utils.h"

#include <math.h>
#include <time.h>
#include <sys/time.h>


void timestamp(ostream &out) {
  char tstr[50];
  struct timeval tv;
  gettimeofday(&tv, NULL);  // get time now
  //time_t t = time(0);   // get time now
  unsigned int millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
  if (millisec >= 1000) { // Allow for rounding up to nearest second
    millisec -= 1000;
    tv.tv_sec++;
  }

  struct tm * now = localtime(&tv.tv_sec);

  char tzone[10];
  //strftime(tstr, 100, "%F %T %z", now);
  strftime(tzone, 10, "%z", now);

  sprintf(tstr, "%d-%02d-%02d %2d:%02d:%02d,%03d %s ", 
	  now->tm_year,
	  now->tm_mon,
	  now->tm_mday,

	  now->tm_hour,
	  now->tm_min,
	  now->tm_sec,
	  millisec,

	  tzone);
  out << tstr;
}


/** create color gradient from green via yellow to red
    http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
*/
void getHeatMapColor(float value, GdkColor &gcolor) {
    //const int NUM_COLORS = 4;
    //static float color[NUM_COLORS][3] = { {0,0x7000,0xffff}, {0,0xffff,0}, {0xffff,0xffff,0}, {0xffff,0,0} };
    const int NUM_COLORS = 3;
    static float color[NUM_COLORS][3] = { {0,0xffff,0}, {0xffff,0xffff,0}, {0xffff,0,0} };
    // A static array of 3 colors:  (green,  yellow,  red) using {r,g,b} for each.
 
    int idx1;        // |-- Our desired color will be between these two indexes in "color".
    int idx2;        // |
    float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.
 
    if(value <= 0)      {  idx1 = idx2 = 0;            }    // accounts for an input <=0
    else if(value >= 1)  {  idx1 = idx2 = NUM_COLORS-1; }    // accounts for an input >=0
    else {
	value = value * (NUM_COLORS-1);        // Will multiply value by 3.
	idx1  = floor(value);                  // Our desired color will be after this index.
	idx2  = idx1+1;                        // ... and before this index (inclusive).
	fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
    }
 
    gcolor.red   = (color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0];
    gcolor.green = (color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1];
    gcolor.blue  = (color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2];
}


