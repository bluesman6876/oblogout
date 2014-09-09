#!/bin/sh
remove_list="Makefile.in
src/Makefile.in
themes/Makefile.in
aclocal.m4
compile
config.guess
config.h.in
config.sub
configure
depcomp
install-sh
intltool-extract.in
intltool-merge.in
intltool-update.in
ltmain.sh
missing"

make maintainer-clean
rm -rf $remove_list
