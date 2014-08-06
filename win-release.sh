#!/bin/bash

# This is script is meant to be run from a MSYS console

MINGW_PATH=C:/MinGW/bin/
OUTPUT=kissplayer
VERSION=`grep AC_INIT configure.ac | cut -d',' -f2 | tr -d '[] '`

containsElement() {
  local e
  for e in "${@:2}"; do [[ "$e" == "$1" ]] && return 1; done
  return 0
}

win_dlls=(advapi32.dll comctl32.dll gdi32.dll kernel32.dll msvcrt.dll ole32.dll shell32.dll user32.dll)

mkdir -p $OUTPUT

cd po

find . -type f -iname "*.gmo" -print0 | while IFS= read -r -d $'\0' file; do
  lang=`echo "$file" | cut -d'.' -f2 | tr -d '/'`
  mkdir -p ../$OUTPUT/locale/$lang/LC_MESSAGES
  cp "$file" ../$OUTPUT/locale/$lang/LC_MESSAGES/kissplayer.mo
done

cd ..

objdump -p kissplayer.exe | grep "DLL Name:" | while read -r dll; do
  dll=`echo $dll | cut -d' ' -f3`
  containsElement $dll "${win_dlls[@]}"
  if [ "$?" -eq "1" ]; then
    continue
  fi
  if [ ! -f $MINGW_PATH$dll ]; then
    echo "$MINGW_PATH$dll not found!"
    continue
  fi
  cp $MINGW_PATH$dll $OUTPUT
done

strip kissplayer.exe
cp kissplayer.exe $OUTPUT
cp LICENSE.txt $OUTPUT
cp CHANGELOG.txt $OUTPUT
cp README.txt $OUTPUT

7z a -tzip "kissplayer-$VERSION.zip" kissplayer > /dev/null

echo done
