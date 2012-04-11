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
import pygtk
pygtk.require('2.0')
import gtk,pango,gobject
import re
from functools import partial

class UI_GTK():
    def __init__(self,net,config,about):
        self.about = about
        self.config = config
        self.net = net
        
        def create_window(self):
            height = 600
            width = 400
            try:
                if self.config.has_option('ui_gtk','height'):
                    height=int(self.config.get('ui_gtk','height'))
                if self.config.has_option('ui_gtk','width'):
                    width=int(self.config.get('ui_gtk','width'))
            except TypeError:
                pass
            self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
            self.window.connect("destroy", self.quit)
            self.window.set_border_width(0)
            self.window.set_size_request(300, 200)
            self.window.set_default_size(height,width)
            self.window.set_title(self.about.name+", version "+
                self.about.version)
            self.V_MAIN = gtk.VBox(False, 0)
            self.window.add(self.V_MAIN)
            self.H_MAIN = gtk.HBox(False, 0)
            self.V_MAIN.pack_end(self.H_MAIN, True, True, 0)
            self.V_RIGHT = gtk.VBox(False, 0)
            self.H_MAIN.pack_end(self.V_RIGHT, True, True, 0)
            self.V_LEFT = gtk.VBox(False, 0)
            self.H_MAIN.pack_start(self.V_LEFT, True, True, 0)
        
        def create_messages(self):
            messageScroll = gtk.ScrolledWindow()
            messageScroll.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
            messageScroll.set_shadow_type(gtk.SHADOW_OUT)
            messageView = gtk.TextView()
            self.messageBuffer = messageView.get_buffer()
            messageView.set_buffer(self.messageBuffer)
            messageView.set_editable(False) 
            messageView.set_cursor_visible(False)
            messageView.set_wrap_mode(gtk.WRAP_WORD)
            messageScroll.add_with_viewport(messageView)
            self.V_LEFT.pack_start(messageScroll, True, True, 0)
            self.messageScrollBar = messageScroll.get_vadjustment()
            self.tagTable = self.messageBuffer.get_tag_table()
        
        def create_userList(self):
            self.userList = gtk.ListStore(gobject.TYPE_STRING)
            self.uList = gtk.TreeView(self.userList)
            uColumn = gtk.TreeViewColumn("Users")
            uColumn.set_sizing(gtk.TREE_VIEW_COLUMN_GROW_ONLY)
            uColumn.set_min_width(100)
            uColumn.set_expand(True)
            self.uList.append_column(uColumn)
            cell = gtk.CellRendererText()
            uColumn.pack_start(cell, True)
            uColumn.add_attribute(cell, "text", 0)
            scroll = gtk.ScrolledWindow()
            scroll.set_policy(gtk.POLICY_NEVER,gtk.POLICY_AUTOMATIC)
            scroll.add_with_viewport(self.uList)
            self.V_RIGHT.pack_start(scroll, True, True, 0)
            def remove_user_highlights(tag):
                tag.set_property("background-set",False)
            def highlight_selected_user(widget):
                selected = self.uList.get_selection()
                if selected != None:
                    model,iterator = selected.get_selected()
                    user = model.get_value(iterator,0)
                    userTag = self.tagTable.lookup(user)
                    if userTag != None:
                        userTag.set_property("background-set",True)
                else:
                    self.tagTable.foreach(remove_user_highlights)
            self.uList.connect('cursor-changed',highlight_selected_user)
            uColumn.set_sort_column_id(1)
        
        def create_messageBox(self):
            self.messageBox = gtk.Entry()
            def send_on_enter(event):
                    self._send_message()
            self.messageBox.connect("activate",send_on_enter)
            self.V_LEFT.pack_end(self.messageBox, False, True, 0) 
        
        def create_sendButton(self):
            button = gtk.Button("Send")
            button.connect("clicked",self._send_message)
            self.V_RIGHT.pack_end(button, False, True, 0)
            
        def create_menubar(self):
            menu_xml = '''
<menubar name="menubar">
    <menu action="Chat">
        <menuitem action="Connect" />
        <menuitem action="SendPM" />
        <menuitem action="Save" />
        <menuitem action="Exit" />
    </menu>
    <menu action="Color">
        <menuitem action="colorBlack" />
        <menuitem action="colorBlue" />
        <menuitem action="colorGreen" />
        <menuitem action="colorRed" />
        <menuitem action="colorYellow" />
        <menuitem action="colorCustom" />
    </menu>
    <menu action="About">
        <menuitem action="License" />
        <menuitem action="Aboutme" />
    </menu>
</menubar>'''
            uiManager = gtk.UIManager()
            accelG = uiManager.get_accel_group()
            self.window.add_accel_group(accelG)
            
            actionG = gtk.ActionGroup("menu")
            actionG.add_actions([
            ('Chat', None, "_Chat"),
            ('Connect', None, "_Connect...", None, "Connect to Server", 
                self._reconnect),
            ('SendPM', None, "Send _PM...", None, "Sends private message", 
                self._sendpm_dialog),
            ('Save', None, "_Save settings", None, "Saves settings", 
                self._saveConfig),
            ('Exit', None, "_Exit", None, "Quits the self.about", self.quit),
            ('Color', None, "C_olor"),
            ('colorBlack', None, "B_lack", None, None, 
                partial(self._set_colour,'000000')),
            ('colorBlue', None, "_Blue", None, None, 
                partial(self._set_colour,'0000FF')),
            ('colorGreen', None, "_Green", None, None, 
                partial(self._set_colour,'008000')),
            ('colorRed', None, "_Red", None, None, 
                partial(self._set_colour,'FF0000')),
            ('colorYellow', None, "_Yellow", None, None, 
                partial(self._set_colour,'FFFF00')),
            ('colorCustom', None, "_Custom...", None, None, 
                partial(self._set_colour,'custom')),
            ('About', None, "_About"),
            ('License', None, "_License", None, None, self._print_license),
            ('Aboutme', None, "_About "+self.about.name+"...", None, None, 
                self._about_dialog)
            ])
            
            uiManager.insert_action_group(actionG,0)
            uiManager.add_ui_from_string(menu_xml)
            menubar = uiManager.get_widget('/menubar')
            self.V_MAIN.pack_start(menubar, False, True, 0)
            menubar.show()
        
        create_window(self)
        create_messageBox(self)
        create_messages(self)
        create_sendButton(self)
        create_userList(self)
        create_menubar(self)
        self.net_run=None
        self.window.show_all()
        
        self.user_re = re.compile('^[\w]{3,10}:')
        self.link_re = re.compile('[a-z]*://[^ ]*')
        self.linkTag = gtk.TextTag("link")
        self.linkTag.set_property("underline", pango.UNDERLINE_SINGLE)
        self.linkTag.set_property("underline-set",True)
        self.linkTag.connect("event",self._open_link)
        self.tagTable.add(self.linkTag)
        
        iterator = self.messageBuffer.get_end_iter()
        self.messageBuffer.insert(iterator,"Copyright © 2012, The Codebomb")
        self.add_message("To get more infomation about licensing of this "+
            "self.about, type '/license'")
    
    def add_message(self,message,colour=None):
        iterator = self.messageBuffer.get_end_iter()
        if self.user_re.match(message): # if message begins with username
            user,noneed = message.split(':',1)
        else: 
            user = None
        if colour == None: # if message has colour information
            self.messageBuffer.insert(iterator,"\n"+message)
        else: 
            colour = self._add_colour(colour)
            if user == None:
                self.messageBuffer.insert_with_tags(iterator,"\n"+message,
                    colour)
            else:
                userTag = self.tagTable.lookup(user)
                if userTag == None: # create tag, if it doesn't exist
                    userTag = gtk.TextTag(user)
                    userTag.set_property("background","yellow")
                    userTag.set_property("background-set",False)
                    self.tagTable.add(userTag)
                self.messageBuffer.insert_with_tags(iterator,"\n"+message,
                        colour,userTag)
        self.messageScrollBar.set_value(self.messageScrollBar.upper)
        self.messageScrollBar.need_scroll = True
        links = self.link_re.finditer(message) # mark links
        if links:
            line = iterator.get_line()
            for link in links:
                begin = self.messageBuffer.get_iter_at_line_offset(line,
                    link.start())
                end = self.messageBuffer.get_iter_at_line_offset(line,
                    link.end())
                self.messageBuffer.apply_tag(self.linkTag,begin,end)
                self.linkTag.set_priority(self.tagTable.get_size()-1)
    
    def add_user(self,name):
        self.userList.append([name])
    
    def begin(self):
        if self.net.is_connected():
            self.net_run = gtk.timeout_add(10,net.listen)
        self.messageBox.grab_focus()
        gtk.main()
    
    def connect_dialog(self,host='',name='',message=None):
        dialog = gtk.Dialog("Choose username",None,0,
                (gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT,
                gtk.STOCK_OK, gtk.RESPONSE_ACCEPT))
        if message != None:
            messageLabel = gtk.Label(message)
            dialog.vbox.pack_start(messageLabel)
            messageLabel.show()
        hostLabel = gtk.Label("Server hostname:")
        dialog.vbox.pack_start(hostLabel)
        hostLabel.show()
        hostBox = gtk.Entry()
        if host != '':
            hostBox.set_text(host)
        def move_to_next_widget(event,widget):
            widget.get_toplevel().child_focus(gtk.DIR_TAB_FORWARD)
        hostBox.connect("activate",move_to_next_widget,hostBox)
        dialog.vbox.pack_start(hostBox)
        hostBox.show()
        nameLabel = gtk.Label("Username:")
        dialog.vbox.pack_start(nameLabel)
        nameLabel.show()
        nameBox = gtk.Entry(10)
        if name != '':
            nameBox.set_text(name)
        def close_on_enter(event,response):
            dialog.response(response)
        nameBox.connect("activate",close_on_enter,gtk.RESPONSE_ACCEPT)
        dialog.vbox.pack_start(nameBox)
        nameBox.show()
        dialog.set_transient_for(self.window)
        response = dialog.run()
        ip = hostBox.get_text()
        name = nameBox.get_text()
        dialog.destroy()
        return response, ip, name
    
    def message_dialog(self,message,type="info"):
        if type == "info":
            type = gtk.MESSAGE_INFO
        elif type == "warning":
            type = gtk.MESSAGE_WARNING
        elif type == "error":
            type == gtk.MESSAGE_ERROR
        messageDialog = gtk.MessageDialog(None,0,type,gtk.BUTTONS_OK,message)
        messageDialog.set_transient_for(self.window)
        response = messageDialog.run()
        messageDialog.destroy()
        return response
    
    def remove_user(self,name): # if name == 0, then remove all users
        iterator = self.userList.get_iter_root()
        while iterator:
            if self.userList.get_value(iterator,0) == name:
                self.userList.remove(iterator)
                break
            elif name == 0:
                self.userList.remove(iterator)
                self.userList.iter_next(iterator)
            else:
                self.userList.iter_next(iterator)
    
    def _about_dialog(self,widget=None):
        dialog = gtk.AboutDialog()
        dialog.set_name(self.about.name)
        dialog.set_version(self.about.version)
        dialog.set_comments(self.about.description)
        dialog.set_copyright(self.about.copyright)
        dialog.set_website(self.about.website)
        dialog.set_website_label(self.about.websitelabel)
        dialog.set_authors(self.about.authors)
        dialog.set_documenters(self.about.documentors)
        dialog.set_artists(self.about.artists)
        dialog.set_logo(self.about.logo)
        dialog.set_license(self.about.license)
        dialog.set_wrap_license(True)
        dialog.set_transient_for(self.window)
        response = dialog.run()
        dialog.destroy()
    
    def _add_colour(self,colour):
        colourTag = gtk.TextTag()
        colourTag.set_property("foreground", gtk.gdk.color_parse('#'+colour))
        self.tagTable.add(colourTag)
        return colourTag
    
    def _open_link(self,linkTag,widget,event,iterator):
        if event.type == gtk.gdk.BUTTON_PRESS:
            begin = iterator.copy()
            begin.backward_to_tag_toggle(linkTag)
            end = iterator
            end.forward_to_tag_toggle(linkTag)
            gtk.show_uri(gtk.gdk.screen_get_default(), 
                self.messageBuffer.get_text(begin,end,False), 0)
    
    def _print_license(self,widget=None):
        self.add_message(self.about.license)
    
    def _reconnect(self,widget=None,data=None):
        if self.net_run != None:
            gtk.timeout_remove(self.net_run)
            self.net_run=None
        if self.net.is_connected():
            self.net.quit()
            _remove_user(0)
            self.add_message("Disconnected")
            self.net = Networking()
        if self.net.connect():
            gtk.timeout_add(10,net.listen)
            self.add_message("Connected")
    
    def _saveConfig(self,widget=None):
        if self.config.has_section('ui_gtk'):
            pass
        else:
            self.config.add_section('ui_gtk')
        height,width = self.window.get_size()
        self.config.set('ui_gtk','height',str(height))
        self.config.set('ui_gtk','width',str(width))
        self.config.SaveConfig()
    
    def _send_message(self,widget=None):
        message = self.messageBox.get_text()
        if len(message) > 0:
            if message[-1] == "\n":
                message = message[0:-1]
            if message[0] == "/":
                if "/pm" in message:
                    try:
                        cmd,name,message = message.split(" ",2)
                        self.net.send_message(message,"pm",name)
                    except ValueError:
                        self.add_message(
                            'Usage: /pm <user> <message>','FF0000')
                elif "/license" in message:
                    self._print_license()
                elif "/colour" in message or "/color" in message:
                    try:
                        cmd,colour = message.split(' ',1)
                        self._set_colour(colour)
                    except ValueError:
                        self.add_message('Current color is '+net.colour)
                elif "/connect" in message:
                    self._reconnect()
                elif "/save" in message:
                    self._saveConfig()
                elif "/exit" in message or "/quit" in message:
                    self.quit()
                elif "/help" in message:
                    self.add_message('''\
Client's commands:
/color <hexvalue>       set or get color, gets if <hexvalue> is omitted
/connect        connects to new server
/exit       quits client
/help       prints this help
/license        prints license
/pm <user> <message>        sends private message
/save       saves settings
/quit       quits client
Server may have more supported commands''')
                else:
                    self.net.send_message(message,"command")
            else:
                self.net.send_message(message)
            self.messageBox.set_text('')
            self.messageBox.grab_focus()
    
    def _sendpm_dialog(self,widget=None):
        dialog = gtk.Dialog("Send private message",None,0,
                (gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT,
                gtk.STOCK_OK, gtk.RESPONSE_ACCEPT))
        
        userLabel = gtk.Label("Select user:")
        dialog.vbox.pack_start(userLabel)
        userLabel.show()
        userSelection = gtk.ComboBoxEntry(self.userList,0)
        cell = gtk.CellRendererCombo()
        userSelection.pack_start(cell, True)
        dialog.vbox.pack_start(userSelection)
        userSelection.show()
        
        messageLabel = gtk.Label("Message:")
        dialog.vbox.pack_start(messageLabel)
        messageLabel.show()
        messageEntry = gtk.Entry()
        dialog.vbox.pack_start(messageEntry)
        def close_on_enter(event,response):
            dialog.response(response)
        messageEntry.connect("activate",close_on_enter,gtk.RESPONSE_ACCEPT)
        messageEntry.show()
        
        response = dialog.run()
        n = userSelection.get_active()
        if n >= 0:
            iterator = self.userList.iter_nth_child(None,n)
            if iterator != None:
                user = self.userList.get_value(iterator,0)
            else:
                user = None
        else:
            user = None
        message = messageEntry.get_text()
        dialog.destroy()
        if response == gtk.RESPONSE_ACCEPT and user != None:
            self.net.send_message(message,"pm",user)
    
    def _set_colour(self,colour,widget=None):
        if re.match('[0-9A-Fa-f]{6}',colour):
            self.net.colour=colour
            self.add_message('Your color is now: '+colour,colour)
        elif colour == "custom":
            dialog = gtk.Dialog("Choose color",None,0,
                (gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT,
                gtk.STOCK_OK, gtk.RESPONSE_ACCEPT))
            colourLabel = gtk.Label("New color value (in hex):")
            dialog.vbox.pack_start(colourLabel)
            colourLabel.show()
            colourBox = gtk.Entry()
            def close_on_enter(event,response):
                dialog.response(response)
            colourBox.connect("activate",close_on_enter,gtk.RESPONSE_ACCEPT)
            dialog.vbox.pack_start(colourBox)
            colourBox.show()
            response = dialog.run()
            colour = colourBox.get_text()
            dialog.destroy()
            if response == gtk.RESPONSE_ACCEPT:
                self._set_colour(colour)
        else:
            self.add_message('Bad colour value, please use hexadecimal values',
                'FF0000')
    
    def quit(self,widget=None):
        self.net.quit()
        gtk.main_quit()
