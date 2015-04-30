#!/bin/bash

# Based on: https://github.com/the-cavalry/light-locker

# Usage:
# Upload the pot file to transifex.

[ -f "po/kissplayer.pot" ] || { echo "pot file not found. Run make -C po update-po" ; exit 1 ; }

if [ -r .transifex-dduser ] ; then
  USERNAME=$(< .transifex-user)
else
  read -p "Username [$USER]: " USERNAME
  echo ${USERNAME:=$USER} > .transifex-user
fi

echo Uploading new pot file...
curl -i -L --user "$USERNAME" -F "file=@po/kissplayer.pot" -X PUT "http://www.transifex.com/api/2/project/kissplayer/resource/kissplayer/content/"
echo
