#!/bin/bash

# ALLFOLDERS=( /home/erlars/C++/GenDeterm/library/pointToPoint/oneSrcOneSnk/toral
# /home/erlars/C++/GenDeterm/library/pointToPoint/oneSrcOneSnk/cyl
# /home/erlars/C++/GenDeterm/library/pointToPoint/oneSrcOneSnk/flat
# /home/erlars/C++/GenDeterm/library/pointToPoint/oneSrcOneSnk/circ )

# ALLFOLDERS=( /home/erlars/C++/GenDeterm/library/pointToPoint/eqSrcsEqSnks/toral
# /home/erlars/C++/GenDeterm/library/pointToPoint/eqSrcsEqSnks/cyl
# /home/erlars/C++/GenDeterm/library/pointToPoint/eqSrcsEqSnks/flat
# /home/erlars/C++/GenDeterm/library/pointToPoint/eqSrcsEqSnks/circ )

# ALLFOLDERS=( /home/erlars/C++/GenDeterm/library/hubsAndSats/oneSrcOneSnk/toral
# /home/erlars/C++/GenDeterm/library/hubsAndSats/oneSrcOneSnk/cyl
# /home/erlars/C++/GenDeterm/library/hubsAndSats/oneSrcOneSnk/flat
# /home/erlars/C++/GenDeterm/library/hubsAndSats/oneSrcOneSnk/circ )

ALLFOLDERS=( /home/erlars/C++/GenDeterm/library/hubsAndSats/eqSrcsEqSnks/toral
/home/erlars/C++/GenDeterm/library/hubsAndSats/eqSrcsEqSnks/cyl
/home/erlars/C++/GenDeterm/library/hubsAndSats/eqSrcsEqSnks/flat
/home/erlars/C++/GenDeterm/library/hubsAndSats/eqSrcsEqSnks/circ )

for dirpath in "${ALLFOLDERS[@]}"
do

	counter=0
	diagfilepath="${dirpath}/diags-gener.txt"
	echo "diagfilepath: ${diagfilepath}"

	for configfile in "$dirpath"/*.par;
	do
		if [ -s "$configfile" ]; then
			echo "configfile: ${configfile}"
			
			shortoutfilename="${configfile%%.*}"
			echo "shortoutfilename: ${shortoutfilename}"
			
			./determ_gen +F ${configfile} -shortOutFileName ${shortoutfilename} &>> ${diagfilepath}
			
			((counter++))
		else			
			echo "ERROR: configuration file ${configfile} could not be opened" >> ${diagfilepath}
			
		fi
	done
	
	echo "counter: ${counter}" >> ${diagfilepath}
	
done