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
 
 /*
  * Generic dummy ui
  * Should be used as reference for real UI objects
  */

#ifndef UI_OBJECT
#define UI_OBJECT

#include "../networking.hpp"
#include <string>

class Ui {
    public:
    
    /*
     * Constructor
     * Creates the userinterface
     */
    Ui();
    
    /*
     * Destructor
     */
    ~Ui();
    
    /*
     * Gives control to UI
     * When this is called, Networking object is ready to use
     * Connects using Networking and attaches Networking::listen to timeout
     * @param pointer to Networking object
     */
    void begin(Networking*);
    
    /*
     * Displays new message
     * @param Message object as defined in ../networking.hpp
     */
    void addNewMessage(Message);
    
    /*
     * Displays a new user in user list and/or displays a notification 
     * that a new user has connected
     * @param username
     */
    void addNewUser(char[10]);
    
    /*
     * Removes user from user list
     * @param username
     */
    void removeUser(char[10]);
    
    /*
     * Displays an error of Networking object, only errors that user should see
     * @param error type
     * @param error message
     */
    void showErrorMessage(char[3],std::string);
};

#endif // UI_OBJECT
