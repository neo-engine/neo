#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

GAME_TITLE_JP	    :=	Pok_mon
GAME_SUBTITLE1_JP	:=  neo Version
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

VERSION			:=	8

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)
export NDSTOOL		:=  $(CURDIR)/tools/ndstool/ndstool

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

#---------------------------------------------------------------------------------
ifneq ($(strip $(NITRO_FILES)),)
_ADDFILES	:=	-d $(NITRO_FILES)
endif

#---------------------------------------------------------------------------------
$(TARGET).nds: FORCE arm7/$(TARGET).elf arm9/$(TARGET).elf
	$(NDSTOOL) -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
		-d ./FSROOT -B $(GAME_ICON) \
		"$(GAME_TITLE_JP);$(GAME_SUBTITLE1_JP);$(GAME_SUBTITLE2_JP)" $(_ADDFILES) \
		"$(GAME_TITLE_EN);$(GAME_SUBTITLE1_EN);$(GAME_SUBTITLE2_EN)" $(_ADDFILES) \
		"$(GAME_TITLE_FR);$(GAME_SUBTITLE1_FR);$(GAME_SUBTITLE2_FR)" $(_ADDFILES) \
		"$(GAME_TITLE_DE);$(GAME_SUBTITLE1_DE);$(GAME_SUBTITLE2_DE)" $(_ADDFILES) \
		"$(GAME_TITLE_IT);$(GAME_SUBTITLE1_IT);$(GAME_SUBTITLE2_IT)" $(_ADDFILES) \
		"$(GAME_TITLE_ES);$(GAME_SUBTITLE1_ES);$(GAME_SUBTITLE2_ES)" $(_ADDFILES) \
		-g ADAE 01 "POKEMON NEO" $(VERSION)

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf: FORCE
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf: FORCE soundbank ndstool
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
soundbank:
	$(MAKE) -C audio

#---------------------------------------------------------------------------------
ndstool:
	$(MAKE) -C tools/ndstool

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C tools/ndstool clean
	$(MAKE) -C audio clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9

rebuild: clean all

FORCE: ;
