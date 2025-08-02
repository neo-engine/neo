#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# make sure we have bash
#---------------------------------------------------------------------------------
export SHELL := /usr/bin/env bash

#---------------------------------------------------------------------------------
# path to tools
#---------------------------------------------------------------------------------
DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
export PATH	:=	$(DEVKITPATH)/tools/bin:$(DEVKITPATH)/devkitARM/bin:$(PATH)

#---------------------------------------------------------------------------------
# the prefix on the compiler executables
#---------------------------------------------------------------------------------
PREFIX		:=	arm-none-eabi-

export CC	:=	$(PREFIX)gcc
export CXX	:=	$(PREFIX)g++
export AR	:=	$(PREFIX)gcc-ar
export OBJCOPY	:=	$(PREFIX)objcopy
export STRIP	:=	$(PREFIX)strip


ISVC=$(or $(VCBUILDHELPER_COMMAND),$(MSBUILDEXTENSIONSPATH32),$(MSBUILDEXTENSIONSPATH))

ifneq (,$(ISVC))
	ERROR_FILTER	:=	2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):/\1(\2):/g'
endif


#---------------------------------------------------------------------------------
# allow seeing compiler command lines with make V=1 (similar to autotools' silent)
#---------------------------------------------------------------------------------
ifeq ($(V),1)
    SILENTMSG := @true
    SILENTCMD :=
else
    SILENTMSG := @echo
    SILENTCMD := @
endif


#---------------------------------------------------------------------------------
# canned command sequence for binary data
#---------------------------------------------------------------------------------
define bin2o
        $(eval _tmpasm := $(shell mktemp))
        $(SILENTCMD)bin2s -a 4 -H `(echo $(<F) | tr . _)`.h $< > $(_tmpasm)
        $(SILENTCMD)$(CC) -x assembler-with-cpp $(CPPFLAGS) $(ASFLAGS) -c $(_tmpasm) -o $(<F).o
        @rm $(_tmpasm)
endef

#---------------------------------------------------------------------------------
# Generate compile commands
#---------------------------------------------------------------------------------
ifeq ($(GENERATE_COMPILE_COMMANDS),1)
    ADD_COMPILE_COMMAND := @/opt/devkitpro/tools/bin/generate_compile_commands
else
    ADD_COMPILE_COMMAND := @true
endif


#---------------------------------------------------------------------------------
%.a:
#---------------------------------------------------------------------------------
	$(SILENTMSG) AR.7 $(notdir $@)
	@rm -f $@
	$(SILENTCMD)$(AR) -rc $@ $^

#---------------------------------------------------------------------------------
%.arm.o: CFLAGS    += -marm
%.arm.o: CXXFLAGS  += -marm

#---------------------------------------------------------------------------------
%.thumb.o: CFLAGS    += -mthumb
%.thumb.o: CXXFLAGS  += -mthumb

#---------------------------------------------------------------------------------
%.iwram.o: CFLAGS    += -marm -mlong-calls
%.iwram.o: CXXFLAGS  += -marm -mlong-calls

#---------------------------------------------------------------------------------
%.itcm.o: CFLAGS     += -marm -mlong-calls
%.itcm.o: CXXFLAGS   += -marm -mlong-calls

#---------------------------------------------------------------------------------
%.o: %.cpp
	$(SILENTMSG) CC.7 $(notdir $<)
	$(ADD_COMPILE_COMMAND) add $(CXX) "$(CPPFLAGS) $(CXXFLAGS) -c $< -o $@" $<
	$(SILENTCMD)$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.c
	$(SILENTMSG) CC.7 $(notdir $<)
	$(SILENTCMD)$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(ERROR_FILTER)
	$(ADD_COMPILE_COMMAND) add $(CC) "$(CPPFLAGS) $(CFLAGS) -c $< -o $@" $<

#---------------------------------------------------------------------------------
%.o: %.s
	$(SILENTMSG) AS.7 $(notdir $<)
	$(ADD_COMPILE_COMMAND) add $(CC) "-x assembler-with-cpp $(CPPFLAGS) $(ASFLAGS) -c $< -o $@" $<
	$(SILENTCMD)$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(CPPFLAGS) $(ASFLAGS) -c $< -o $@ $(ERROR_FILTER)

ifneq ($(strip $(NITRO_FILES)),)
_ADDFILES	:=	-d $(NITRO_FILES)
endif

#---------------------------------------------------------------------------------
%.arm7: %.elf
	$(SILENTCMD)$(OBJCOPY) -O binary $< $@
	$(SILENTMSG) built ... $(notdir $@)

#---------------------------------------------------------------------------------
%.elf:
	$(SILENTMSG) linking $(notdir $@)
	$(ADD_COMPILE_COMMAND) end
	$(SILENTCMD)$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@


#---------------------------------------------------------------------------------
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files
# all directories are relative to this makefile
#---------------------------------------------------------------------------------
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include ../common build
DATA		:=

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb-interwork

CFLAGS	:=	-g -Wall -O3 -D_FORTIFY_SOURCE=1 -ffast-math -fgcse-sm -fgcse-las -fgcse-after-reload \
			-funsafe-loop-optimizations $(EXTRA_FLAGS) \
			-DGAME_CODE="\"$(GAME_CODE)\"" -DVERSION=$(VERSION) \
			-DGAME_TITLE="\"$(GAME_TITLE)\"" -DVERSION_NAME="\"$(VERSION_NAME)\""
			-mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer\
			$(ARCH)

ifdef DESQUID
CFLAGS	+=	-DDESQUID
endif
ifdef NOSOUND
CFLAGS	+=	-DNO_SOUND
endif
ifdef FLASHCARD
CFLAGS	+=	-DFLASHCARD
endif

CFLAGS	+=	$(INCLUDE) -DARM7
CXXFLAGS	:=	$(CFLAGS) -fno-rtti -fno-exceptions -fno-rtti


ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=ds_arm7.specs -g $(ARCH) -Wl,--nmagic -Wl,-Map,$(notdir $*).map

LIBS	:=	-ldswifi7 -lnds7

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------

LIBPATH	:=	../lib/
LIBDIRS	:=	$(foreach dir,$(LIBPATH),$(sort $(dir $(wildcard $(dir)/*/))))


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export ARM7ELF	:=	$(CURDIR)/$(TARGET).elf
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))


export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(abspath $(CURDIR)/$(dir))) \
			$(foreach dir,$(LIBDIRS),-I$(abspath $(dir)/include)) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(abspath $(dir)/lib))


#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	$(SILENTMSG) clean ...
	$(SILENTCMD)rm -fr $(BUILD) *.elf


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(ARM7ELF)	:	$(OFILES)
	$(SILENTMSG) linking $(notdir $@)
	$(SILENTCMD)$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@


#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	$(SILENTMSG)echo bin2o.7 $(notdir $<)
	$(SILENTCMD)$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
