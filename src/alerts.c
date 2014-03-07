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

#include "alerts.h"

#include "log.h"

#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <stdbool.h>

/*
   parse_time(str, time) Parses the time string and places the corresponding
                           struct tm in tm.
      PRE:  Valid str, time pointers.
      POST: time is updated with the time specified in str.
*/
static void parse_time(const char *str, struct tm *tm)
{
   zlog_debug(alog, "Entering");

   if (!str || !tm) return;

   // 2014-02-26T15:41:00-05:00
   strptime(str, "%Y-%m-%dT%H:%M:%S", tm);

   zlog_debug(alog, "Exiting");
}// End of format_time

/*
   keep_alert(js_alert) Returns true if the alert should be kept.
      PRE:  true
      POST: true if the alert should be kept, false otherwise
*/
static bool keep_alert(json_value *js_alert)
{
   zlog_debug(alog, "Entering");

   if (!js_alert) return false;

   json_value *js_status = json_object_value(js_alert, "Status");
   if (!js_status || strcmp(js_status->u.string.ptr, "Actual") != 0) return false;

   zlog_debug(alog, "Exiting");
   return true;
}// End of keep_alert method

/*
   keep_alert_info(js_info) Returns true if the alert info should be kept.
      PRE:  true
      POST: true if the alert info should be kept, false otherwise
*/
static bool keep_alert_info(json_value *js_info)
{
   zlog_debug(alog, "Entering");

   if (!js_info) return false;

   json_value *js_language = json_object_value(js_info, "Language");
   if (!js_language || strcmp(js_language->u.string.ptr, "fr-CA") == 0) return false;

   zlog_debug(alog, "Exiting");
   return true;
}// End of keep_alert method

// IMPLEMENTATION: See header for details
Alerts * load_alerts_from_json(json_value *json)
{
   zlog_debug(alog, "Entering");

   if (!json || json->type != json_array)
   {
      zlog_warn(alog, "Invalid JSON array provided");
      return NULL;
   }// End of if

   // Declare and initalize variables
   Alerts *alerts = NULL;
   int count = 0;
   int index = 0;

   // Get the number of alerts (Information objects)
   for (int i = 0; i < json->u.array.length; ++i)
   {
      json_value *js_alert = json->u.array.values[i];
      if (!keep_alert(js_alert)) continue;

      json_value *js_information = json_object_value(js_alert, "Information");
      if (!js_information) continue;

      for (int ii = 0; ii < js_information->u.array.length; ++ii)
      {
         json_value *js_info = js_information->u.array.values[ii];
         if (!keep_alert_info(js_info)) continue;

         ++count;
      }// End of for (ii)
   }// End of for (i)

   alerts = malloc(sizeof(Alerts));

   if (!alerts)
   {
      zlog_warn(alog, "Failed to allocate memory for alerts object");
      return NULL;
   }// End of if

   alerts->alerts = malloc(sizeof(Alert *) * count);

   if (!alerts->alerts)
   {
      zlog_warn(alog, "Failed to allocate memory for alerts array");
      free_alerts(alerts);
      return NULL;
   }// End of if

   alerts->count = count;

   // Get the alerts
   for (int i = 0; i < json->u.array.length && index < count; ++i)
   {
      json_value *js_alert = json->u.array.values[i];
      if (!keep_alert(js_alert)) continue;

      json_value *js_information = json_object_value(js_alert, "Information");
      if (!js_information) continue;

      for (int ii = 0; ii < js_information->u.array.length; ++ii)
      {
         json_value *js_info = js_information->u.array.values[ii];
         if (!keep_alert_info(js_info)) continue;

         Alert *alert = malloc(sizeof(Alert));
         alerts->alerts[index] = alert;
         ++index;

         if (!alert)
         {
            zlog_warn(alog, "Failed to allocate memory for alert");
            free_alerts(alerts);
            return NULL;
         }// End of if

         alert->headline = json_string_or_default(js_info, "Headline", "");
         alert->description = json_string_or_default(js_info, "Description", "");
         alert->issuer = json_string_or_default(js_info, "SenderName", "");

         char *effective = json_string_or_default(js_info, "Effective", "");
         char *expires = json_string_or_default(js_info, "Expires", "");

         parse_time(effective, &alert->effective);
         parse_time(expires, &alert->expires);

         free(effective);
         free(expires);
      }// End of for (ii)
   }// End of for (i)

   zlog_debug(alog, "Exiting");
   return alerts;
}// End of load_alerts_from_json method

// IMPLEMENTATION: See header for details
Alerts * load_alerts_from_json_file(FILE *file)
{
   zlog_debug(alog, "Entering");

   if (!file)
   {
      zlog_warn(alog, "NULL file pointer provided");
      return NULL;
   }// End of if

   // Declare and initalize variables
   int file_length = 0;
   char *contents = NULL;
   Alerts * alerts = NULL;

   json_value *json = NULL;

   // Read contents of the file
   zlog_debug(alog, "Reading JSON file");

   fseek(file, 0, SEEK_END);
   file_length = ftell(file);
   contents = calloc(file_length + 1, sizeof(char));
   zlog_debug(alog, "File size: %d bytes", file_length);

   if (!contents)
   {
      zlog_warn(alog, "Failed to allocate memory for file contents");
      return NULL;
   }// End of if

   rewind(file);
   fgets(contents, file_length, file);

   // Parse the JSON
   zlog_info(alog, "Parsing JSON");
   json = json_parse(contents, file_length);

   if (!json)
   {
      zlog_warn(alog, "JSON Parse Error");
      free(contents);

      return NULL;
   }// End of if

   // Cleanup
   free(contents);

   // Get the alerts from the json object
   zlog_debug(alog, "Loading alerts from parsed JSON");
   alerts = load_alerts_from_json(json);

   // Cleanup
   json_value_free(json);

   zlog_debug(alog, "Exiting");
   return alerts;
}// End of load_alerts_from_json_file method

/*
   curl_write_response(ptr, size, nmemb, stream) Writes the response to stream.
      PRE:  Valid pointers
      POST: Response written to the stream.

   // Adpated from: http://stackoverflow.com/questions/1636333/download-file-using-libcurl-in-c-c
*/
static size_t curl_write_response(void *ptr, size_t size, size_t nmemb, FILE *stream) {
   return fwrite(ptr, size, nmemb, stream);
}

// IMPLEMENTATION: See header for details
Alerts * load_alerts_from_http_json_file(const char *url)
{
   zlog_debug(alog, "Entering");

   if (!url)
   {
      zlog_warn(alog, "NULL url provided");
      return NULL;
   }// End of if

   // Declare and initalize variables
   Alerts *alerts = NULL;

   FILE *ftmp = tmpfile();
   CURL *curl = curl_easy_init();
   CURLcode res;

   if (!ftmp)
   {
      zlog_warn(alog, "Failed to create temp file");
      if (curl) curl_easy_cleanup(curl);
      return NULL;
   }// End of if

   if (!curl)
   {
      zlog_warn(alog, "Failed to create curl object");
      fclose(ftmp);
      return NULL;
   }// End of if

   // Configure curl
   zlog_debug(alog, "Configuring curl");
   curl_easy_setopt(curl, CURLOPT_URL, url);
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_response);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, ftmp);

   zlog_info(alog, "Performing HTTP request");
   res = curl_easy_perform(curl);
   curl_easy_cleanup(curl);

   if (res == CURLE_OK)
   {
      alerts = load_alerts_from_json_file(ftmp);
   }// End of if
   else
   {
      zlog_warn(alog, "HTTP request failed");
   }// End of if

   fclose(ftmp);

   zlog_debug(alog, "Exiting");
   return alerts;
}// End of load_alerts_from_http_json_file method

// IMPLEMENTATION: See header for details
void free_alerts(Alerts *alerts)
{
   if (!alerts) return;

   zlog_debug(alog, "Entering");

   for (int x = 0; x < alerts->count; ++x)
   {
      free_alert(alerts->alerts[x]);
   }// End of for

   free(alerts->alerts);
   free(alerts);

   zlog_debug(alog, "Exiting");
}// End of free_alerts method
