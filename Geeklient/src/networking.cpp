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

#include "networking.hpp"
//#include <sys/socket.h>

int Networking::connect() { // pitäskö tähän kuitenkin laittaa ip-osote parametriksi?
    Networking::socket = socket(PF_INET, SOCK_STREAM, 0);
    if (Networking::socket < 0) {
        return -1;
    }
    name.sin_family = AF_INET;
    name.sin_port = htons(Networking::port);
    name.sin_addr.s_addr = htons(Networking::host);
}

int Networking::sendMessage(char* message) {
    
}

char* Networking::receiveMessage() {
    
}

int Networking::listen() {
    
}

void Networking::quit() {
    
}
