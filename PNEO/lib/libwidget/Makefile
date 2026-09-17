export TOPDIR	:=	$(CURDIR)

export LIBWIDGET_MAJOR	:= 0
export LIBWIDGET_MINOR	:= 1
export LIBWIDGET_REVISION	:= 0

VERSION	:=	$(LIBWIDGET_MAJOR).$(LIBWIDGET_MINOR).$(LIBWIDGET_REVISION)

.PHONY: release debug clean all

all: include/libwidgetversion.h release debug

include/libwidgetversion.h : Makefile
	@echo "#ifndef __LIBWIDGETVERSION_H__" > $@
	@echo "#define __LIBWIDGETVERSION_H__" >> $@
	@echo >> $@
	@echo "#define LIBWIDGET_MAJOR    $(LIBWIDGET_MAJOR)" >> $@
	@echo "#define LIBWIDGET_MINOR    $(LIBWIDGET_MINOR)" >> $@
	@echo "#define LIBWIDGET_REVISION $(LIBWIDGET_REVISION)" >> $@
	@echo >> $@
	@echo '#define _LIBWIDGET_STRING "libwidget Release '$(LIBWIDGET_MAJOR).$(LIBWIDGET_MINOR).$(LIBWIDGET_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __LIBWIDGETVERSION_H__" >> $@


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
	@$(RM) -r include/libwidgetversion.h lib

