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

enum {
	DIR_NONE,
	DIR_NE,
	DIR_NW,
	DIR_SE,
	DIR_SW
};

static void move_window(void *win, int dx, int dy);
static void resize_window(void *win, int dx, int dy);
static void *get_toplevel(void *win);
static int get_res_dir(int x, int y, int left, int top, int right, int bottom);


#if defined(__GNUC__) || defined(GNUC)
#define SHARED	__attribute__((section(".shr"), shared))
int moving SHARED = 0;
int resizing SHARED = 0;
int res_dir SHARED;
int mod_key SHARED = VK_LMENU, move_bn SHARED = 1, res_bn SHARED = 3;
void *win SHARED;
POINT prev_pt SHARED;
#elif defined(_MSC_VER)
#pragma data_seg(".shared")
int moving, resizing, res_dir;
int mod_key = VK_LMENU, move_bn = 1, res_bn = 3;
POINT prev_pt;
void *win;
#pragma data_seg()
#else
#error "unsupported compiler, may need modifications"
#endif

__declspec(dllexport) void hook_set_buttons(int mod, int move, int res)
{
	mod_key = mod;
	move_bn = move;
	res_bn = res;
}

static int get_down_event(int bn)
{
	static int res[] = {0, WM_LBUTTONDOWN, WM_MBUTTONDOWN, WM_RBUTTONDOWN};
	return res[bn];
}

static int get_up_event(int bn)
{
	static int res[] = {0, WM_LBUTTONUP, WM_MBUTTONUP, WM_RBUTTONUP};
	return res[bn];
}

__declspec(dllexport) long CALLBACK hook_func(int code, WPARAM wpar, LPARAM lpar)
{
	POINT pt;
	int modkey_state, msg = wpar;
	int handled = 0;
	
	if(code < 0) {
		return CallNextHookEx(0, code, wpar, lpar);
	}

	GetCursorPos(&pt);
	modkey_state = GetAsyncKeyState(mod_key);
	
	switch(msg) {
	case WM_MOUSEMOVE:
		if(moving) {
			move_window(win, pt.x - prev_pt.x, pt.y - prev_pt.y);
			prev_pt = pt;
			handled = 1;
		} else if(resizing) {
			resize_window(win, pt.x - prev_pt.x, pt.y - prev_pt.y);
			prev_pt = pt;
			handled = 1;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if(modkey_state && msg == get_down_event(move_bn)) {
			moving = 1;
			prev_pt = pt;
			handled = 1;
			win = get_toplevel(WindowFromPoint(pt));

		} else if(modkey_state && msg == get_down_event(res_bn)) {
			RECT rect;
			resizing = 1;
			prev_pt = pt;
			handled = 1;
			win = get_toplevel(WindowFromPoint(pt));

			GetWindowRect(win, &rect);
			res_dir = get_res_dir(pt.x, pt.y, rect.left, rect.top, rect.right, rect.bottom);
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		if(msg == get_up_event(move_bn)) {
			if(moving) {
				handled = 1;
			}
			moving = 0;
		} else if(msg == get_up_event(res_bn)) {
			if(resizing) {
				handled = 1;
			}
			resizing = 0;
		}
		break;
	}

	/* if we didn't handle the event, call the next hook in the list, which
	 * will eventually result in the event being passed to the application that
	 * was bound to receive it in the first place...
	 */
	if(!handled) {
		return CallNextHookEx(0, code, wpar, lpar);
	}
	return 1;	/* ... otherwise return non-zero to block the event from going further */
}

static void move_window(void *win, int dx, int dy)
{
	RECT rect;
	int x, y;

	GetWindowRect(win, &rect);
	x = rect.left + dx;
	y = rect.top + dy;

	SetWindowPos(win, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

static void resize_window(void *win, int dx, int dy)
{
	RECT rect;
	int x, y, xsz, ysz;

	GetWindowRect(win, &rect);
	
	switch(res_dir) {
	case DIR_SW:
		dx = -dx;
		x = rect.left - dx;
		y = rect.top;
		break;

	case DIR_NW:
		dx = -dx;
		dy = -dy;
		x = rect.left - dx;
		y = rect.top - dy;
		break;

	case DIR_NE:
		dy = -dy;
		x = rect.left;
		y = rect.top - dy;
		break;

	case DIR_SE:
	default:
		x = rect.left;
		y = rect.top;
		break;
	}

	xsz = rect.right - rect.left + dx;
	ysz = rect.bottom - rect.top + dy;

	SetWindowPos(win, 0, x, y, xsz, ysz, SWP_NOZORDER);
}

static void *get_toplevel(void *win)
{
	if(!(GetWindowLong(win, GWL_STYLE) & WS_CHILD)) {
		return win;
	}
	return get_toplevel(GetParent(win));
}

static int get_res_dir(int x, int y, int left, int top, int right, int bottom)
{
	int horiz, vert;
	int dirs[2][2] = {{DIR_NW, DIR_SW}, {DIR_NE, DIR_SE}};
	
	horiz = (abs(x - left) < abs(x - right)) ? 0 : 1;
	vert = (abs(y - top) < abs(y - bottom)) ? 0 : 1;
	return dirs[horiz][vert];
}
