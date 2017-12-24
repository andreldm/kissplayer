#!/bin/bash

# This is script is meant to be run from a MSYS2(mingw32) console

MINGW_PATH=C:/msys64/mingw32/bin
OUTPUT=kissplayer
VERSION=`grep AC_INIT configure.ac | cut -d',' -f2 | tr -d '[] '`

echo "* Copying files to '$OUTPUT'"
mkdir -p $OUTPUT

cd po
find . -type f -iname "*.gmo" -print0 | while IFS= read -r -d $'\0' file; do
  lang=`echo "$file" | cut -d'.' -f2 | tr -d '/'`
  mkdir -p ../$OUTPUT/locale/$lang/LC_MESSAGES
  cp "$file" ../$OUTPUT/locale/$lang/LC_MESSAGES/kissplayer.mo
done
cd ..

cp kissplayer.exe $OUTPUT
cp LICENSE $OUTPUT
cp CHANGELOG $OUTPUT
cp README.md $OUTPUT
cp "$MINGW_PATH/fmodex.dll" $OUTPUT
cp "$MINGW_PATH/libgcc_s_dw2-1.dll" $OUTPUT
cp "$MINGW_PATH/libiconv-2.dll" $OUTPUT
cp "$MINGW_PATH/libintl-8.dll" $OUTPUT
cp "$MINGW_PATH/libsigc-2.0-0.dll" $OUTPUT
cp "$MINGW_PATH/libsqlite3-0.dll" $OUTPUT
cp "$MINGW_PATH/libstdc++-6.dll" $OUTPUT
cp "$MINGW_PATH/libtag.dll" $OUTPUT
cp "$MINGW_PATH/libwinpthread-1.dll" $OUTPUT
cp "$MINGW_PATH/mgwfltknox-1.3.dll" $OUTPUT
cp "$MINGW_PATH/zlib1.dll" $OUTPUT

cd $OUTPUT
echo "* Stripping kissplayer.exe"
strip kissplayer.exe

for file in *.dll
do
  # As the distributed fmodex dll is MSVC compiled, stripping it might break it
  case $file in
    fmodex*) continue;
  esac
  echo "* Stripping $file..."
  strip $file
done

command -v upx >/dev/null 2>&1 && {
  echo "* Compressing executable and dlls with UPX..."
  # Stripping all binaries, except libgcc_s_dw2-1.dll
  upx -9 kissplayer.exe fmodex.dll libiconv-2.dll \
  libsqlite3-0.dll libtag.dll libwinpthread-1.dll \
  libintl-8.dll mgwfltknox-1.3.dll zlib1.dll
} || {
  echo "* UPX not found, skipping executable compression..."
}

echo "* Checking kissplayer.exe..."
./kissplayer.exe || {
  echo "kissplayer.exe doesn't seem to be working"
  exit;
}

cd ..

command -v 7z >/dev/null 2>&1 && {
  echo "* Zipping..."
  7z a -tzip "kissplayer-$VERSION.zip" kissplayer
} || {
  echo "* 7z not found, skipping zipping..."
}

echo "* Done!"
