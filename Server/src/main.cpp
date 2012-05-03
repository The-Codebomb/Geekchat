/*
    Geekchat server software.
    Copyright (C) 2012 Codebomb
    Copyright (c) 2012 Anssi "Miffyli" Kanervisto

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// geekchat server.

#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 13377      //Port where connections should come
#define MAXUSERS 50     //Size of chatters array (max amount of clients connected)
#define BUFFER 1024     //Maximum message size from client
#define SLEEPTIME 50000 //Sleeptime between loops (1000000 = 1 second)

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
void handleSystemMsg(string message, string checkIfSysMsg, int posInArray,
                      int msgLn, char msgBuffer[]);
void newMessageFromClient(char msgBuffer[], int posInArray);
void closeConnection(int posInArray);
void removeChatter(int posInArray);
void sendMsgForward(char msgBuffer[], int posInArray, int dontSendBack=-1);
void sendUserList(int posInArray);
int stringToCharArray(string stringText, char charText[], uint len);



//mainListeningSocket = all incoming connections go here.
int mainListeningSocket, tempSock;
//chatterArray contains info of all connections (ip, name and socket)
struct chatter chatterArray[MAXUSERS];
//clientname used to store incoming connections data
struct sockaddr_in clientname;
uint32_t size;

/*
* Contains main loop and inizializing
*/
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
        tempSock = accept(mainListeningSocket, (struct sockaddr *) &clientname,
                           &size);

        if (tempSock != -1) {
            newConnection(tempSock, inet_ntoa (clientname.sin_addr));
        }

        //check if new incoming messages
        for(int i = 0; i < MAXUSERS; i++) {
            if (chatterArray[i].ip == "") {
                break;
            }
            //Check/Read if there is any new messages in buffers
            recvDat = recv(chatterArray[i].socketID, msgBuffer, BUFFER, 0);

            if (recvDat != -1) {
                if (recvDat == 0) {
                    continue;
                }
                //If new message recieved, continue to handle it in function
                newMessageFromClient(msgBuffer, i);
            }
        }

        usleep(SLEEPTIME);        // 1000000 = 1 second
    }
    return 0;
}

/*
* Handles new message from client (send forward etc.)
*@param msgBuffer[] contains new recieved message
*@param posInArray position of sender in chatter array
*/
void newMessageFromClient(char msgBuffer[], int posInArray) {

    string message = msgBuffer;
    int msgLn = message.length();
    string checkIfSysMsg = "";
    cout << message << " :: " << chatterArray[posInArray].socketID << endl;

    //Check if new message has new "illegal" name
    /*
    string parsedName = message.substr(0, message.find(":"));
    if ((parsedName != string::npos) | (parsedName != "")) {
        if (parsedName != chatterArray[posInArray].name) {

            newMsg("---Illegal namechange!");
            newMsg("---Saved name: " + chatterArray[posInArray].name);
            newMsg("---Illegal name: " + message.substr(0, message.find(":")));

            closeConnection(posInArray);
            //Send disconnection message about crashed client
            string newMsgString = ":::SYSTEM:::2[" +
                                            chatterArray[posInArray].name + "]";
            int strLength = newMsgString.length();
            char newMessage[strLength];

            stringToCharArray(newMsgString, newMessage, strLength);

            sendMsgForward(newMessage, posInArray);
            return;
        }
    }
    */


    if (msgLn > 12) {
        checkIfSysMsg = message.substr(0,12);
    }

    //Check if system message for program
    if (checkIfSysMsg == ":::SYSTEM:::") {
        handleSystemMsg(message, checkIfSysMsg, posInArray, msgLn, msgBuffer);
    } else {
        sendMsgForward(msgBuffer, posInArray);
    }
    //Viestin tarkistus ja eteenpäin lähetys ym.
}

/*
* Handles given system message
*@param message contains recieved message
*@param posInArray position of sender in chatter array
*/
void handleSystemMsg(string message, string checkIfSysMsg, int posInArray,
                        int msgLn, char msgBuffer[]) {

    //Get system message number
    if (msgLn < 12) {
        newMsg("---Invalid system message number: " + message);
        return;
    }

    checkIfSysMsg = message.at(12);
    newMsg("--- System message: " + checkIfSysMsg);

    //If system2 message, delete chatter from array.
    if (checkIfSysMsg == "2") {
        newMsg("--- Connection closed: " + chatterArray[posInArray].ip);
        closeConnection(posInArray);
        sendMsgForward(msgBuffer, posInArray);

    //Check if client is asking name check
    } else if (checkIfSysMsg == "3") {

        //Check for forbidden chars with regexp?

        bool nameIsGood = true;

        if (message.length() < 16) {
            newMsg("---Invalid msg length in system3: " + message);
            return;
        }
        string sentName = message.substr(14, message.length() - 15);
        newMsg("...New name: " + sentName);

        //Check name length
        if ((sentName.length() < 3) | (sentName.length() > 10)) {
            newMsg("...Invalid amount of chars");
            nameIsGood = false;
        }

        //check invalid chars
        if ((sentName.find(":") != string::npos) |
                    (sentName.find("/") != string::npos)) {
            newMsg("...Invalid chars in name");
            nameIsGood = false;
        }

        for (int i=0; i < MAXUSERS; i++) {
            if (chatterArray[i].ip == "") {
                break;
            }
            if (sentName == chatterArray[i].name) {
                nameIsGood = false;
                break;
            }
        }

        if (!nameIsGood) {
            /*If name was already in use
                OR
                Has invalid amount of chars, or invalid ones */
            send(chatterArray[posInArray].socketID, "FALSE", BUFFER, 0);
        } else {
            //If name is free to use
            send(chatterArray[posInArray].socketID, "TRUE", BUFFER, 0);
            chatterArray[posInArray].name = sentName;
            sendUserList(posInArray);

            //String -> Char array
            string newMsgToClients = ":::SYSTEM:::1[" + sentName +"]";
            int msgLen = newMsgToClients.length();
            char newMsgBuffer[msgLen];

            stringToCharArray(newMsgToClients, newMsgBuffer, msgLen);

            sendMsgForward(newMsgBuffer, posInArray, posInArray);
        }
    }
}

/*
* Sends list of current users to given connection
*@param posInArray position of sender in chatter array
*/
void sendUserList(int posInArray) {
    string userList = ":::SYSTEM:::4[";

    for (int i = 0; i < MAXUSERS; i++) {
        if (chatterArray[i].ip == "") {
            break;
        }
        userList.append(chatterArray[i].name + ",");
    }

    userList.append("]");
    char msgBuffer[userList.length()];

    if (stringToCharArray(userList, msgBuffer, BUFFER) == -1) {
        errorMsg("Userlist bigger than the array size");
    }

    send(chatterArray[posInArray].socketID, msgBuffer, BUFFER, 0);
}

/*
* turns string into char array
*@param stringText string that will be converted to char array
*@param charText char array where new char array will be stored
*@return success if -1, string was longer than array size.
*/
int stringToCharArray(string stringText, char charText[], uint len) {
    if (len < stringText.length()) {
        return -1;
    }
    for(uint i = 0; i < stringText.length(); i++) {
        charText[i] = stringText.at(i);
    }
    return 1;
}

/*
* Sends given message to all connected sockets
*@param msgBuffer[] contains message which will be sent forward
*@param posInArray position of sender in chatter array
*@param dontSendBack position in chatter array where you don't
                     want to send message (-1 = send to everybody)
*/
void sendMsgForward(char msgBuffer[], int posInArray, int dontSendBack) {


    for(int i = 0; i < MAXUSERS; i++) {

        if (chatterArray[i].ip == "") {
            break;
        }
        if (i == dontSendBack) {
            continue;
        }
        //Check if there's error in sending to certain socket
        if (send(chatterArray[i].socketID, msgBuffer, BUFFER, 0) < 0) {

            // If error occured...
            if (errno == EPIPE) {
                cout << "Socket " << chatterArray[i].socketID << ":";
                newMsg("--- Socket to " + chatterArray[i].ip + " crashed");
                closeConnection(i);
                //Send disconnection message about crashed client
                string newMsgString = ":::SYSTEM:::2[" +
                                        chatterArray[posInArray].name + "]";
                int strLength = newMsgString.length();
                char newMessage[strLength];

                stringToCharArray(newMsgString, newMessage, strLength);

                sendMsgForward(newMessage, posInArray);

            } else {
                newMsg("--- Other error in socket "
                       + chatterArray[i].socketID);
            }
        } else {
            cout << "---Message '" << msgBuffer << "' sent to "
                    << chatterArray[i].socketID << endl;
        }
    }
}

/*
* Closes connection to certain client and removes chatter
*@param PosInArray position of sender in chat array
*/
void closeConnection(int posInArray) {
    //Close connection
    shutdown(chatterArray[posInArray].socketID, 2);
    //Remove chatter from connections array
    removeChatter(posInArray);
}

/*
* Removes chatter element from chatters array
*@param posInArray position of sender in chat array
                   This client will be removed
*/
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

/*
* Adds new incoming connection to chatters array
*@param sock handle to socket
*@param ipaddr address of connected client in presentation form (string/char[])
*/
void arrayAddSock(int sock, string ipaddr) {
    //Finds where is first "empty slot"
    for(int i = 0; i < MAXUSERS; i++) {
        if (chatterArray[i].ip == "") {
            chatterArray[i].ip = ipaddr;
            chatterArray[i].socketID = sock;
            return;
        }
    }
    newMsg("!!!User array is full!!!");
    return;
}

/*
* Handles new incoming connection
*@param sock handle to socket
*@param ipaddr address of connected client in presentation form (string/char[])
*/
void newConnection(int sock, char ipaddr[]) {
    string tempAddr = ipaddr;

    //Set NONBLOCKING mode for socket
    if (fcntl (sock, F_SETFL, O_NONBLOCK) < 0) {
        errorMsg("Error in chaning flag of incoming connection socket");
    }

    arrayAddSock(sock, ipaddr);
    newMsg("--- New connection from: " + tempAddr);
    return;
}

/*
* Prints out error msg and exits program
*@param error error message to be printed
*/
void errorMsg(string error) {
    cout << "ERROR: " << error << endl;
    exit(EXIT_FAILURE);
}

/*
* Prints out message
*@param msg message to be printed
*/
void newMsg(string msg) {
    cout << msg << endl;
}

/*
* Creates listening socket
*@param port port used when making the socket
*/
int make_socket (uint16_t port) {
    int sock;
    struct sockaddr_in name;

    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        errorMsg("error when creating listening socket");
    }

    if (fcntl (sock, F_SETFL, O_NONBLOCK) < 0) {
        errorMsg("Error in flag changing");
    }

    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);

    //Bind socket data to socket
    if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
        errorMsg("error when binding listening socket");
    }
    return sock;
}
