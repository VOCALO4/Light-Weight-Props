/*
win-x-move - easier window move/resize for ms windows.
Copyright (C) 2007 John Tsiombikas <nuclear@siggraph.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef _GUI_H_
#define _GUI_H_

#include <windows.h>

enum {
	MITEM_ACTIVE = 100,
	MITEM_RRES,
	MITEM_MRES,
	MITEM_QUIT
};


typedef long (CALLBACK *msg_handler_t)(HWND, unsigned int, unsigned int, long);

void create_gui(void);
void destroy_gui(void);

void set_menu_checkbox(int item, int onoff);
void show_menu(void);

void set_tray_icon(int onoff);
void reg_win_class(const char *class_name, msg_handler_t handler);

#endif	/* _GUI_H_ */
