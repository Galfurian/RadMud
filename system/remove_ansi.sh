#!/bin/bash
FILES=*.log
for f in $FILES
do
	echo "'Processing $f' ..."
	# Remove ansi codes.
	sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g" $f >> $f.temp
	# remove old log file.
	rm $f
	# Move the temporary cleaned file to original file name.
	mv $f.temp $f
done
