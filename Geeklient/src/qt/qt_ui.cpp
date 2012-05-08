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
  * Wraps Qt based ui written by Topias
  */

#include <string>
#include <QtGui/QApplication>

#include "../networking.hpp"
#include "chatwindow.h"
#include "../ui.hpp"
using namespace std;

#ifndef QT_UI
#define QT_UI

Ui::Ui();
Ui::~Ui();

void Ui::addNewMessage(struct Message message) {
    ChatWindow::appendMessage(message->sender, message->message, 
        message->colour);
}
void Ui::addNewUser(char[10] user) {
    ChatWindow::newChatter(string(user));
}
void Ui::removeUser(char[10] user) {
    ChatWindow::chatterLeft(string(user));
}
void Ui::showErrorMessage(char[3] type,std::string message);

void Ui::begin(class Networking* net_obj)
{
    this->net = net_obj
    QApplication a(argc, argv);
    ChatWindow w;
    w.show();
}

#endif // QT_UI
