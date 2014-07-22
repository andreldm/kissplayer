#!/bin/sh

rm -r autom4te.cache \
autotools \
.deps \
src/.deps \
src/linux/.deps \
src/widget/.deps \
src/win/.deps

rm configure \
aclocal.m4 \
config.h* \
config.status \
Makefile.in \
Makefile \
stamp-h1 \
kissplayer.exe \
kissplayer \
ABOUT-NLS \
*.log \
src/.dirstamp \
src/*.o \
src/linux/.dirstamp \
src/widget/.dirstamp \
src/win/.dirstamp

cd po
rm *.sed *.header *.gmo Rules-quot stamp-po Makefile Makefile.in Makevars.template POTFILES
cd ..

cd m4
rm Makefile Makefile.in
cd ..

find autotools/ -type f -not -name 'config.rpath' -delete
