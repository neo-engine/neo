#!/bin/sh

arm-none-eabi-gcc -M -Iarm9/include -Iarm9/source -Iarm9/build -I/opt/devkitpro/libnds/include -DARM9 arm9/source/*  | sed -e 's/[\\ ]/\n/g' | sed -e '/^$/d' -e '/\.o:[ \t]*$/d' | awk '!a[$0]++' | ctags  -L - --fields=+l --exclude="/opt/devkitpro/devkitARM/arm-none-eabi/**" --exclude="/opt/devkitpro/devkitARM/lib/gcc/**"  -f ./tags
