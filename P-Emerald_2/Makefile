#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

GAME_TITLE	    :=	Pok_mon
GAME_SUBTITLE1	:=  neo Version
GAME_ICON       :=  $(CURDIR)/icon.bmp

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)
export NDSTOOL		:=  $(CURDIR)/tools/ndstool/ndstool

.PHONY: checkarm7 checkarm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: checkarm7 checkarm9 $(TARGET).nds

#---------------------------------------------------------------------------------
checkarm7:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C tools/ndstool
	$(MAKE) -C audio
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
ifneq ($(strip $(NITRO_FILES)),)
_ADDFILES	:=	-d $(NITRO_FILES)
endif

#---------------------------------------------------------------------------------
$(TARGET).nds	: FORCE	arm7/$(TARGET).elf arm9/$(TARGET).elf
	$(NDSTOOL) -c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf -d ./FSROOT -b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1)" $(_ADDFILES)

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C tools/ndstool
	$(MAKE) -C audio
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C tools/ndstool clean
	$(MAKE) -C audio clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9

rebuild: clean $(BUILD)

FORCE: ;
