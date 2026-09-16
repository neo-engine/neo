export TOPDIR	:=	$(CURDIR)

export LIBSOUND_MAJOR	:= 0
export LIBSOUND_MINOR	:= 1
export LIBSOUND_REVISION	:= 0

VERSION	:=	$(LIBSOUND_MAJOR).$(LIBSOUND_MINOR).$(LIBSOUND_REVISION)

.PHONY: release debug clean all

all: include/libsound_version.h release debug

include/libsound_version.h : Makefile
	@echo "#ifndef _libsound_version_h_" > $@
	@echo "#define _libsound_version_h_" >> $@
	@echo >> $@
	@echo "#define LIBSOUND_MAJOR    $(LIBSOUND_MAJOR)" >> $@
	@echo "#define LIBSOUND_MINOR    $(LIBSOUND_MINOR)" >> $@
	@echo "#define LIBSOUND_REVISION $(LIBSOUND_REVISION)" >> $@
	@echo >> $@
	@echo '#define LIBSOUND_VERSION "'$(LIBSOUND_MAJOR).$(LIBSOUND_MINOR).$(LIBSOUND_REVISION)'"' >> $@
	@echo >> $@
	@echo "#endif // _libsound_version_h_" >> $@


#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	echo $(LIBNDS)
	$(MAKE) -C arm9 BUILD=release
	$(MAKE) -C arm7 BUILD=release

#-------------------------------------------------------------------------------
debug: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=debug
	$(MAKE) -C arm7 BUILD=debug

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib

#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@$(MAKE) -C arm9 clean
	@$(MAKE) -C arm7 clean

