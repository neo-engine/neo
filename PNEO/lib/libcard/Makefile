export TOPDIR	:=	$(CURDIR)

export LIBCARD_MAJOR	:= 0
export LIBCARD_MINOR	:= 1
export LIBCARD_REVISION	:= 0

VERSION	:=	$(LIBCARD_MAJOR).$(LIBCARD_MINOR).$(LIBCARD_REVISION)

#Assumes that the Backup is a 512k flash memory (512 * 1024)
export BACKUP_SIZE := 524288

.PHONY: release debug clean all

all: include/libcardversion.h release debug

include/libcardversion.h : Makefile
	@echo "#ifndef __LIBCARDVERSION_H__" > $@
	@echo "#define __LIBCARDVERSION_H__" >> $@
	@echo >> $@
	@echo "#define LIBCARD_MAJOR    $(LIBCARD_MAJOR)" >> $@
	@echo "#define LIBCARD_MINOR    $(LIBCARD_MINOR)" >> $@
	@echo "#define LIBCARD_REVISION $(LIBCARD_REVISION)" >> $@
	@echo >> $@
	@echo '#define _LIBCARD_STRING "libcard Release '$(LIBCARD_MAJOR).$(LIBCARD_MINOR).$(LIBCARD_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __LIBCARDVERSION_H__" >> $@


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
	@$(RM) -r include/libcardversion.h lib

