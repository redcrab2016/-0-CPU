#!/bin/bash

set -u
# path location of this script
scriptpath=${0%/*}
pushd "$scriptpath" 1>/dev/null 2>&1
scriptpath="`pwd`"
popd 1>/dev/null 2>&1

# convert a 256 values channel in a 6 value channel
# $scriptpath/bs3gfxlogo.inc
function convert256to6
{
# from 0 to 26    : 0
  if [ $1 -le 26 ]; then
    return 0
  fi
  
# from 27 to 77   : 1
  if [ $1 -le 77 ]; then
    return 1
  fi
  
# from 78 to 128  : 2
  if [ $1 -le 128 ]; then
    return 2
  fi
  
# from 129 to 179 : 3
  if [ $1 -le 179 ]; then
    return 3
  fi
  
# from 180 to 230 : 4
  if [ $1 -le 230 ]; then
    return 4
  fi
  
# from 231 to 255 : 5
  return 5
}

cc=0;
echo "/* Condensed  bs3gfxlogo.data */" > $scriptpath/bs3gfxlogo.inc

echo -n 'BYTE bs3gfxlogo[]={' >> $scriptpath/bs3gfxlogo.inc
hexdump -v -e '4/1 "%02d,"' -e '"\n"' $scriptpath/bs3gfxlogo.data | while IFS=',' read -r red green blue alpha; do

# echo -n " red $red, green $green, blue $blue"
convert256to6 $red
red=$?
convert256to6 $green
green=$?
convert256to6 $blue
blue=$?
# echo -n " === red $red, green $green, blue $blue"
ansi=$(( 16 + $red * 36 + $green * 6 + $blue ))


echo "$ansi"

done | uniq -c  | while read -r count ansicolor; do
  while [[ $count -gt 255 ]]; do
    echo -n 255,$ansicolor,
    cc=$(( cc + 1 ))
    if [ $cc -ge 32 ]; then
      cc=0
      echo " "
    fi
    count=$(( count - 255 ))
  done
  if [[ $count -gt 0 ]]; then
    echo -n $count,$ansicolor,
    cc=$(( cc + 1 ))
    if [ $cc -ge 16 ]; then
      cc=0
      echo " " 
    fi
  fi
done >> $scriptpath/bs3gfxlogo.inc
echo '0,0};' >> $scriptpath/bs3gfxlogo.inc
