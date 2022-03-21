.SUFFIXES:
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

GAME_TITLE_JP	    :=  ポケモン
GAME_SUBTITLE1_JP	:=  ネオ
GAME_SUBTITLE2_JP	:=  Homebrew

GAME_TITLE_EN	    :=	Pok_mon
GAME_SUBTITLE1_EN	:=  neo Version
GAME_SUBTITLE2_EN	:=  Homebrew

GAME_TITLE_FR	    :=	Pok_mon
GAME_SUBTITLE1_FR	:=  neo Version
GAME_SUBTITLE2_FR	:=  Homebrew

GAME_TITLE_DE	    :=	Pok_mon
GAME_SUBTITLE1_DE	:=  neo Edition
GAME_SUBTITLE2_DE	:=  Homebrew

GAME_TITLE_IT	    :=	Pok_mon
GAME_SUBTITLE1_IT	:=  neo Version
GAME_SUBTITLE2_IT	:=  Homebrew

GAME_TITLE_ES	    :=	Pok_mon
GAME_SUBTITLE1_ES	:=  neo Version
GAME_SUBTITLE2_ES	:=  Homebrew

GAME_ICON       :=  $(CURDIR)/icon.bmp

export DEV_CODE			:=	01
export GAME_CODE		:=	ADAJ
export GAME_TITLE		:=	POKEMON NEO
export VERSION			:=	10
export VERSION_NAME		:=	Waiting Suicune neo

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)
export NDSTOOL		:=  $(CURDIR)/tools/ndstool/ndstool

#main targets
all: $(TARGET).nds

ifneq ($(strip $(NITRO_FILES)),)
_ADDFILES	:=	-d $(NITRO_FILES)
endif

ifdef FLASHCARD
$(TARGET).nds: FORCE arm7/$(TARGET).elf arm9/$(TARGET).elf
	$(NDSTOOL) -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -d ./FSROOT -b $(GAME_ICON) "$(GAME_TITLE_EN);$(GAME_SUBTITLE1_EN);$(GAME_SUBTITLE2_EN)" $(_ADDFILES)
else
$(TARGET).nds: FORCE arm7/$(TARGET).elf arm9/$(TARGET).elf
	$(NDSTOOL) -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
		-d ./FSROOT -B $(GAME_ICON) \
		"$(GAME_TITLE_JP);$(GAME_SUBTITLE1_JP);$(GAME_SUBTITLE2_JP)" $(_ADDFILES) \
		"$(GAME_TITLE_EN);$(GAME_SUBTITLE1_EN);$(GAME_SUBTITLE2_EN)" $(_ADDFILES) \
		"$(GAME_TITLE_FR);$(GAME_SUBTITLE1_FR);$(GAME_SUBTITLE2_FR)" $(_ADDFILES) \
		"$(GAME_TITLE_DE);$(GAME_SUBTITLE1_DE);$(GAME_SUBTITLE2_DE)" $(_ADDFILES) \
		"$(GAME_TITLE_IT);$(GAME_SUBTITLE1_IT);$(GAME_SUBTITLE2_IT)" $(_ADDFILES) \
		"$(GAME_TITLE_ES);$(GAME_SUBTITLE1_ES);$(GAME_SUBTITLE2_ES)" $(_ADDFILES) \
		-g $(GAME_CODE) $(DEV_CODE) "$(GAME_TITLE)" $(VERSION)
	padbin 0x8000000 $(TARGET).nds
endif

arm7/$(TARGET).elf: FORCE
	$(MAKE) -C arm7

ifdef MMOD
arm9/$(TARGET).elf: FORCE $(_ADDFILES)/SOUND/sound.msl ndstool
else
arm9/$(TARGET).elf: FORCE ndstool
endif
	$(MAKE) -C arm9


$(_ADDFILES)/SOUND/sound.msl:
	$(MAKE) -C audio clean
	$(MAKE) -C audio

ndstool:
	$(MAKE) -C tools/ndstool

clean:
	$(MAKE) -C audio clean
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C tools/ndstool clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9

rebuild: clean all

FORCE: ;
