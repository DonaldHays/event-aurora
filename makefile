CC = sdcc/bin/sdcc
ASMC = sdcc/bin/sdasgb
LIBTOOL = sdcc/bin/sdar

CFLAGS = -c -mgbz80 -I "gblib/include"
ASMFLAGS = -plosgff
LINKFLAGS = -mgbz80 --no-std-crt0 --data-loc 0xc0a0 -L gblib/lib

GBLIB_SRCS = gblib/src/gb.c
GBLIB_OBJS = $(patsubst gblib/src%,gblib/obj%,$(patsubst %.c,%.rel,$(GBLIB_SRCS)))

HOME_SRCS = game/src/main.c
HOME_OBJS = $(patsubst game/src%,game/obj%,$(patsubst %.c,%.rel,$(HOME_SRCS)))

GAME_OBJS = $(HOME_OBJS)

ENSURE_DIRECTORY = @mkdir -p $(@D)

build: library game

clean: cleanLibrary cleanGame

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

cleanGame:
	rm -rf game/obj
	rm -rf game/bin

game: game/bin/EventAurora.gb

game/bin/EventAurora.gb: game/obj/game.ihx
	$(ENSURE_DIRECTORY)
	node ihx2gb --name EVENTAURORA --licensee DH --cartridge 3 --rom 1 --ram 1 game/obj/game.ihx game/bin/EventAurora.gb

game/obj/game.ihx: $(GAME_OBJS)
	$(ENSURE_DIRECTORY)
	$(CC) $(LINKFLAGS) gblib/lib/crt0.rel gblib/lib/gb.lib $^ -o $@

game/obj/%.rel: game/src/%.c
	$(ENSURE_DIRECTORY)
	$(CC) $(CFLAGS) $< -o $@
