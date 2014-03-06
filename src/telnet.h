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

#ifndef _TELNET
#define _TELNET

#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_BUFFER_SIZE 100

struct telnet_client;
typedef struct telnet_client TelnetClient;

/*
   telnet_create(*server, port) Creates the telnet client.
      PRE:  Valid address string (NULL terminated), port > 0
      POST: TelnetClient object is returned.
*/
TelnetClient* telnet_create(char addr[], int port);

/*
   telnet_connect(*server) Connects to the telnet server.
      PRE:  Valid telnet client pointer.
      POST: True if successfully connected, false otherwise.
*/
bool telnet_connect(TelnetClient *telnetClient);

/*
   telnet_disconnect(telnetClient) Disconnects the active telnet connection.
      PRE:  Valid telnet client pointer.
      POST: Telnet client is disconnected.
*/
void telnet_disconnect(TelnetClient *telnetClient);

/*
   telnet_set_callback(*callback) Sets the callback function when a message is received.
      PRE:  Valid telnet client and function pointer for callback.
      POST: Callback function is set, and will be used when a message is received.
*/
void telnet_set_callback(TelnetClient *telnetClient, void (*callback)(char *));

/*
   telnet_set_buffer_size(size) Sets the size of the incoming message buffer.
      PRE:  Valid telnet client pointer, size > 0.
      POST: Buffer size is stored for incoming messages.
*/
void telnet_set_buffer_size(TelnetClient *telnetClient, const int size);

#endif
