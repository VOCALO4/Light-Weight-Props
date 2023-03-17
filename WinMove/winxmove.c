/*
win-x-move 1.1 - easier window move/resize for ms windows.
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
#include <stdlib.h>
#include <windows.h>
#include "gui.h"

int set_hook(void);
void remove_hook(void);
void cleanup(void);
#define errmsg(x)	MessageBox(0, x, "error", MB_ICONERROR | MB_OK)

static HINSTANCE dll;
static HHOOK hook;
static void *hook_func;
void (*hook_set_buttons)(int, int, int);

int hook_enabled;
int res_middle;

int WINAPI WinMain(HINSTANCE pid, HINSTANCE prev, char *args, int show)
{
	MSG msg;

	if(!(dll = LoadLibrary("wxm_hook.dll"))) {
		errmsg("failed to load hook dll");
		return EXIT_FAILURE;
	}
	if(!(hook_func = (void*)GetProcAddress(dll, "hook_func"))) {
		if(!(hook_func = (void*)GetProcAddress(dll, "hook_func@12"))) {
			errmsg("failed to load hook function");
			FreeLibrary(dll);
			return EXIT_FAILURE;
		}
	}
	hook_set_buttons = GetProcAddress(dll, "hook_set_buttons");

	if(strstr(args, "-m")) {
		res_middle = 1;
		hook_set_buttons(VK_LMENU, 1, 2);
	}


	if(set_hook() == -1) {
		return EXIT_FAILURE;
	}
	atexit(cleanup);

	create_gui();

	while(GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

int set_hook(void)
{
	if(hook_enabled) return 0;

	if(!(hook = SetWindowsHookEx(WH_MOUSE, hook_func, dll, 0))) {
		errmsg("failed to set hook");
		return -1;
	}
	hook_enabled = 1;
	return 0;
}

void remove_hook(void)
{
	if(!hook_enabled) return;

	if(!UnhookWindowsHookEx(hook)) {
		errmsg("failed to remove hook");
		return;
	}
	hook_enabled = 0;
}

void cleanup(void)
{
	if(hook_enabled) remove_hook();
	FreeLibrary(dll);
}
