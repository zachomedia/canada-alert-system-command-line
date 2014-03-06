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

void str_uppercase(char *str)
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

int main(void)
{
   configure_log();

   // Load current alerts
   Alerts *alerts = load_alerts_from_http_json_file("http://alerts.zacharyseguin.ca/alerts.json");

   // Setup ncurses
   initscr();
   start_color();
   init_pair(1, COLOR_BLACK, COLOR_RED);

   int wincols, winrows;
   getmaxyx(stdscr, winrows, wincols);

   WINDOW *win = newwin(winrows, wincols, 0, 0);
   scrollok(win, true);

   keypad(win, true);
   refresh();

   char tm[17];
   int x = 0;
   while (x < alerts->count)
   {
      wclear(win);

      Alert *alert = &alerts->alerts[x];

      wprintw(win, "*******************************************************\n");
      wprintw(win, "*                 CANADA ALERT SYSTEM                 *\n");
      wprintw(win, "*******************************************************\n\n\n");

      wattron(win, COLOR_PAIR(1));

      char *headline = alert->headline;
      str_uppercase(headline);

      wprintw(win, "%s\n", headline);
      wattroff(win, COLOR_PAIR(1));

      wprintw(win, "Issued by ");
      wattron(win, A_BOLD);
      wprintw(win, "%s", alert->issuer);
      wattroff(win, A_BOLD);

      wprintw(win, " on ");
      wattron(win, A_BOLD);
      strftime(tm, sizeof(tm) - 1, "%Y-%m-%d %H:%M", &alert->effective);
      wprintw(win, "%s", tm);
      wattroff(win, A_BOLD);

      wprintw(win, ". Effective until ");
      wattron(win, A_BOLD);
      strftime(tm, sizeof(tm) - 1, "%Y-%m-%d %H:%M", &alert->expires);
      wprintw(win, "%s", tm);
      wattroff(win, A_BOLD);
      wprintw(win, ".\n\n");

      wprintw(win, "%s\n\n", alert->description);

      wprintw(win, "(%d of %d)\n", x + 1, alerts->count);

      wrefresh(win);
      int ch = wgetch(win);

      if (ch == KEY_LEFT || ch == KEY_UP || ch == KEY_BACKSPACE || ch == KEY_DC)
      {
         if (x > 0) --x;
      }// End of if
      else if (ch == KEY_RIGHT || ch == KEY_DOWN || ch == '\n')
      {
         if (x < alerts->count - 1) ++x;
      }// End of else if
      else if (ch == 'q' || ch == 'x')
      {
         break;
      }// End of else if
   }// End of while

   clrtoeol();
   refresh();
   wrefresh(win);
   endwin();

   free_alerts(alerts);
   close_log();
}// End of main method
