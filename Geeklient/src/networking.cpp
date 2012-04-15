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
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

int Networking::connect() { // pitäskö tähän kuitenkin laittaa ip-osote parametriksi?
    int res; // result value
    struct sockaddr_in name;
    
    this->socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (this->socket < 0) {
        return -1;
    }
    name.sin_family = AF_INET;
    name.sin_port = htons(this->port);
    name.sin_addr.s_addr = htons(this->host);
    
    res = ::connect(this->socket,(struct sockaddr *) &name,sizeof(name));
    if (res < 0) {
        return -2;
    }
    // WIP
}

int Networking::sendMessage(char* message, int length=1024) {
    int res;
    res = send(socket, message, length, 0);
    return res;
}

int Networking::sendMessage(Message message) { // WIP
    int res;
    res = sendMessage(message,length);
    return res;
}

char* Networking::receiveMessage(char* message, int length=1024) {
    int res;
    res = recv(this->socket, message, length, 0);
    if (res < 0) {
        return 0;
    }
    return message;
}

int Networking::listen() {
    char* message;
    message = receiveMessage(message);
    if (message == 0) {
        return 0;
    }
    // paloittele ja sitten kutsu ui:n funktioita
}

void Networking::quit() {
    // tee poistumiskomennot
    // vapauta muistia
}
