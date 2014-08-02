#!/bin/sh

echo Cleaning...
rm -r autom4te.cache autotools locale .deps 2> /dev/null
rm configure aclocal.m4 config.* Makefile.in Makefile stamp-h1 kissplayer.exe kissplayer ABOUT-NLS *.log 2> /dev/null

cd src
echo src...
rm -r .deps 2> /dev/null
rm .dirstamp *.o 2> /dev/null
cd ..

cd src/linux
echo src/linux...
rm -r .deps 2> /dev/null
rm .dirstamp *.o 2> /dev/null
cd ../..

cd src/widget
echo src/widget...
rm -r .deps 2> /dev/null
rm .dirstamp *.o 2> /dev/null
cd ../..

cd src/win
echo src/win...
rm -r .deps 2> /dev/null
rm .dirstamp *.o 2> /dev/null
cd ../..

cd po
echo po...
rm *.sed *.header *.gmo *.mo remove-potcdate.sed Rules-quot stamp-po Makefile Makefile.in Makevars.template POTFILES 2> /dev/null
cd ..

cd m4
echo m4...
rm Makefile Makefile.in 2> /dev/null
cd ..

echo Done