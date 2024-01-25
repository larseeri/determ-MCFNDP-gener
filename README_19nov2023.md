Copyright (c) 2023 Eric Larsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

****************************************************************************
****************************************************************************

### Generator of instances of the linear, deterministic, multi-commodity, fixed charge, capacitated network design problem: User's guide (19Nov2023)

This application randomly generates instances of the linear, deterministic, multi-commodity, capacitated, fixed charge network design (MCFND) problem. Details of the process are governed by user-specified configuration settings.

For detailed background, see
Larsen, E., Bisaillon, S., Cordeau, J.F. and Frejinger, E.
Pseudo-random Instance Generators in C++ for Deterministic and Stochastic 
Multi-commodity Network Design Problems. 

#### 1 Building the program

Under Linux, follow these steps to build the program:

a- Uncompress the archive file containing the program files.<br/>
b- In the terminal, go to the directory just created.<br/>
c- Run this command:  make<br/>
	to compile the program.<br/>
d- An executable file named 'exe' will be created.

#### 2 Running the program

The program can be run with default option parameter values as follows:

./exe

By default, the program will read some options from the test.par file.
(This default setting can be updated by modifying line 75 in file
main.cpp.) The default file name can be overridden with the option +F:

./exe +F newParamfile.txt

The option parameters file should have this format:

optionParameter1   <value><br/>
optionParameter2   <value><br/>
.<br/>
.<br/>
optionParameterN   <value>

For options expecting boolean values, no values will be read: presence
of option name is sufficient to toggle the corresponding value to true.
By default, boolean values are set to false.
For other options expecting string, integer or double values, values should
be specified.

Verbosity option (+v) can be used to get more information about all
options that have been set in the parameter file or on the command line:

./exe +v

Values set on the command line will override those from the parameter file
which will in turn override the hard-coded values in data.cpp file.

To display the list of all options available, use option (-help):

./exe -help

Example of a valid command line:

./exe +F newParamFile.txt -stream 1234 -seed 4567 -nbCom 10

In this example, the final value for parameters 'stream', 'seed' and 'nbCom'
will be retrieved from command line, and all other parameters appearing
in 'newParamFile.txt' will have their value taken from there. Parameters
that appear neither in the command line nor in the parameter file will be
set to the default values defined in data.cpp.

Setting a subset or all of options values, even more than once, is possible.
In this latter case, the last value assigned will be retained. For example,
from this command line:

./exe +F newParamFile.txt -stream 1234 -seed 4567 -nbCom 10 -seed 1212

the value retained for option parameter seed will be 1212 (not 4567).

READING and WRITING the basic graph from and to a file

Calling the topology option with value 3 instructs the program to read
the basic graph (nodes + arcs without costs and capacities) from a plain
text file. The default name of this file is basicGraph.dat. An alternative
name may be specified with the option basicGraphFileName. To be read
correctly, this file must be structured as follows:

(beginning of file)<br/>
Two blank or comment rows (are disregarded when reading)<br/>
low = lowNodeNumber #lowNodeNumber is smallest node number appearing in rows below<br/>
high = highNodeNumber #highNodeNumber is highest node number appearing in rows below<br/>
Three blank or comment rows (are disregarded when reading)<br/>
<origNodeNumber> <destNodeNumber> # each row indicates arc in graph from orig. to dest.<br/>
<origNodeNumber> <destNodeNumber><br/>
<origNodeNumber> <destNodeNumber><br/>
.<br/>
.<br/>
.<br/>
(end of file)

If option topology is NOT called with value 3, then the program will save the
basic graph in a plain text file whose default name, basicGraph.dat, may
be superseded using the option basicGraphFileName. In this case, the file
will have the following appearance:

(beginning of file)<br/>
1 Nodes<br/>
Nodes are identified with integers in closed interval [low, high].  Program will perform appropriate renumbering.<br/>
low = 0<br/>
high = 11

2 Arcs<br/>
NodeFrom  NodeTo<br/>
0  1<br/>
0  2<br/>
0  3<br/>
0  9<br/>
.<br/>
.<br/>
.<br/>
(end of file)

#### 3 Usage of options

##### 3.1 Options available on command line only

+F <string>; name of file supplying option parameter values (any item in
this file supersedes item in file with name hard-coded in main.cpp; items
in both files superseded by items specified on command line)

+v <> (no value supplied); requests verbosity

##### 3.2 Options available on command line and in configuration file

(Omit dash prefix (-) in configuration file.)

-h <> (no value supplied); display help message

-outFileName <string>; name of output file; suffix will differ according to
format: DOW, STD, LP, MPS

DOW and STD formats are specific to MCFND problems and described in files
DOW-format-desc.txt and STD-format-desc.txt; DOW presupposes that for each
arc, capacities and variable costs are identical for all commodities;
LP supplies a generic human readable statement

-mpsOut <> (no value supplied); save output with MPS format

-lpOut <> (no value supplied); save output with human readable LP format

-noStdOut <> (no value supplied); do not save output with STD format

-stream <int: 0, INT_MAX>; random stream

-seed <int: 0, INT_MAX>; random seed

-topology <int: 0, 1, 2, 3>; graph topology: 0 random, 1 grid, 2 circular,
3 read basic graph (nodes + arcs without costs and capacities) from file

-basicGraphFileName <string>; name of file where basic graph will be read if
topology==3 and will be written if topology!=3

-lengthX <int: 1, INT_MAX>; length along X if grid topology

-lengthY <int: 1, INT_MAX>; length along Y if grid topology

-numbNodes <int: 1, INT_MAX>; number of nodes if random or circular topology

-oneSrcOneSnk <> (no value supplied); impose one source and one sink for
each commodity; impose also that for each arc, capacities and variable
costs are identical for all commodities; will then output at least with
DOW format)

-eqSrcsEqSnks <> (no value supplied); impose identical sources over all
commodities and identical sinks over all commodities

-intCaps <> (no value supplied); impose integer total arc capacities

-intCommCaps <> (no value supplied); impose integer commodity specific arc
capacities

-noComCaps <> (no value supplied); do not impose commodity specific arc
capacities

-nbCom <int: 0, INT_MAX>; number of commodities

-nbAddArc <int: 0, INT_MAX>; number of additional random arcs

-minNbSrc <int: 1, INT_MAX>; min number of sources

-maxNbSrc <int: 1, INT_MAX>; max number of sources

-minNbSnk <int: 1, INT_MAX>; min number of sinks

-maxNbSnk <int: 1, INT_MAX>; max number of sinks

-minComVol <int: 1, INT_MAX>; min commodity volume

-maxComVol <int: 1, INT_MAX>; max commodity volume

-minVarCos <int: 0, INT_MAX>; min variable cost

-maxVarCos <int: 0, INT_MAX>; max variable cost

-minArcCap <int: 1, INT_MAX>; min arc total capacity

-maxArcCap <int: 1, INT_MAX>; max arc total capacity

-minFixCos <int: 0, INT_MAX>; min fixed cost

-maxFixCos <int: 0, INT_MAX>; max fixed cost

-minComCap <int: 0, INT_MAX>; min commodity-specific arc capacity

-maxComCap <int: 0, INT_MAX>; max commodity-specific arc capacity

-ratZeroFix <int: 0, 100>; percent random arcs with fixed\ncost set to
zero

-ratTotVolCap <int: 0, 100>; percent random arcs with total capacity set
to total volume

-ratZeroComCap <int: 0, 100>; percent random arcs with a commodity
specific capacity set to zero

-ratMaxComCap <int: 0, 100>; percent random arcs with a commodity specific
capacity set to maxComCap

-noParalArc <> (no value supplied); avoid parallel arcs

-maxIterNoParallelArc <int: 0, INT_MAX>; when adding random arcs, number of
failed attempts to add a non-parallel arc until number of random arcs to be
added is decreased by one

-flagAdjCapacsDown <> (no value supplied); require proportional uniform
downward adjustment of arc capacities according to adjCapacDown

-adjCapacDown <int: 0, INT_MAX>; percent uniform adjustment of arc capacities

-flagAdjFixedCostsUp <> (no value supplied); require proportional uniform
upward adjustment of fixed costs according to adjFixedCosUp

-adjFixCosUp <int: 0, INT_MAX>; percent uniform upward adjustment of fixed
costs
