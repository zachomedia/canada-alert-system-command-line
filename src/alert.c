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

#include "alert.h"

#include <stdlib.h>
#include "log.h"

// IMPLEMENTATION: See header for details
void free_alert(Alert *alert)
{
   if (!alert) return;

   zlog_debug(alog, "Entering");

   free(alert->headline);
   free(alert->description);
   free(alert->instruction);
   free(alert->issuer);

   if (alert->areas)
   {
      for (int x = 0; x < alert->area_count; ++x)
      {
         free_alert_area(alert->areas[x]);
      }// End of for
   }// End of if

   free(alert);

   zlog_debug(alog, "Exiting");
}// End of free_alert method

// IMPLEMENTATION: See header for details
void free_alert_area(AlertArea *area)
{
   if (!area) return;

   zlog_debug(alog, "Entering");

   free(area->name);
   free(area->geocodes);

   free(area);

   zlog_debug(alog, "Exiting");
}// End of free_alert method
