hookobj = hook.o
hooklib = wxm_hook.dll

obj = winxmove.o gui.o events.o
bin = winxmove.exe

CC = gcc
CFLAGS = -mno-cygwin -mwindows -std=gnu89 -pedantic -Wall -Os

all: $(hooklib) $(bin)

$(hooklib): $(hookobj)
	$(CC) -shared -o $@ $<

$(bin): $(obj) icon.o
	$(CC) $(CFLAGS) -o $@ $(obj) icon.o -lgdi32

icon.o: icon.ico
	windres -i wxm.rc -o icon.o

.PHONY: clean
clean:
	rm -f $(obj) $(hooklib) $(bin) icon.o
