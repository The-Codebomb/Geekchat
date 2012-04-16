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

#define MSG_LENGTH 1024
#define MSG_SYS_BASE :::SYSTEM:::
#define MSG_SYS_CONNECT MSG_SYS_BASE1
#define MSG_SYS_DISCONNECT MSG_SYS_BASE2
#define MSG_SYS_CHKUSRNAME MSG_SYS_BASE3
#define MSG_SYS_USRLIST MSG_SYS_BASE4 /* TBD */

#include "networking.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
using namespace std;

Networking::Networking(void *ui_pointer) {
    port = 13377;
    strcpy(colour,"000000");
    ui = ui_pointer;
}

Networking::~Networking() { // FIXME
    //delete[] socket;
    delete[] host;
    //delete[] port;
    //delete[] ui;
    delete[] name;
    delete[] colour;
    //delete[] users;
}

int Networking::connect(char* host,char* uname) {
    strcpy(this->host,host);
    strcpy(this->name,uname);
    delete[] host, uname;
    
    int res; // result value
    struct sockaddr_in name;
    
    this->socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (this->socket < 0) {
        return -1;
    }
    name.sin_family = AF_INET;
    name.sin_port = htons(this->port);
    name.sin_addr.s_addr = htons(inet_addr(this->host));
    
    res = ::connect(this->socket,(struct sockaddr *) &name,sizeof(name));
    if (res < 0) {
        return -2;
    }
    char msg[sizeof("MSG_SYS_CHKUSRNAME")+12] = "MSG_SYS_CHKUSRNAME[";
    strcat(msg,this->name);
    strcat(msg,"]");
    sendMessage(msg,strlen(msg));
}

int Networking::sendMessage(Message message) {
    int res;
    string msg = "";
    msg.append(this->name); // vai pitäskö käyttää message.sender ?
    msg.append(":");
    msg.append(this->colour); // vai pitäskö käyttää message.colour ?
    msg.append(":");
    msg.append(message.receiver);
    msg.append(":");
    msg.append(message.message);
    res = sendMessage(msg.c_str(),msg.size());
    return res;
}

int Networking::sendMessage(const char* message, int length=1024) {
    int res;
    res = send(socket, message, length, 0);
    return res;
}

char* Networking::receiveMessage(char* message, int length=MSG_LENGTH) {
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
    Message msg;
    char * parts;
    parts = strtok(message,":");
    // tarkistukset tähän
    strcpy(msg.sender,parts);
    strtok(NULL,":");
    strcpy(msg.colour,parts);
    strtok(NULL,":");
    strcpy(msg.receiver,parts);
    strtok(NULL,":");
    strcpy(msg.message,parts);
    delete[] message, parts;
    //this->ui.newMessage(msg); // FIXME
}

void Networking::quit() {
    // tee poistumiskomennot
}
