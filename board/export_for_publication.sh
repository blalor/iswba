#!/bin/bash -e

_EAGLE="/Applications/EAGLE/EAGLE.app/Contents/MacOS/EAGLE"
board_dir="$( cd $(dirname $0) && /bin/pwd )"

cd "${board_dir}"

rm -f iswba.parts.txt iswba.sch.png iswba.brd.png

"${_EAGLE}" -C "script ${board_dir}/export.scr" iswba.brd

## parts list gets exported as iso-8859-1
iconv -f iso-8859-1 -t utf-8 iswba.parts.txt > iswba.parts.txt.converted
mv iswba.parts.txt.converted iswba.parts.txt
