// geekchat server.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 13377      //Port where connections should come
#define MAXUSERS 50     //Size of chatters array (max amount of clients connected)
#define BUFFER 1024     //Maximum message size from client

using namespace std;

//Struct for saving info about connected clients
struct chatter {
    string name;
    string ip;
    int socketID;
};

// For initializing functions
int make_socket(uint16_t port);
void errorMsg(string error);
void newMsg(string msg);
void arrayAddSock(int sock, string ipaddr);
void newConnection(int sock, char ipaddr[]);
void newMessageFromClient(char msgBuffer[], int posInArray);
void closeConnection(int posInArray);
void removeChatter(int posInArray);
void sendMsgForward(char message[]);

int mainListeningSocket, tempSock;      //mainListeningSocket = all incoming connections go here.
struct chatter chatterArray[MAXUSERS];      //chatterArray contains info of all connections (ip, name and socket)
struct sockaddr_in clientname;      //clientname used to store incoming connections data
uint32_t size;

// Contains initializing (listening socket etc.) and main loop
int main() {
    mainListeningSocket = make_socket(PORT);

    if (listen (mainListeningSocket, 5) < 0) {
        errorMsg("Error in listening");
    }
    size = sizeof(clientname);
    char msgBuffer[BUFFER];
    int recvDat;

    while(1) {              // main loop

        //check if new incoming connections
        tempSock = accept(mainListeningSocket, (struct sockaddr *) &clientname, &size);

        if (tempSock != -1) {
            newConnection(tempSock, inet_ntoa (clientname.sin_addr));
        }

        //check if new incoming messages
        for(int i = 0; i < MAXUSERS; i++) {
            if (chatterArray[i].ip == "") {
                break;
            }
            recvDat = recv(chatterArray[i].socketID, msgBuffer, BUFFER, 0);

            if (recvDat != -1) {
                if (recvDat == 0) {
                    continue;
                }
                newMessageFromClient(msgBuffer, i);
            }
        }

        usleep(50000);        // 1000000 = 1 second
    }
    return 0;
}

// Handles new message from client (send forward etc.)
void newMessageFromClient(char msgBuffer[], int posInArray) {
    string message = msgBuffer;
    string checkIfSysMsg = message.substr(0,12);

    //Check if system message for program
    if (checkIfSysMsg == ":::SYSTEM:::") {

        checkIfSysMsg = message.substr(12,13);

        //If system2 message, delete chatter from array.
        if (checkIfSysMsg == "2") {
            newMsg("--- Connection closed: " + chatterArray[posInArray].ip);
            closeConnection(posInArray);
            sendMsgForward(msgBuffer);
        }

    } else {
        sendMsgForward(msgBuffer);
    }
    cout << message << " :: " << chatterArray[posInArray].socketID << endl;
    //Viestin tarkistus ja eteenpäin lähetys ym.
}

// Sends given message to all connected sockets
void sendMsgForward(char message[]) {

    for(int i = 0; i < MAXUSERS; i++) {

        if (chatterArray[i].ip == "") {
            break;
        }
        //Check if there's error in sending to certain socket
        if (send(chatterArray[i].socketID, message, BUFFER, 0) < 0) {

            if (errno == EPIPE) {
                cout << "Socket " << chatterArray[i].socketID << ":";
                newMsg("--- Socket to " + chatterArray[i].ip + " crashed");
                closeConnection(i);
            } else {
                newMsg("--- Other error in socket " + chatterArray[i].socketID);
            }
        } else {
            cout << "---Message '" << message << "' sent to " << chatterArray[i].socketID << endl;
        }
    }
}

// Closes connection to certain client and removes chatter
void closeConnection(int posInArray) {
    shutdown(chatterArray[posInArray].socketID, 2);
    removeChatter(posInArray);
}

// Removes chatter element from chatters array
void removeChatter(int posInArray) {
    chatterArray[posInArray].ip = "";
    chatterArray[posInArray].name = "";
    chatterArray[posInArray].socketID = 0;

    //Move elements in array so there is no empty "slots"
    for (int i = posInArray+1; i < MAXUSERS; i++) {
        if (chatterArray[i].ip == "") {
            chatterArray[i-1].ip = "";
            chatterArray[i-1].name = "";
            chatterArray[i-1].socketID = 0;
            break;
        }
        chatterArray[i-1] = chatterArray[i];
    }
}

// Adds new incoming connection to chatters array
void arrayAddSock(int sock, string ipaddr) {
    for(int i = 0; i < MAXUSERS; i++) {
        if (chatterArray[i].ip == "") {
            chatterArray[i].ip = ipaddr;
            chatterArray[i].socketID = sock;
            break;
        }
    }
    return;
}

// Handles new incoming connection
void newConnection(int sock, char ipaddr[]) {
    string tempAddr = ipaddr;

    if (fcntl (sock, F_SETFL, O_NONBLOCK) < 0) {
        errorMsg("Error in chaning flag of incoming connection socket");
    }

    arrayAddSock(sock, ipaddr);
    newMsg("--- New connection from: " + tempAddr);
    return;
}

// Prints out error msg and exits program
void errorMsg(string error) {
    cout << error << endl;
    exit(EXIT_FAILURE);
}

// Prints out message
void newMsg(string msg) {
    cout << msg << endl;
}

// Make listening socket
int make_socket (uint16_t port) {
    int sock;
    struct sockaddr_in name;

    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        errorMsg("error in creating listening socket");
    }

    if (fcntl (sock, F_SETFL, O_NONBLOCK) < 0) {
        errorMsg("Error in flag changing");
    }

    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);

    if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
        errorMsg("error in binding listening socket");
    }
    return sock;
}
