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

#include "json.h"
#include "alert.h"

#include <stdio.h>

#ifndef _ALERTS
#define _ALERTS

struct Alerts {
   int count;
   Alert *alerts;
};
typedef struct Alerts Alerts;

/*
   load_alerts_from_json(json) Loads alerts from a JSON object.
      PRE:  Valid json pointer, and json value is an array type
      POST: Alerts are extracted from JSON and returned in a Alerts object.
*/
Alerts * load_alerts_from_json(json_value *json);

/*
   load_alerts_from_json_file(file) Loads alerts from a JSON file.
      PRE:  Valid file pointer.
      POST: Alerts are read from the JSON file, and an Alerts object is returned.
*/
Alerts * load_alerts_from_json_file(FILE *file);

/*
   load_alerts_from_http_json_file(url) Loads alerts by performing an HTTP request
                                          for the JSON file at url.
      PRE:  Valid url string (valid pointer and NULL terminated)
      POST: HTTP request made and JSON read, and an Alerts object is returned.

   CURL Code adapted from http://stackoverflow.com/questions/1636333/download-file-using-libcurl-in-c-c
*/
Alerts * load_alerts_from_http_json_file(const char *url);

/*
   free_alerts(alerts) Frees the alerts object.
      PRE:  Valid alerts pointer
      POST: Memore allocated for the alerts object is freed.
*/
void free_alerts(Alerts * alerts);

#endif
