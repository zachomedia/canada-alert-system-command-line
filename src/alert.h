/*
   The MIT License (MIT)

   Copyright (c) 2014 Zachary Seguin

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifndef _ALERT
#define _ALERT

#include <time.h>

struct AlertArea;
typedef struct AlertArea AlertArea;

struct Alert {
   char *headline;
   char *description;
   char *instruction;
   char *issuer;

   struct tm effective;
   struct tm expires;

   int area_count;
   AlertArea **areas;
};
typedef struct Alert Alert;

struct AlertArea {
   char *name;

   int geocode_count;
   int *geocodes;
};

/*
   free_alert(alert) Frees the memory allocted for the alert.
      PRE:  Valid alert pointer
      POST: Memory allocated for the alert and its values are freed.
*/
void free_alert(Alert *alert);

/*
   free_alert_area(alert) Frees the memory allocted for the alert area.
      PRE:  Valid alert area pointer
      POST: Memory allocated for the alert area and its values are freed.
*/
void free_alert_area(AlertArea *area);

#endif
