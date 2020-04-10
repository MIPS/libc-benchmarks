#!/bin/bash

OBJDIR=$1
TARGET=`basename $OBJDIR`
OUT=$2

for i in $( seq 5 18 ); do
    $TOOLS/bin/$TARGET"-readelf" -s $OBJDIR/bench-switch_case"$i"_O2.elf | \
	grep -e jumptab | awk '{print $8,$3}' | \
	sed s/jumptab//g | sort -n | \
	awk -v i="$i" '{if ($1 >= i) print $1,$2+$1*4; else print $1,$2;}' \
	    > $OUT/nmips-switch_c"$i"_O2_size.csv

    $TOOLS/bin/$TARGET"-readelf" -s $OBJDIR/bench-switch_case"$i"_O2.elf | \
	grep -e jumptab | awk '{print $8,$3}' | \
	sed s/jumptab//g | sort -n | \
	awk -v i="$i" '{if ($1 >= i) print $1,$2,$1*4; else print $1,$2,0;}' \
	    > $OUT/nmips-switch_c"$i"_O2_cd.csv

    
    $TOOLS/bin/$TARGET"-readelf" -s $OBJDIR/bench-switch_case"$i"_Os.elf | \
	grep -e jumptab | awk '{print $8,$3}' | \
	sed s/jumptab//g | sort -n | \
	awk -v i="$i" '{if ($1 >= i) print $1,$2+$1*4; else print $1,$2;}' \
	    > $OUT/nmips-switch_c"$i"_Os_size.csv

    $TOOLS/bin/$TARGET"-readelf" -s $OBJDIR/bench-switch_case"$i"_Os.elf | \
	grep -e jumptab | awk '{print $8,$3}' | \
	sed s/jumptab//g | sort -n | \
	awk -v i="$i" '{if ($1 >= i) print $1,$2,$1*4; else print $1,$2,0;}' \
	    > $OUT/nmips-switch_c"$i"_Os_cd.csv
done
