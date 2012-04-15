/*
 * Geeklient is Geekchat Client software
 * 
 * Copyright (C) 2012  The Codebomb
 * Copyright (C) 2012  Tomi Leppänen aka Tomin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETWORKING_OBJECT
#define NETWORKING_OBJECT
#include <string.h>

/* 
 * Message-struct
 * Used in communication between ui and Networking-object
 */
struct Message {
    char message[1024]; // dynamic length?
    char sender[10];
    char receiver[10];
}

/*
 * Networking-object class 
 * Provides all needed networking apis for Geekchat clients
 */
class Networking {
    private:
        int socket;
        char host;
        int port;
        void *ui;
    public:
        char name[10];
        char colour[7];
        char users; // ***change to dynamic array of char arrays***
    
    /*
     * Constructor
     * @param pointer to ui-object
     */
    Networking(void *ui_pointer) {
        port = 13377;
        strcpy(colour,"000000");
        ui = ui_pointer;
    }
    
    /*
     * Connects to server
     * Called by UI when trying to connect
     * @return < 0 in error
     */
    int connect();
    
    /*
     * Sends message to server
     * @param char[] receiver (can be 'All' or a username)
     * @param char[] message to send
     * @param int length of the message
     * @return < 0 in error
     */
        int sendMessage(Message);
    
    /*
     * Listens for new messages
     * Should be run frequently by UI
     * @return < 0 in error
     */
    int listen();
    
    /*
     * Stops all networking when disconnecting or closing program
     */
    void quit();
    
    private:
        char* receiveMessage(char*,int);
        int sendMessage(char[],int);
};
#endif
