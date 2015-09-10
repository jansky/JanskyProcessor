#!/bin/sh

sansextension=$(echo "$1" | cut -f 1 -d '.')

if test -e "$sansextension".o; then
    exit 0
fi

cpp -undef -nostdinc -x assembler-with-cpp "$1" | sed 's/^#/;/g' > "$sansextension".sp

if test "$?" -ne 0; then
    echo "Error: Preprocessor failed."
    exit 1
fi

../assembler/jassembler "$sansextension".sp "$sansextension".o && rm "$sansextension".sp

exit "$?"

