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

/*
 * Networking-object class 
 * Provides all needed networking apis for Geekchat clients
 */
class Networking {
    int socket;
    char* host;
    int port = 13377;
    
    public:
        char* name;
        char* colour = "000000";
        char** users;
    
    /*
     * Connects to server
     * @return < 0 in error
     */
    int connect();
    
    /*
     * Sends message to server
     * @param char* message to send
     * @return < 0 in error
     */
    int sendMessage(char*);
    
    /*
     * Receives message
     * @return the message
     */
    char* receiveMessage();
    
    /*
     * Listens for new messages
     * @return < 0 in error
     */
    int listen();
    
    /*
     * Stops all networking 
     */
    void quit();
};
#endif
