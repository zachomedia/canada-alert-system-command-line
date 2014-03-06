/*
   The MIT License (MIT)

   Copyright (c) 2014 Zachary Seguin

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
   the Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "telnet.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

struct telnet_client {
   int sock;
   bool connected;
   struct sockaddr_in addr;
   pthread_t read_thread;
   int buffer_size;
   void (*callback)(char *);
};

// IMPLEMENTATION: See header for details
TelnetClient* telnet_create(char addr[], int port) {
   assert(addr);
   assert(port > 0);

   TelnetClient *telnetClient = malloc(sizeof(TelnetClient));

   if (!telnetClient) {
      return NULL;
   }

   telnetClient->sock = socket(AF_INET, SOCK_STREAM, 0);

   telnetClient->addr.sin_addr.s_addr = inet_addr(addr);
   telnetClient->addr.sin_family = AF_INET;
   telnetClient->addr.sin_port = htons(port);

   telnetClient->connected = true;
   telnetClient->buffer_size = DEFAULT_BUFFER_SIZE;
   telnetClient->callback = NULL;

   return telnetClient;
}

/*
   read_loop(*argument) Read loop for the telnet client.
      PRE:  Valid telnet client pointer
      POST: Read loop has started.
*/
static void* read_loop(void *argument) {
   assert(argument);

   TelnetClient *telnetClient = (TelnetClient *)argument;

   while (telnetClient->connected) {
      char *message = malloc(sizeof(char) * telnetClient->buffer_size + 1);

      int ret = recv(telnetClient->sock, message, telnetClient->buffer_size, 0);

      if (ret == 0) {
         telnetClient->connected = false;
         break;
      } else if (ret < 0) {
         break;
      } else {
         if (telnetClient->callback) {
            telnetClient->callback(message);
         }

         free(message);
      }
   }

   return NULL;
}

// IMPLEMENTATION: See header for details
bool telnet_connect(TelnetClient *telnetClient) {
   assert(telnetClient);

   if (telnetClient->sock == -1) {
      return false;
   }

   int connect_result = connect(telnetClient->sock, (struct sockaddr *)&telnetClient->addr, sizeof(telnetClient->addr));
   if (connect_result < 0) {
      return false;
   }

   telnetClient->connected = true;

   return !pthread_create(&telnetClient->read_thread, NULL, read_loop, (void *)telnetClient);
}

// IMPLEMENTATION: See header for details
void telnet_disconnect(TelnetClient *telnetClient) {
   telnetClient->connected = false;

   pthread_join(telnetClient->read_thread, NULL);
   close(telnetClient->sock);
}

// IMPLEMENTATION: See header for details
void telnet_set_callback(TelnetClient *telnetClient, void (*callback)(char *)) {
   telnetClient->callback = callback;
}

// IMPLEMENTATION: See header for details
void telnet_set_buffer_size(TelnetClient *telnetClient, const int size) {
   telnetClient->buffer_size = size;
}
