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
// Selecting ui
#ifdef UI_GENERIC
#include "ui/ui_generic.hpp"
#endif

using namespace std;

/*
 * Doesn't print anything to user nor ask any user input
 * Handles only initialization of ui and Networking and 
 * reading configuration file
 */

int main(int argc, char** argv) {
    Ui ui = Ui();
    Networking net = Networking(&ui);
    ui.begin(&net);
}
