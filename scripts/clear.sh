#!/bin/sh

echo Cleaning...
rm -rf autom4te.cache autotools locale .deps 2> /dev/null
rm -f configure aclocal.m4 config.* Makefile.in Makefile stamp-h1 kissplayer.exe kissplayer ABOUT-NLS *.log 2> /dev/null

cd src
echo src...
rm -rf .deps 2> /dev/null
rm -f .dirstamp *.o 2> /dev/null
cd ..

cd src/linux
echo src/linux...
rm -rf .deps 2> /dev/null
rm -f .dirstamp *.o 2> /dev/null
cd ../..

cd src/widget
echo src/widget...
rm -rf .deps 2> /dev/null
rm -f .dirstamp *.o 2> /dev/null
cd ../..

cd src/win
echo src/win...
rm -r .deps 2> /dev/null
rm -f .dirstamp *.o 2> /dev/null
cd ../..

cd data
echo data...
find . -name "Makefile" -o -name "Makefile.in" -type f -delete 2> /dev/null
rm -f kissplayer.desktop 2> /dev/null
cd ..

cd po
echo po...
rm -f *.sed *.header *.gmo *.mo remove-potcdate.sed Rules-quot stamp-po Makefile Makefile.in Makevars.template POTFILES 2> /dev/null
cd ..

cd m4
echo m4...
rm -f Makefile Makefile.in 2> /dev/null
cd ..

echo Done
