#!/bin/bash

# Based on: https://github.com/the-cavalry/light-locker

# Usage: [language]
# If language is given only that language will be downloaded from transifex.
# If no language is given all languages will be downloaded from transifex.

if [ -r .transifex-user ] ; then
  USERNAME=$(< .transifex-user)
else
  read -p "Username [$USER]: " USERNAME
  echo ${USERNAME:=$USER} > .transifex-user
fi
PASSWORD=

if [ -n "$1" ] ; then
  LANGUAGES="$1"
else
  echo Enter your password once or skip this and enter your password for every file.
  read -p "Password:" -s PASSWORD
  echo
  [ -n "$PASSWORD" ] && PASSWORD=":$PASSWORD"
  echo Collecting all languages...
  LANGUAGES=$(curl -s -L --user "$USERNAME$PASSWORD" -X GET "http://www.transifex.com/api/2/project/kissplayer/languages/" | jshon -a -e "language_code" -u)

  echo Downloading pot file...
  curl -L --user "$USERNAME$PASSWORD" -X GET "http://www.transifex.com/api/2/project/kissplayer/resource/kissplayer/content/?file" -o "po/kissplayer.pot"
fi

for L in $LANGUAGES ; do
  echo "Downloading language $L..."
  curl -L --user "$USERNAME$PASSWORD" -X GET "http://www.transifex.com/api/2/project/kissplayer/resource/kissplayer/translation/$L/?file" | sed "s/YEAR/$(date +'%Y')/" | sed "s/SOME DESCRIPTIVE TITLE/$L translation for KISS Player/" > "po/$L.po"
done

cat > po/LINGUAS << EOF
# please keep this list sorted alphabetically

EOF
ls po/*.po | sed 's@^po/@@;s@\.po$@@' >> po/LINGUAS

