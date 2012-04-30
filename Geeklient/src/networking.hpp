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

#ifdef UI_GENERIC
#include "ui/ui_generic.hpp"
#endif

#include <string>

#define MSG_LENGTH 1024

/* 
 * Message-struct
 * Used in communication between ui and Networking-object
 * Messages to be sent must have message and receiver filled
 * Messages that are received are completely filled
 */
struct Message {
    std::string message;
    std::string sender;
    std::string receiver;
    char colour[7];
};

/*
 * Networking-object class 
 * Provides all needed networking apis for Geekchat clients
 */
class Networking {
    private:
        int socket;
        std::string host;
        int port;
        Ui *ui;
    public:
        std::string name;
        char colour[7];
        std::string users; // FIXME
    
    /*
     * Constructor
     * @param pointer to ui-object
     */
    Networking(Ui*);
    
    /*
     * Destructor
     */
    ~Networking();
    
    /*
     * Connects to server
     * Called by UI when trying to connect
     * @param string host to connect
     * @param string user name to use
     * @return < 0 in error
     */
    int connect(std::string,std::string);
    
    /*
     * Sends message to server
     * @param Message Message-structure to send
     * @return < 0 in error
     */
    int sendMessage(Message);
    
    /*
     * Listens for new messages
     * Should be run frequently by UI and in case of new message calls
     * ui's newMessage function
     * @return < 0 in error
     */
    int listen();
    
    /*
     * Stops all networking when disconnecting or closing program
     */
    void quit();
    
    private:
        std::string receiveMessage(int=MSG_LENGTH);
        int sendMessage(const char*);
        int sendMessage(std::string);
};
#endif
