export TOPDIR	:=	$(CURDIR)

export LIBFONT_MAJOR	:= 0
export LIBFONT_MINOR	:= 0
export LIBFONT_REVISION	:= 1

VERSION	:=	$(LIBFONT_MAJOR).$(LIBFONT_MINOR).$(LIBFONT_REVISION)

.PHONY: release debug clean all

all: include/libfontversion.h release debug

include/libfontversion.h : Makefile
	@echo "#ifndef __LIBFONTVERSION_H__" > $@
	@echo "#define __LIBFONTVERSION_H__" >> $@
	@echo >> $@
	@echo "#define LIBFONT_MAJOR    $(LIBFONT_MAJOR)" >> $@
	@echo "#define LIBFONT_MINOR    $(LIBFONT_MINOR)" >> $@
	@echo "#define LIBFONT_REVISION $(LIBFONT_REVISION)" >> $@
	@echo >> $@
	@echo '#define _LIBFONT_STRING "libfont Release '$(LIBFONT_MAJOR).$(LIBFONT_MINOR).$(LIBFONT_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __LIBFONTVERSION_H__" >> $@


#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	echo $(LIBNDS)
	$(MAKE) -C arm9 BUILD=release

#-------------------------------------------------------------------------------
debug: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=debug

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib

#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@$(MAKE) -C arm9 clean
	@$(RM) -r include/libfontversion.h lib

