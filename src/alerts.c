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

#include <stdio.h>
#include <string.h>
#include <time.h>

#define JSON_FILE_CONTENTS_BUFFER_LENGTH 0xFFFFFF

static char * format_time(const char *str)
{
   zlog_debug(alog, "Entering");

   if (!str) return NULL;

   // ####-##-## ##:##
   char *str_time = calloc(17, sizeof(char));

   if (!str_time) return NULL;

   struct tm tm;
   // 2014-02-26T15:41:00-05:00
   char *res = strptime(str, "%Y-%m-%dT%H:%M:%S", &tm);

   if (!res)
   {
      zlog_warn(alog, "Date parse error");
      return NULL;
   }// End of if
   strftime(str_time, 16, "%F %R", &tm);

   zlog_debug(alog, "Exiting");
   return str_time;
}// End of format_time

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
      json_value *alert = json->u.array.values[i];
      if (!alert) continue;

      json_value *information = json_object_value(alert, "Information");
      if (!information) continue;

      for (int ii = 0; ii < information->u.array.length; ++ii)
      {
         json_value *info = information->u.array.values[ii];
         if (!info) continue;

         json_value *language = json_object_value(info, "Language");
         if (!language || strcmp(language->u.string.ptr, "fr-CA") == 0) continue;

         ++count;
      }// End of for (ii)
   }// End of for (i)

   alerts = malloc(sizeof(Alerts));

   if (!alerts)
   {
      zlog_warn(alog, "Failed to allocate memory for alerts object");
      return NULL;
   }// End of if

   alerts->alerts = malloc(sizeof(Alert) * count);

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
      json_value *alert = json->u.array.values[i];
      if (!alert) continue;

      json_value *information = json_object_value(alert, "Information");
      if (!information) continue;

      for (int ii = 0; ii < information->u.array.length && index < count; ++ii)
      {
         json_value *info = information->u.array.values[ii];
         if (!info) continue;

         json_value *language = json_object_value(info, "Language");
         if (!language || strcmp(language->u.string.ptr, "fr-CA") == 0) continue;

         alerts->alerts[index].headline = json_string_or_default(info, "Headline", "");
         alerts->alerts[index].description = json_string_or_default(info, "Description", "");
         alerts->alerts[index].issuer = json_string_or_default(info, "SenderName", "");

         char *effective = json_string_or_default(info, "Effective", "");
         char *tm_effective = format_time(effective);

         if (tm_effective)
         {
            free(effective);
            effective = NULL;
            alerts->alerts[index].effective = tm_effective;
         }// End of if
         else
         {
            alerts->alerts[index].effective = effective;
         }// End of else

         char *expires = json_string_or_default(info, "Expires", "");
         char *tm_expires = format_time(expires);

         if (tm_expires)
         {
            free(expires);
            expires = NULL;
            alerts->alerts[index].expires = tm_expires;
         }// End of if
         else
         {
            alerts->alerts[index].expires = expires;
         }// End of else

         ++index;
      }// End of for (ii)
   }// End of for (i)

   zlog_debug(alog, "Exiting");
   return alerts;
}// End of load_alerts_from_json method

// IMPLEMENTATION: See header for details
Alerts * load_alerts_from_json_file(const char *file_path)
{
   zlog_debug(alog, "Entering");

   if (!file_path)
   {
      zlog_warn(alog, "NULL file pointer provided");
      return NULL;
   }// End of if

   // Declare and initialize variables
   FILE *file = NULL;

   char *contents = calloc(JSON_FILE_CONTENTS_BUFFER_LENGTH + 1, sizeof(char));
   Alerts * alerts = NULL;

   if (!contents)
   {
      zlog_warn(alog, "Failed to allocate memory for file contents");
      return NULL;
   }// End of if

   json_value *json = NULL;


   // Read the raw contents of the file
   zlog_info(alog, "Opening JSON file \"%s\"", file_path);
   file = fopen(file_path, "r");

   if (!file)
   {
      zlog_warn(alog, "Failed to open file \"%s\"", file_path);
      return NULL;
   }// End of if

   // Read contents of the file
   zlog_debug(alog, "Reading JSON file \"%s\"", file_path);
   fgets(contents, JSON_FILE_CONTENTS_BUFFER_LENGTH, file);

   // Close the file, it's no longer needed
   fclose(file);

   // Parse the JSON
   zlog_info(alog, "Parsing JSON");
   json = json_parse(contents, JSON_FILE_CONTENTS_BUFFER_LENGTH);

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

// IMPLEMENTATION: See header for details
void free_alerts(Alerts *alerts) {
   free(alerts->alerts);
   free(alerts);
}
