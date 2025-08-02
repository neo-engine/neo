#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# make sure we have bash shell
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
	$(SILENTMSG) $(notdir $@)
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
	$(SILENTMSG) CXX.9 $(notdir $<)
	$(ADD_COMPILE_COMMAND) add $(CXX) "$(CPPFLAGS) $(CXXFLAGS) -c $< -o $@" $<
	$(SILENTCMD)$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.c
	$(SILENTMSG) CC.9 $(notdir $<)
	$(SILENTCMD)$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(ERROR_FILTER)
	$(ADD_COMPILE_COMMAND) add $(CC) "$(CPPFLAGS) $(CFLAGS) -c $< -o $@" $<

#---------------------------------------------------------------------------------
%.o: %.s
	$(SILENTMSG) AS.9 $(notdir $<)
	$(ADD_COMPILE_COMMAND) add $(CC) "-x assembler-with-cpp $(CPPFLAGS) $(ASFLAGS) -c $< -o $@" $<
	$(SILENTCMD)$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(CPPFLAGS) $(ASFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.arm9: %.elf
	$(SILENTCMD)$(OBJCOPY) -O binary $< $@
	$(SILENTMSG) built ... $(notdir $@)

#---------------------------------------------------------------------------------
%.elf:
	$(SILENTMSG) linking $(notdir $@)
	$(ADD_COMPILE_COMMAND) end
	$(SILENTCMD)$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@


TOPDIR ?= $(CURDIR)/..

#---------------------------------------------------------------------------------
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files
# all directories are relative to this makefile
#---------------------------------------------------------------------------------
BUILD		?=	release
SOURCES		:=	source ../common/source
INCLUDES	:=	include ../common/source ../include
DATA		:=	data

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -Os\
			-ffunction-sections -fdata-sections \
			-march=armv5te -mtune=arm946e-s -fomit-frame-pointer\
			-ffast-math -Wno-multichar \
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM9
CXXFLAGS	:=	$(CFLAGS)

ASFLAGS	:=	-g $(ARCH) $(INCLUDE) -DARM9
LDFLAGS	=	-specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

ifneq ($(BUILD),debug)
export ARM9BIN	:=	$(TOPDIR)/lib/libfont.a
else
export ARM9BIN	:=	$(TOPDIR)/lib/libfontd.a
CFLAGS	+=	-DSGIP_DEBUG -DDESQUID
endif


#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBNDS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------


export DEPSDIR := $(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export CC		:=	$(PREFIX)gcc
export CXX		:=	$(PREFIX)g++
export AR		:=	$(PREFIX)ar
export OBJCOPY	:=	$(PREFIX)objcopy

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

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

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr debug release
	@rm -f $(TOPDIR)/lib/libfont*

all: $(ARM9BIN)

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(ARM9BIN)	:	$(OFILES)
	@rm -f "$(ARM9BIN)"
	@$(AR) rcs "$(ARM9BIN)" $(OFILES)
	@echo built ... $(notdir $@)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)


-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
