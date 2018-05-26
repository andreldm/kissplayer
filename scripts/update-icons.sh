#!/bin/bash

# Linux icon
convert -density 384 -size 64 -background transparent img/svg/logo_circle.svg src/linux/icon.xpm
sed -i "s/static char \*icon/static const char \*icon_xpm/g" src/linux/icon.xpm

convert -density 384 -size 48 -background transparent img/svg/logo_circle.svg -resize 48 data/icons/hicolor/48x48/apps/kissplayer.png
convert -density 384 -size 64 -background transparent img/svg/logo_circle.svg -resize 64 data/icons/hicolor/64x64/apps/kissplayer.png
rsvg-convert -w 256 -h 256 img/svg/logo_circle.svg -o data/icons/hicolor/scalable/apps/kissplayer.svg

# Windows icon
convert -density 384 -background transparent img/svg/logo_circle.svg -define icon:auto-resize="128,96,64,48,32,16" -colors 4 img/kiss.ico
