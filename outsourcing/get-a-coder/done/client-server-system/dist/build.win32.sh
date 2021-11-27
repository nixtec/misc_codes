#!/bin/sh

# you _must_ have cross-tools installed to cross-compile this project

MAKE="cross-make.sh"

#${MAKE} clean

${MAKE} -f Makefile.win32 "$@"
