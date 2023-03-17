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
#include "gui.h"
#include "events.h"
#include "resource.h"

static HWND create_window(const char *title, int xsz, int ysz, const char *class_name);

HWND win_main;
HMENU menu, menu_res;
HICON icon_on, icon_off;
static NOTIFYICONDATA nicon;
int trayfied = 1;

void create_gui(void) {
	reg_win_class("winxmove_gui", win_proc);
	win_main = create_window("win-x-move", 0, 0, "winxmove_gui");

	menu_res = CreatePopupMenu();
	AppendMenu(menu_res, MF_STRING, MITEM_RRES, "Right mouse button");
	AppendMenu(menu_res, MF_STRING, MITEM_MRES, "Middle mouse button");

	menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING | MF_CHECKED, MITEM_ACTIVE, "Active");
	AppendMenu(menu, MF_STRING | MF_POPUP, (int)menu_res, "Resize button");
	AppendMenu(menu, MF_SEPARATOR, 0, 0);
	AppendMenu(menu, MF_STRING, MITEM_QUIT, "Quit");

	icon_on = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(ICON_TRAY_ON));
	icon_off = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(ICON_TRAY_OFF));

	memset(&nicon, 0, sizeof nicon);
	nicon.cbSize = sizeof nicon;
	nicon.hWnd = win_main;
	nicon.uID = 1;
	nicon.uCallbackMessage = WM_USER;
	nicon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nicon.hIcon = icon_on;
	strcpy(nicon.szTip, "Win-X-Move");
	Shell_NotifyIcon(NIM_ADD, &nicon);
}

void destroy_gui(void)
{
	DestroyMenu(menu);
	Shell_NotifyIcon(NIM_DELETE, &nicon);
	DestroyWindow(win_main);
}

void set_menu_checkbox(int item, int onoff)
{
	HMENU m = (item == MITEM_RRES || item == MITEM_MRES) ? menu_res : menu;
	CheckMenuItem(m, item, MF_BYCOMMAND | (onoff ? MF_CHECKED : MF_UNCHECKED));
}

void show_menu(void)
{
	extern int hook_enabled, res_middle;

	POINT pt;
	GetCursorPos(&pt);

	set_menu_checkbox(MITEM_ACTIVE, hook_enabled);
	set_menu_checkbox(MITEM_RRES, !res_middle);
	set_menu_checkbox(MITEM_MRES, res_middle);

	SetForegroundWindow(win_main);
	TrackPopupMenuEx(menu, TPM_VERTICAL, pt.x, pt.y, win_main, 0);
}

void set_tray_icon(int onoff)
{
	nicon.hIcon = onoff ? icon_on : icon_off;
	Shell_NotifyIcon(NIM_MODIFY, &nicon);
}

static HWND create_window(const char *title, int xsz, int ysz, const char *class_name) {
	int x = (GetSystemMetrics(SM_CXSCREEN) - xsz) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - ysz) / 2;
	unsigned long style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	return CreateWindow(class_name, title, style, x, y, xsz, ysz, 0, 0, GetModuleHandle(0), 0);
}

void reg_win_class(const char *class_name, msg_handler_t handler) {
	WNDCLASSEX wc;
	HINSTANCE pid = GetModuleHandle(0);

	memset(&wc, 0, sizeof wc);
	wc.cbSize = sizeof wc;
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.hCursor = LoadCursor(pid, MAKEINTRESOURCE(IDC_ARROW));
	wc.hIcon = wc.hIconSm = LoadIcon(pid, MAKEINTRESOURCE(ICON_WXM));
	wc.hInstance = pid;
	wc.lpfnWndProc = handler;
	wc.lpszClassName = class_name;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wc);
}
