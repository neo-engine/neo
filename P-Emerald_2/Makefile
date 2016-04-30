#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

GAME_TITLE	    :=	Pokemon
GAME_SUBTITLE1	:=  Smaragd Edition 2
GAME_ICON       :=  $(CURDIR)/icon.bmp

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)


.PHONY: $(TARGET).arm7 $(TARGET).arm9

ifneq ($(strip $(NITRO_FILES)),)
_ADDFILES	:=	-d $(NITRO_FILES)
endif

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).ds.gba

$(TARGET).ds.gba	: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9 -d ./FSROOT -b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1)" $(_ADDFILES)

#---------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9

rebuild: clean $(BUILD)