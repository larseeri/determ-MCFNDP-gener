#!/bin/bash

# ALLFOLDERS=( ../library/pointToPoint/oneSrcOneSnk/toral )
# ALLFOLDERS=( ../library/pointToPoint/oneSrcOneSnk/cyl )
# ALLFOLDERS=( ../library/pointToPoint/oneSrcOneSnk/flat )
# ALLFOLDERS=( ../library/pointToPoint/oneSrcOneSnk/circ )

# ALLFOLDERS=( ../library/pointToPoint/eqSrcsEqSnks/toral)
# ALLFOLDERS=( ../library/pointToPoint/eqSrcsEqSnks/cyl )
# ALLFOLDERS=( ../library/pointToPoint/eqSrcsEqSnks/flat )
# ALLFOLDERS=( ../library/pointToPoint/eqSrcsEqSnks/circ )

# ALLFOLDERS=( ../library/hubsAndSats/oneSrcOneSnk/toral )
# ALLFOLDERS=( ../library/hubsAndSats/oneSrcOneSnk/cyl )
# ALLFOLDERS=( ../library/hubsAndSats/oneSrcOneSnk/flat )
# ALLFOLDERS=( ../library/hubsAndSats/oneSrcOneSnk/circ )

# ALLFOLDERS=( ../library/hubsAndSats/eqSrcsEqSnks/toral )
# ALLFOLDERS=( ../library/hubsAndSats/eqSrcsEqSnks/cyl )
# ALLFOLDERS=( ../library/hubsAndSats/eqSrcsEqSnks/flat )
# ALLFOLDERS=( ../library/hubsAndSats/eqSrcsEqSnks/circ )


#ALLFOLDERS=( ../library/pointToPoint/oneSrcOneSnk/toral ../library/pointToPoint/oneSrcOneSnk/cyl ../library/pointToPoint/oneSrcOneSnk/flat ../library/pointToPoint/oneSrcOneSnk/circ )

#ALLFOLDERS=( ../library/pointToPoint/eqSrcsEqSnks/toral ../library/pointToPoint/eqSrcsEqSnks/cyl ../library/pointToPoint/eqSrcsEqSnks/flat  ../library/pointToPoint/eqSrcsEqSnks/circ )

#ALLFOLDERS=( ../library/hubsAndSats/oneSrcOneSnk/toral ../library/hubsAndSats/oneSrcOneSnk/cyl ../library/hubsAndSats/oneSrcOneSnk/flat ../library/hubsAndSats/oneSrcOneSnk/circ )

#ALLFOLDERS=( ../library/hubsAndSats/eqSrcsEqSnks/toral ../library/hubsAndSats/eqSrcsEqSnks/cyl ../library/hubsAndSats/eqSrcsEqSnks/flat ../library/hubsAndSats/eqSrcsEqSnks/circ )


for dirpath in "${ALLFOLDERS[@]}"
do
	counter=0
	diagfilepath="${dirpath}/diags-comput.txt"
		
	echo "diagfilepath: ${diagfilepath}"
	
	TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
	
	echo "timestamp: ${TIMESTAMP}" >> ${diagfilepath}
	
	for instfile in "$dirpath"/*.std;
	do
		if [ -s "$instfile" ]; then
			echo "instfile: ${instfile}"
			
			echo -e "\n" >> ${diagfilepath} # -e: enables backslash escapes
			echo "${instfile}" >> ${diagfilepath}
			
			./determ_feasib -F G -I ${instfile} &>> ${diagfilepath}			
			
			((counter++))			
			echo "number of files processed so far: ${counter}" >> ${diagfilepath}			
		else			
			echo "ERROR: instance file ${instfile} could not be opened" >> ${diagfilepath}			
		fi
	done	
	
done