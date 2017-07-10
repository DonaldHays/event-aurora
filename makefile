CC = sdcc/bin/sdcc
ASMC = sdcc/bin/sdasgb
LIBTOOL = sdcc/bin/sdar

CFLAGS = -c -mgbz80 -I "gblib/include"
ASMFLAGS = -plosgff

GBLIB_SRCS = gblib/src/gb.c
GBLIB_OBJS = $(patsubst gblib/src%,gblib/obj%,$(patsubst %.c,%.rel,$(GBLIB_SRCS)))

ENSURE_DIRECTORY = @mkdir -p $(@D)

build: library

clean: cleanLibrary

cleanLibrary:
	rm -rf gblib/obj
	rm -rf gblib/lib

library: gblib/obj/dependencies gblib/lib/crt0.rel gblib/lib/gb.lib

gblib/obj/dependencies: $(GBLIB_SRCS)
	$(ENSURE_DIRECTORY)
	@rm -f gblib/obj/dependencies
	@for srcFile in $(GBLIB_SRCS) ; do \
		{ printf "gblib/obj/"; $(CC) -MM $$srcFile; } >> gblib/obj/dependencies ; \
	done

-include gblib/obj/dependencies

gblib/obj/%.rel: gblib/src/%.c
	$(ENSURE_DIRECTORY)
	$(CC) $(CFLAGS) $< -o $@

gblib/lib/crt0.rel: gblib/src/crt0.s
	$(ENSURE_DIRECTORY)
	$(ASMC) $(ASMFLAGS) $@ $<

gblib/lib/gb.lib: $(GBLIB_OBJS)
	$(ENSURE_DIRECTORY)
	$(LIBTOOL) -rc gblib/lib/gb.lib $(GBLIB_OBJS)
