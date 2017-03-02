#!/bin/sh

# Packagers set DESTDIR so we don't want to try writing to root
if [ -z $DESTDIR ]; then

	PREFIX=${MESON_INSTALL_PREFIX:-/usr}

	echo 'Updating desktop database'
	update-desktop-database -q
	echo 'Updating icon cache'
	gtk-update-icon-cache -q -t -f "$PREFIX/share/icons/hicolor"

fi
