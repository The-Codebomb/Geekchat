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
}

int Networking::connect(string host,string uname) {
    this->host = host;
    this->name = uname;
    
    int res; // result value
    struct sockaddr_in name;
    
    this->socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (this->socket < 0) {
        return -2;
    }
    name.sin_family = AF_INET;
    name.sin_port = htons(this->port);
    name.sin_addr.s_addr = htons(inet_addr(this->host.c_str()));
    
    res = ::connect(this->socket,(struct sockaddr *) &name,sizeof(name));
    if (res < 0) {
        return -3;
    }
    string message = "MSG_SYS_CHKUSRNAME[";
    message.append(this->name);
    message.append("]");
    sendMessage(message);
    string msg = receiveMessage();
    if (msg == "FALSE") {
        return -1;
    }
    
}

int Networking::sendMessage(Message message) {
    int res;
    string msg = this->name;
    msg.append(":");
    msg.append(this->colour);
    msg.append(":");
    msg.append(message.receiver);
    msg.append(":");
    msg.append(message.message);
    res = sendMessage(msg);
    return res;
}

int Networking::sendMessage(const char* message) {
    string msg = message;
    return sendMessage(msg);
}

int Networking::sendMessage(string message) {
    int res;
    res = send(socket, message.c_str(), message.size(), 0);
    return res;
}

string Networking::receiveMessage(int length) {
    int res;
    char* msg = new char[length];
    res = recv(this->socket, msg, length, 0);
    if (res < 0) {
        return 0;
    }
    string message = msg;
    delete[] msg;
    return message;
}

int Networking::listen() {
    string message;
    message = receiveMessage();
    if (message.data() == 0) {
        return 0;
    }
    Message msg;/*
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
    delete[] message, parts;*/
    //this->ui.newMessage(msg); // FIXME
}

void Networking::quit() {
    // tee poistumiskomennot
}
