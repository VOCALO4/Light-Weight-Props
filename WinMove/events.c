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
#include <stdio.h>
#include <windows.h>
#include "gui.h"

int set_hook(void);
void remove_hook(void);
extern int hook_enabled;
extern int res_middle;
extern void (*hook_set_buttons)(int, int, int);

static void act_item_handler(void)
{
	if(hook_enabled) {
		remove_hook();
		set_tray_icon(0);
	} else {
		set_hook();
		set_tray_icon(1);
	}
}

static void quit(void)
{
	remove_hook();
	destroy_gui();
	PostQuitMessage(0);
}

long CALLBACK win_proc(HWND win, unsigned int msg, unsigned int wparam, long lparam) {
	switch(msg) {
	case WM_CLOSE:
		quit();
		return 0;

	case WM_COMMAND:
		if(LOWORD(wparam) == MITEM_ACTIVE) {
			act_item_handler();
		} else if(LOWORD(wparam) == MITEM_RRES) {
			res_middle = 0;
			set_menu_checkbox(MITEM_RRES, 1);
			set_menu_checkbox(MITEM_MRES, 0);

			if(hook_set_buttons) {
				hook_set_buttons(VK_LMENU, 1, 3);
			}
		} else if(LOWORD(wparam) == MITEM_MRES) {
			res_middle = 1;
			set_menu_checkbox(MITEM_RRES, 0);
			set_menu_checkbox(MITEM_MRES, 1);

			if(hook_set_buttons) {
				hook_set_buttons(VK_LMENU, 1, 2);
			}
		} else if(LOWORD(wparam) == MITEM_QUIT) {
			quit();
		}
		return 0;

	case WM_USER:
		if(lparam == WM_LBUTTONDBLCLK) {
			act_item_handler();
		} else if(lparam == WM_RBUTTONUP) {
			show_menu();
		}
		return 0;

	default:
		break;
	}

	return DefWindowProc(win, msg, wparam, lparam);
}
