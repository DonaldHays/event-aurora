CC = sdcc/bin/sdcc
ASMC = sdcc/bin/sdasgb
LIBTOOL = sdcc/bin/sdar

CFLAGS = -c -mgbz80 -I "gblib/include"
ASMFLAGS = -plosgff
LINKFLAGS = -mgbz80 --no-std-crt0 --data-loc 0xc0a0 -L gblib/lib -Wl-b_CODE_1=0x014000

GBLIB_SRCS = \
	gblib/src/gb.c \
	gblib/src/joypad.c

HOME_SRCS = \
	game/src/main.c \
	game/src/module.c

BANK1_GFX = \
	game/data/gfx/font.png

BANK1_SRCS = $(patsubst game/data/gfx/%.png,game/src/data_gfx_%.c,$(BANK1_GFX))
GAME_SRCS = $(HOME_SRCS) $(BANK1_SRCS)

GBLIB_OBJS = $(patsubst gblib/src%,gblib/obj%,$(patsubst %.c,%.rel,$(GBLIB_SRCS)))

GAME_OBJS = $(patsubst game/src%,game/obj%,$(patsubst %.c,%.rel,$(GAME_SRCS)))

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

game: game/obj/dependencies game/bin/EventAurora.gb

game/obj/dependencies: $(GAME_SRCS)
	$(ENSURE_DIRECTORY)
	@rm -f game/obj/dependencies
	@for srcFile in $(GAME_SRCS) ; do \
		{ printf "game/obj/"; $(CC) -MM $$srcFile; } >> game/obj/dependencies ; \
	done

-include game/obj/dependencies

game/src/data_gfx_%.c: game/data/gfx/%.png
	node img2gb -n $(notdir $(basename $<)) $< $@

game/bin/EventAurora.gb: game/obj/game.ihx
	$(ENSURE_DIRECTORY)
	node ihx2gb --name EVENTAURORA --licensee DH --cartridge 3 --rom 1 --ram 1 game/obj/game.ihx game/bin/EventAurora.gb

game/obj/game.ihx: $(GAME_OBJS)
	$(ENSURE_DIRECTORY)
	$(CC) $(LINKFLAGS) gblib/lib/crt0.rel gblib/lib/gb.lib $^ -o $@

game/obj/%.rel: game/src/%.c
	$(ENSURE_DIRECTORY)
	$(CC) $(CFLAGS) $< -o $@
