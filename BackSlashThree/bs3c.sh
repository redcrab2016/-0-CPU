#!/bin/bash
#    ___ ___ ____          
#   | _ ) __|__ /          
#   | _ \__ \|_ \          
#   |___/___/___/      ___ 
#   |_   _(_)_ _ _  _ / __|
#     | | | | ' \ || | (__ 
#     |_| |_|_||_\_, |\___|
#                |__/      
# tool chain to compile .tc or .asm file
# if .tc, the chain is cpp -> tinyc -> bs3asm
# other wise it is only bs3asm
set -u
last=${@:$#} # last parameter (source file name)
p="$(dirname "$0")" # get this script directory path

if [ "$last" == "" ]; then
  echo "Missing parameter"
  exit 1
fi

ext="${last##*.}"
if [ "$ext" == "tc" ]; then
    # process .tc file
    other=${*%${!#}} # all parameters except the last
    basename="${last%.*}"
    nametinyc="${basename}.tc_"
    nameasm="${basename}.asm"
    namecpperr="${basename}.cpperr"
    cpp -I "$p/include" "$last" |  grep -E -v '^# [0-9]+.*$' 2> "$namecpperr" > "$nametinyc"
    result=$?
    if [ "$result" != "0" ]; then 
        cat "$namecpperr"
        rm -f "$namecpperr"
        echo "Error with preprocessing of $last"
        exit 1
    fi
    rm -f "$namecpperr"
    cat "$nametinyc" | java -jar $p/bs3tinyc.jar $@ > "$nameasm"
    result=$?
    if [ "$result" != "0" ]; then 
        echo "Error with tinyc of $nametinyc"
        cat "$nameasm"
        rm -f "$nameasm"
        exit 1
    fi
    "$p/bs3asm" $other "$nameasm"
    exit $?
else
    # process as .asm or .s or .bs3 file
  "$p/bs3asm" $@
  exit $?
fi

