#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# Copyright (c) 2012, The Codebomb
# Copyright (c) 2011-2012, Tomi Leppänen aka Tomin
# All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following disclaimer
#    in the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name 'The Codebomb' nor the names of the contributors 
#    may be used to endorse or promote products derived from this software 
#    without specific prior written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# This program is replaces the AIChat client on Linux
# but can be used on other OSs
# Protocol is the same that is used on AIChat client
#
# Please read the README-file 
# Developers may want to read developer_information.txt
#
from ConfigParser import SafeConfigParser
from os import path
import socket

msg_base = ':::SYSTEM:::' # First part of messages
msg_name_good = '3' # Message when the client asks if name is ok
msg_connect_chat = '1' # Message when a new client starts to chat
msg_name_users = '4' # Message when new chat users are asked
msg_close = '2' # Message when a client leaves chat
msg_end = '§' # Message ending

class about():
    def __init__(self):
        self.name = "Geeklient"
        self.version = '0.3'
        self.description = "AIChat client written in Python"
        self.website = "http://codebomb.dy.fi/"
        self.websitelabel = "The Codebomb"
        self.authors = ["Tomi Leppänen"]
        self.documentors = ["Tomi Leppänen"]
        self.artists = ["Tomi Leppänen"]
        self.logo = None
        self.copyright = "Copyright © 2012, The Codebomb and all the authors"
        self.license = '''\
Redistribution and use in source and binary forms, with or without \
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, \
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, \
this list of conditions and the following disclaimer in the documentation \
and/or other materials provided with the distribution.
* Neither the name of The Codebomb nor the names of the contributors may be \
used to endorse or promote products derived from this software without \
specific prior written permission.
  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" \
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE \
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE \
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE \
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR \
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF \
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS \
INTERRUPTION) HOWEVER CAUSED AND ON ANYTHEORY OF LIABILITY, WHETHER IN \
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) \
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE \
POSSIBILITY OF SUCH DAMAGE.'''
about = about()

class Networking():
    def __init__(self):
        if config.has_option('net','name'):
            self.name=config.get('net','name')
        else:
            self.name=''
        if config.has_option('net','host'):
            self.host=config.get('net','host')
        else:
            self.host=''
        if config.has_option('net','port'):
            try:
                self.port=int(config.get('net','port'))
            except TypeError:
                self.port=13377
        else:
            self.port=13377
        if config.has_option('net','colour'):
            self.colour=config.get('net','colour')
        else:
            self.colour='000000'
        self.killed=False
        self.connected=False
        self.users=[]
        if config.has_section('net'):
            pass
        else:
            config.add_section('net')
    
    def connect(self):
        message=None
        while True:
            response, host, name = ui.connect_dialog(self.host,self.name,
                message)
            if response == -3:
                self.name=name
                self.host=host
                self.connected = True
                self.socket = socket.socket()
                self.socket.settimeout(1)
                try:
                    self.socket.connect((self.host, self.port))
                    self.send_message(msg_name_good+'['+self.name+']',"sys")
                except socket.error:
                    message = "Couldn't connect: Bad hostname"
                    continue
                except socket.timeout:
                    message = "Couldn't connect: Try again"
                    continue
                if self.recv_message() == "FALSE":
                    message = "Couldn't connect: Bad username"
                    continue
                else:
                    self.send_message(msg_connect_chat+'['+self.name+']',"sys")
                    self.socket.settimeout(0)
                    return True
            elif response == -2 or response == -4:
                self.connected = False
                return False
            else: 
                self.connected = False
                print("fixme: "+str(response)) #debug
                return False
    
    def send_message(self,message,type="message",receiver=None):
        msg = None
        message = self._encode_characters(message)
        if type == "message": # regular message
            if self.connected:
                msg = self.name+':0x'+self.colour+';:All: '+message+msg_end
            ui.add_message(self.name+': '+message,self.colour)
        elif type == "pm": # private message
            msg = self.name+':0x'+self.colour+';:'+receiver+': '+\
                message+msg_end
            ui.add_message('PM to '+receiver+':'+message,self.colour)
        elif type == "sys": # system message
            msg = msg_base+message+msg_end
        elif type == "command": # other server command
            if self.connected:
                msg = self.name+':0x'+self.colour+';:All: '+message+msg_end
            ui.add_message(self.name+': '+message,'0000CD')
        if self.connected and msg != None:
            self.socket.sendall(msg.encode("iso-8859-1","ignore"))
            print("Sent: "+msg) #debugging
        elif not self.connected:
            ui.add_message("You are disconnected!",'FF0000')
    
    def recv_message(self,length=1024):
        try:
            msg = self.socket.recv(length)
            print("Got: "+msg.decode("iso-8859-1")) #debugging
            return msg.decode("iso-8859-1")
        except (socket.error,socket.timeout):
            return None
    
    def listen(self):
        messages = self.recv_message()
        if messages == None:
            if not self.killed:
                return True
            else:
                return False
        messages = messages.split("§")
        for message in messages:
            if msg_base in message[0:len(msg_base)]:
                if msg_base+msg_name_good in message:
                    pass
                elif msg_base+msg_name_users in message:
                    message = (message.split("["))[-1]
                    users = message.split(",")
                    users = users[0:-1]
                    for user in users:
                        self.users.append(user)
                        ui.add_user(user)
                elif msg_base+msg_connect_chat in message:
                    user = message.split("[")[1][:-1]
                    self.users.append(user)
                    ui.add_user(user)
                    ui.add_message(str(user)+" joined the chat!",'0000CD')
                elif msg_base+msg_close in message:
                    user = message.split("[")[1][:-1]
                    ui.remove_user(user)
                    ui.add_message(str(user)+" left the chat!,'0000CD'")
                    if user == self.name: # UNDOCUMENTED
                        ui.message_dialog("Connection dropped","error")
                        ui.quit()
                elif msg_base+msg_end in message:
                    ui.message_dialog("You have been kicked","error")
                    ui.quit()
                else:
                    print("Unknown system message: "+message) #debugging
            else:
                sender,colour,receiver,message = message.split(":",3)
                message = self._decode_characters(message)
                if receiver == "All":
                    ui.add_message(user+":"+message,colour[2:-1])
                else:
                    ui.add_message("PM from "+user+":"+message,colour[2:-1])
            if not self.killed:
                return True
            else:
                return False
    
    def _encode_characters(self,text):
        text = text.replace('&','&amp;')
        text = text.replace('§','&sect;')
        return text
    
    def _decode_characters(self,text):
        text = text.replace('&amp;','&')
        text = text.replace('&sect;','§')
        return text
    
    def is_connected(self):
        return self.connected
    
    def kill(self):
        self.killed=True
    
    def quit(self):
        self.kill()
        if self.connected:
            self.send_message(msg_close+'['+self.name+']',"sys")
            self.socket.close()

def saveConfig():
    config.set('net','host',net.host)
    if net.name != 'You':
        config.set('net','name',net.name)
    config.set('net','port',str(net.port))
    config.set('net','colour',net.colour)
    with open(confPath, 'wb') as configFile:
        config.write(configFile)
        configFile.close()

def main():
    global ui, net, config, confPath
    
    config = SafeConfigParser()
    config.SaveConfig = saveConfig
    if path.isfile('geeklient.cfg'): # portable mode
        confPath = 'geeklient.cfg'
    elif path.expandvars('$XDG_CONFIG_HOME') != '$XDG_CONFIG_HOME':
        confPath = path.expandvars('$XDG_CONFIG_HOME/geeklient.cfg')
    elif path.expandvars('%APPDATA%') != '%APPDATA%':
        confPath = path.expandvars('%APPDATA%/geeklient.cfg')
    else:
        confPath = path.expandvars('$HOME/.config/geeklient.cfg')
    if path.isfile(confPath):
        config.read(confPath)
    
    net = Networking()
    from ui import UI_GTK
    ui = UI_GTK(net,config,about)
    if net.connect():
        ui.add_message("Welcome to chat!")
    else: 
        net.name = "You"
        ui.add_message("You are not connected",'FF0000')
    ui.begin()
    return None

if __name__ == "__main__":
    main()
