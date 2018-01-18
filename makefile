CC = sdcc/bin/sdcc
ASMC = sdcc/bin/sdasgb
LIBTOOL = sdcc/bin/sdar

CFLAGS = -c -mgbz80 -I "gblib/include"
ASMFLAGS = -plosgff
LINKFLAGS = -mgbz80 --no-std-crt0 --data-loc 0xc0a0 -L gblib/lib -Wl-b_CODE_1=0x014000 -Wl-b_CODE_2=0x024000 -Wl-b_CODE_3=0x034000 -Wl-b_CODE_4=0x044000

GBLIB_SRCS = \
	gblib/src/gb.c \
	gblib/src/joypad.c

HOME_SRCS = \
	game/src/main.c \
	game/src/module.c \
	game/src/banks.c \
	game/src/audio.c \
	game/src/memory.c \
	game/src/text.c \
	game/src/rand.c \
	game/src/palette.c \
	game/src/sprites.c \
	game/src/metatiles.c

BANK1_SRCS = \
	game/src/bank1/mainMenu.c

BANK1_STRINGS = game/data/strings.json
BANK1_SRCS += game/src/data/strings.c

BANK1_GFX = \
	game/data/gfx/font.png \
	game/data/gfx/titleTiles.png \
	game/data/gfx/heroTiles.png \
	game/data/gfx/particleTiles.png
BANK1_SRCS += $(patsubst game/data/gfx/%.png,game/src/data/gfx_%.c,$(BANK1_GFX))

BANK2_SONGS = \
	game/data/music/testSong.txt \
	game/data/music/titleSong.txt \
	game/data/music/jumpSound.txt \
	game/data/music/jumpLandSound.txt \
	game/data/music/testBoing.txt
BANK2_SRCS = $(patsubst game/data/music/%.txt,game/src/data/music_%.c,$(BANK2_SONGS))

BANK2_MAPS = \
	game/data/maps/sample.json \
	game/data/maps/walljump.json \
	game/data/maps/testright.json \
	game/data/maps/abovesample.json \
	game/data/maps/topleft.json
BANK2_SRCS += $(patsubst game/data/maps/%.json,game/src/data/map_%.c,$(BANK2_MAPS))

BANK2_METATILES = \
	game/data/metatiles/castleMetatiles.png
BANK2_SRCS += $(patsubst game/data/metatiles/%.png,game/src/data/meta_%.c,$(BANK2_METATILES))

BANK3_GFX = \
	game/data/gfx/castleTiles.png
BANK3_SRCS = $(patsubst game/data/gfx/%.png,game/src/data/gfx_%.c,$(BANK3_GFX))

BANK4_SRCS = \
	game/src/bank4/game.c \
	game/src/bank4/hero.c \
	game/src/bank4/metamap.c \
	game/src/bank4/particles.c \
	game/src/bank4/heroBullets.c

GAME_SRCS = $(HOME_SRCS) $(BANK1_SRCS) $(BANK2_SRCS) $(BANK3_SRCS) $(BANK4_SRCS)

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
	rm -rf game/src/data

game: game/obj/dependencies game/bin/EventAurora.gb

game/obj/dependencies: $(GAME_SRCS)
	$(ENSURE_DIRECTORY)
	@rm -f game/obj/dependencies
	@for srcFile in $(GAME_SRCS) ; do \
		{ printf "game/obj/"; $(CC) -MM $$srcFile; } >> game/obj/dependencies ; \
	done

-include game/obj/dependencies

game/src/data/strings.c: game/data/strings.json
	$(ENSURE_DIRECTORY)
	node stringc -b 1 game/data/stringsMap.json $< $@

game/src/data/music_%.c: game/data/music/%.txt
	$(ENSURE_DIRECTORY)
	node musicc $< $@

game/src/data/gfx_%.c: game/data/gfx/%.png game/data/gfx/%.meta.json
	$(ENSURE_DIRECTORY)
	node img2gb -n $(notdir $(basename $<)) $< $@

game/src/data/meta_%.c: game/data/metatiles/%.png
	$(ENSURE_DIRECTORY)
	node metac $< $@

game/src/data/map_%.c: game/data/maps/%.json
	$(ENSURE_DIRECTORY)
	node mapc $< $@

game/bin/EventAurora.gb: game/obj/game.ihx
	$(ENSURE_DIRECTORY)
	node ihx2gb --name EVENTAURORA --licensee DH --cartridge 3 --rom 2 --ram 1 game/obj/game.ihx game/bin/EventAurora.gb

game/obj/game.ihx: $(GAME_OBJS)
	$(ENSURE_DIRECTORY)
	$(CC) $(LINKFLAGS) gblib/lib/crt0.rel gblib/lib/gb.lib $^ -o $@

game/obj/%.rel: game/src/%.c
	$(ENSURE_DIRECTORY)
	$(CC) $(CFLAGS) $< -o $@
