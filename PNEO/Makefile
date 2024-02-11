.SUFFIXES:
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

GAME_TITLE_JP	    :=  ポケットモンスター
GAME_SUBTITLE1_JP	:=  ネオ
GAME_SUBTITLE2_JP	:=  Homebrew

GAME_TITLE_EN_2	    :=	Pokemon

GAME_TITLE_EN	    :=	Pokémon
GAME_SUBTITLE1_EN	:=  neo Version
GAME_SUBTITLE2_EN	:=  Homebrew

GAME_TITLE_FR	    :=	Pokémon
GAME_SUBTITLE1_FR	:=  Version neo
GAME_SUBTITLE2_FR	:=  Homebrew

GAME_TITLE_DE	    :=	Pokémon
GAME_SUBTITLE1_DE	:=  neo Edition
GAME_SUBTITLE2_DE	:=  Homebrew

GAME_TITLE_IT	    :=	Pokémon
GAME_SUBTITLE1_IT	:=  Versione neo
GAME_SUBTITLE2_IT	:=  Homebrew

GAME_TITLE_ES	    :=	Pokémon
GAME_SUBTITLE1_ES	:=  Edición neo
GAME_SUBTITLE2_ES	:=  Homebrew

GAME_ICON       :=  $(CURDIR)/icon.bmp

export DEV_CODE			:=	01
export GAME_CODE		:=	ADAJ
export GAME_TITLE		:=	POKEMON NEO
export VERSION			:=	10
export VERSION_NAME		:=	Mimicking Mimikyu

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)
export NDSTOOL		:=  $(CURDIR)/tools/ndstool/ndstool

#main targets
all: $(TARGET).nds

ifneq ($(strip $(NITRO_FILES)),)
_ADDFILES	:=	-d $(NITRO_FILES)
endif

ifdef NDSTOOL
$(TARGET).nds: FORCE arm7/$(TARGET).elf arm9/$(TARGET).elf
	ndstool -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -d ./FSROOT -b $(GAME_ICON) "$(GAME_TITLE_EN_2);$(GAME_SUBTITLE1_EN);$(GAME_SUBTITLE2_EN)" $(_ADDFILES)
else
ifdef FLASHCARD
$(TARGET).nds: FORCE arm7/$(TARGET).elf arm9/$(TARGET).elf
	$(NDSTOOL) -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -d ./FSROOT -b $(GAME_ICON) "$(GAME_TITLE_EN);$(GAME_SUBTITLE1_EN);$(GAME_SUBTITLE2_EN)" $(_ADDFILES)
else
$(TARGET).nds: FORCE arm7/$(TARGET).elf arm9/$(TARGET).elf
	$(NDSTOOL) -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
		-d ./FSROOT -bi $(GAME_ICON) \
		-bt0 "$(GAME_TITLE_JP);$(GAME_SUBTITLE1_JP);$(GAME_SUBTITLE2_JP)" $(_ADDFILES) \
		-bt1 "$(GAME_TITLE_EN);$(GAME_SUBTITLE1_EN);$(GAME_SUBTITLE2_EN)" $(_ADDFILES) \
		-bt2 "$(GAME_TITLE_FR);$(GAME_SUBTITLE1_FR);$(GAME_SUBTITLE2_FR)" $(_ADDFILES) \
		-bt3 "$(GAME_TITLE_DE);$(GAME_SUBTITLE1_DE);$(GAME_SUBTITLE2_DE)" $(_ADDFILES) \
		-bt4 "$(GAME_TITLE_IT);$(GAME_SUBTITLE1_IT);$(GAME_SUBTITLE2_IT)" $(_ADDFILES) \
		-bt5 "$(GAME_TITLE_ES);$(GAME_SUBTITLE1_ES);$(GAME_SUBTITLE2_ES)" $(_ADDFILES) \
		-g $(GAME_CODE) $(DEV_CODE) "$(GAME_TITLE)" $(VERSION)
	padbin 0x8000000 $(TARGET).nds
endif
endif

arm7/$(TARGET).elf: FORCE
	$(MAKE) -C arm7

arm9/$(TARGET).elf: FORCE ndstool
	$(MAKE) -C arm9

ndstool:
	$(MAKE) -C tools/ndstool

clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C tools/ndstool clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9

rebuild: clean all

FORCE: ;
