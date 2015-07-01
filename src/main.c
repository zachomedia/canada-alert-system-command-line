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

/* LIBRARIES */
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <stdbool.h>

/* PROJECT */
#include "log.h"
#include "alerts.h"

/* DEFINES */
#define HEADLINE_COLOUR 1

/* INSTANCE VARIABLES */
static Alerts *alerts = NULL;
static int active_alert = 0;

static WINDOW *alert_window = NULL;
static WINDOW *stats_window = NULL;
static WINDOW *input_window = NULL;

static int winrows = 0;
static int wincols = 0;

static void str_uppercase(char *str)
{
   while (*str)
   {
      if (*str >= 'a' && *str <= 'z')
      {
         *str = *str - 'a' + 'A';
      }// End of if

      ++str;
   }// End of while
}// End of str_uppercase

static void process_input(const int ch)
{
   if (!alerts) return;

   zlog_debug(alog, "Entering");

   switch (ch)
   {
      case KEY_LEFT:
      case KEY_UP:
      case KEY_BACKSPACE:
      case KEY_DC:            if (active_alert > 0) --active_alert;
                              break;

      case KEY_RIGHT:
      case KEY_DOWN:
      case '\n':              if (active_alert < alerts->count - 1) ++active_alert;
                              break;

      case KEY_EXIT:
      case 'q':
      case 'Q':
      case 'x':
      case 'X':               active_alert = -1;
                              break;
   }// End of switch

   zlog_debug(alog, "Exiting");
}// End of process_input method

static void configure_alert_window(void)
{
   zlog_debug(alog, "Entering");

   if (!alert_window)
   {
      zlog_info(alog, "Setting up alert window");

      alert_window = newwin(winrows - 1, wincols, 0, 0);
      scrollok(alert_window, true);
   }// End of window

   if (!alerts || alerts->count == 0) return;

   // Declare variables
   zlog_info(alog, "Showing alert #%d", active_alert);
   Alert *alert = alerts->alerts[active_alert];

   // Configure window
   wclear(alert_window);

   if (!alert)
   {
      zlog_warn(alog, "Error getting alert (NULL pointer)");
      wprintw(alert_window, "SORRY! An error occurred.\n");
      wrefresh(alert_window);
      return;
   }// End of if

   zlog_debug(alog, "Declaring and initializing variables for output");
   char headline[strlen(alert->headline) + 1];
   char tm[18];

   zlog_debug(alog, "Upper-casing headline");
   snprintf(headline, sizeof(headline), "%s", alert->headline);
   str_uppercase(headline);

   zlog_debug(alog, "Printing headline");
   wattron(alert_window, COLOR_PAIR(HEADLINE_COLOUR));
   wprintw(alert_window, " *** %s *** \n", headline);
   wattroff(alert_window, COLOR_PAIR(HEADLINE_COLOUR));

   zlog_debug(alog, "Printing issuer");
   wprintw(alert_window, "Issued by ");
   wattron(alert_window, A_BOLD);
   wprintw(alert_window, "%s", alert->issuer);
   wattroff(alert_window, A_BOLD);

   zlog_debug(alog, "Printing effective time");
   wprintw(alert_window, " on ");
   wattron(alert_window, A_BOLD);
   strftime(tm, sizeof(tm) - 1, "%Y-%m-%d %H:%M", &alert->effective);
   wprintw(alert_window, "%s", tm);
   wattroff(alert_window, A_BOLD);

   zlog_debug(alog, "Printing expires time");
   wprintw(alert_window, "\nEffective until ");
   wattron(alert_window, A_BOLD);
   strftime(tm, sizeof(tm) - 1, "%Y-%m-%d %H:%M", &alert->expires);
   wprintw(alert_window, "%s", tm);
   wattroff(alert_window, A_BOLD);
   wprintw(alert_window, ".\n\n");

   zlog_debug(alog, "Printing areas");
   wprintw(alert_window, "For ");
   for (int x = 0; x < alert->area_count; ++x)
   {
      if (x > 0) wprintw(alert_window, ", ");

      wattron(alert_window, A_BOLD);
      wprintw(alert_window, "%s", alert->areas[x]->name);
      wattroff(alert_window, A_BOLD);
   }// End of for
   wprintw(alert_window, ".\n\n\n\n");

   zlog_debug(alog, "Printing description");
   wprintw(alert_window, "%s\n\n", alert->description);

   zlog_debug(alog, "Printing instruction");
   wattron(alert_window, A_BOLD);
   wprintw(alert_window, "%s\n\n", alert->instruction);
   wattroff(alert_window, A_BOLD);

   wrefresh(alert_window);

   zlog_debug(alog, "Exiting");
}// End of configure_alert_window method

static void configure_stats_window(void)
{
   zlog_debug(alog, "Entering");

   if (!stats_window)
   {
      stats_window = newwin(1, wincols, winrows - 1, 0);
   }// End of window

   if (!alerts) return;

   // Declare variables

   // Configure window
   wclear(stats_window);

   wprintw(stats_window, "Canada Alert System");

   if (alerts->count > 0)
   {
      wprintw(stats_window, " | %d of %d", active_alert + 1, alerts->count);
   }// End of if
   else
   {
      wprintw(stats_window, " | No active alerts");
   }// End of else

   wrefresh(stats_window);

   zlog_debug(alog, "Exiting");
}// End of configure_stats_window method

static void configure_input_window(void)
{
   zlog_debug(alog, "Entering");

   if (!input_window)
   {
      input_window = newwin(1, 1, winrows - 1, wincols - 1);
      keypad(input_window, true);
   }// End of window

   // Declare variables
   int ch = 0;

   // Configure window
   wclear(input_window);

   ch = wgetch(input_window);
   process_input(ch);

   wrefresh(input_window);

   zlog_debug(alog, "Exiting");
}// End of configure_input_window method

static void configure_windows(void)
{
   zlog_debug(alog, "Entering");

   getmaxyx(stdscr, winrows, wincols);

   configure_alert_window();
   configure_stats_window();
   configure_input_window();

   zlog_debug(alog, "Exiting");
}// End of configure_windows method

int main(void)
{
   configure_log();

   // Load current alerts
   alerts = load_alerts_from_http_json_file("http://alerts.zacharyseguin.ca/api/alerts.json");

   // return -1;
   // Set up ncurses
   initscr();
   start_color();
   init_pair(HEADLINE_COLOUR, COLOR_BLACK, COLOR_RED);

   refresh();

   while (active_alert >= 0)
   {
      configure_windows();
   }// End of while

   endwin();

   free_alerts(alerts);
   close_log();
}// End of main method
