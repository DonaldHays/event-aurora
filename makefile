CC = sdcc/bin/sdcc
ASMC = sdcc/bin/sdasgb
LIBTOOL = sdcc/bin/sdar

CFLAGS = -c -mgbz80 -I "include"

GBLIBSRCS = gblib/src/gb.c
GBLIBOBJS = $(patsubst gblib/src%,gblib/obj%,$(patsubst %.c,%.rel,$(GBLIBSRCS)))

build: gblib/lib/gb.lib

gblib/obj/%.rel: gblib/src/%.c
	$(CC) $(CFLAGS) $< -o $@

gblib/lib/gb.lib: $(GBLIBOBJS)
	$(LIBTOOL) -rc gblib/lib/gb.lib $(GBLIBOBJS)
