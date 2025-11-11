Copyright (c) 2025 Eric Larsen

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

### GENERATOR OF INSTANCES OF THE LINEAR, DETERMINISTIC,<br>MULTI-COMMODITY, FIXED CHARGE, CAPACITATED<br>NETWORK DESIGN PROBLEM<br>(Nov 10th 2025)
This application randomly generates instances of the linear deterministic
multi-commodity, capacitated, fixed charge network design (MCFND) problem.
Details of the process are governed by user-specified configuration
settings.

For detailed background, see
Larsen, E., Bisaillon, S., Cordeau, J.F. and Frejinger, E.
Pseudo-random Instance Generators in C++ for Deterministic and Stochastic
Multi-commodity Network Design Problems.

#### 1 Pseudo-random generation and available topologies

All pseudo-random sampling operations occurring in the program are performed
with the permuted congruential generator (PCG). (See O'Neill, M. PCG, a
family of better random number generators.
https:www.pcg-random.org/, 2023.) All generated numerical characteristics
are integer-valued: commodity volumes, fixed costs, overall capacities,
commodity-specific variable costs, commodity-specific capacities.

Commodity volumes are pseudo-randomly generated independently and uniformly
between minComVol and maxComVol.

The topology of the graph can either be based on a grid (the latter can be
laid either on a torus, a cylinder or a flat bounded surface), on a circle,
be explicited in a file, or be entirely random generated. Under the toral
topology, non-random arcs are initially created to build the structure of
a torus with major and minor circumferences respectively equal to lengthX
and lengthY. Under the cylindrical topology, non-random arcs are initially
created to build the structure of a cylinder with circumference equal to
lengthX and height equal to lengthY. Under the flat topology, non-random
arcs are initially created to build the structure of a bounded flat surface
of width and height respectively equal to lengthX and lengthY. Under the
circular topology, non-random arcs are initially created in order to chain
the nodes. A basic graph (including nodes and arcs but excluding costs and
capacities) can also be read from a file. Likewise, non-random arcs are
initially created. Each one of the aforementioned non-random arcs has
overall capacity equal to total volume and fixed cost equal to maxFixedCos.
Over the non-random arcs, commodity-specific capacities are equal to
commodity volumes and commodity-specific variable costs are equal to
maxVarCos.

Addition of random arcs to the aforementioned structures can be performed
in either one or both of the two following ways: (i) A specified number of
additional arcs whose head and tail are generated pseudo-randomly. (ii) A
specified number of additional arcs starting from each existing node are
created whereas only their heads are generated pseudo-randomly. Each one of
the arcs in (i) and (ii) has fixed cost pseudo-randomly generated
independently and uniformly between minFixCos and maxFixCos and overall
capacity pseudo-randomly generated independently and uniformly between
minArcCap and maxArcCap. The commodity-specific variable costs and
capacities are pseudo-randomly generated independently and uniformly between
minVarCos and maxVarCos and between minComCap and maxComCap, respectively.

#### 2 Building and running the program directly from the Linux command line

##### 2.1 Building the program from the Linux command line

Follow these steps to build the program:

a- From https://github.com/larseeri/determ-MCFNDP-gener, retrieve, copy to an
appropriate folder and uncompress the archive file named 'determ_gen.zip'
containing the program files.
b- In a Linux terminal, go to the program folder just created.
c- Make sure that the gcc C++ compiler and the make C++ build tool are
available.
d- Run this instruction to build the program. An executable file named
'determ_gen' will be created inside the program folder.

make

Building with versions 11.5.0, 12.2.0, 13.2.0, 14.2.0 and 15.1.0 of the gcc
compiler has been verified.

##### 2.2 Running the program with Linux command line instructions

The program can be run as follows from the program folder just created
along with default option parameter values:

./determ_gen

By default, the program will read option values from file determ_config.par.
(This default setting can be updated by modifying line 727 of file
main.cpp.) The default file name can be overridden with option +F:

./determ_gen +F newParamfile.par

This assumes that determ_config.par and newParamfile.par are located at the
root of the program folder. Otherwise, their absolute paths or their paths
relative to the program folder must be explicited.

An option parameters file should have this format:

optionKey1   <value>
optionKey2   <value>
.
.
optionKeyN   <value>

For options expecting boolean values, no values need to be supplied. By
default, boolean values are set to false and presence of option key is
sufficient to set the corresponding value to true. For other options
expecting string, integer or double values, values should be specified.

Verbosity option +v should be used to obtain more information about all
options that have been set in the parameter file or in the command line:

./determ_gen +v

Parameter values are successively updated from these locations in this
order:
(i) hard-coded initialization values in lines 28-41 of data.cpp
(ii) values appearing in configuration file whose name is specified in
command line with +F (if it is specified and if it exists); if no valid
configuration file is specified with +F in command line, values will be
taken from configuration file whose name is specified in line 727 of
main.cpp; at this point, the latter should exist, although it may be empty
(iii) values specified in the command line
Location (i) is insufficiently transparent to the user and should not be
relied upon. Parameter values should be set through (ii) and (iii).

To display the list of all options available, use option -help:

./determ_gen -help

Example of a valid command line:

./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10

In this example, the final value for parameters stream, seed and
numbCom will be retrieved from command line, and all other parameters
appearing in file newParamfile.par will have their value taken from there.
Parameters that appear neither in command line, nor in this parameter file
will be set to the initialization values hard-coded in lines 28-41 of
data.cpp.

Setting a parameter value more than once in a location will not cause an
error. In this case, the last value assigned will be retained. For example,
from the command line

./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10 -seed 1212

the value retained for option parameter seed will be 1212 (not 4567).

Remark:
See Section 5 below for examples of configuration files and instructions
when the program is run directly from the Linux command line.

###### 2.2.1 Reading and writing the basic graph from and to a file

Setting topology option to 5 instructs the program to read the basic graph
(nodes + arcs without costs and capacities) from a plain text file. The
default name of this file is basic_graph.gra. An alternative long name
(including extension) may be specified with the option
longBasicGraphFileName. To be read correctly, this file must be structured
as follows:

(beginning of file)
Two blank or comment rows (these are disregarded when reading)
low = lowNodeNumber (lowNodeNumber is smallest node number appearing in rows below)
high = highNodeNumber (highNodeNumber is highest node number appearing in rows below)
Three blank or comment rows (these are disregarded when reading)
origNodeNumber destNodeNumber (a row identifies an arc from origin to destination)
origNodeNumber destNodeNumber
origNodeNumber destNodeNumber
.
.
.
(end of file)

When topology is set between 1 and 4, the program will save the basic graph
(i.e., including nodes and arcs, but excluding random arcs, costs and
capacities) in a plain text file whose default name, basicGraph.gra, may be
replaced using the parameter longBasicGraphFileName. The name specified
using the latter should include the extension. The content of the file will
appear like this:

(beginning of file)
1 Nodes
Nodes are identified with integers in closed interval [low, high].  Program will perform appropriate renumbering.
low = 0
high = 11

2 Arcs
NodeFrom  NodeTo
0  1
0  2
0  3
0  9
.
.
.
(end of file)

Remark:
See Section 5 below for examples of basic graph files.

#### 3 Running the program through Docker instructions

An advantage of running the executable determ_gen inside a Docker container
is the possibility to avoid selection and installation of suitable C++
compiler, C++ make build tool and required C++ libraries. It suffices that
the Docker engine be installed on the host where the computation will be
run. For this, see https:docs.docker.com/engine/install/.

Once the Docker engine is installed, follow these steps:

a- Open a terminal and retrieve from the GitHub container registry the
Docker image ghcr.io/larseeri/determ_gen_image:latest as follows:

docker pull ghcr.io/larseeri/determ_gen_image:latest

(From this image, Docker containers running the executable 'determ_gen' will
be generated.)
b- For simplicity, define the alias 'determ_gen_image' through

docker tag ghcr.io/larseeri/determ_gen_image:latest determ_gen_image

Remark:
If the Docker image determ_gen_image were instead available as the tar ball
determ_gen_image.tar, then it could be loaded through

docker load --input determ_gen_image.tar

c- From https://github.com/larseeri/determ-MCFNDP-gener, copy to an
appropriate folder and uncompress the archive file named
'determ_gen_docker.zip'. The latter contains the program folder named 
'determ_gen_docker' where are stored (i) the primitive files named
'Dockerfile' and 'makefile' based on which the Docker image was originally
generated, (ii) an empty subfolder /inout used for exchanging files between
the program running inside a Docker container and the directory structure of
the host, (iii) some auxiliary files, and (iv) the subfolder /examples 
containing a number of configuration files and their resulting outputs 
(examined in Section 5 below).
d- In a terminal, go to the program folder just created.

Then, instructions similar to those appearing in Section 2.2 can be run
from the terminal by adding this prefix:

docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image

and by replacing the executable location ./determ_gen with /determ_gen (i.e.
removing the initial dot), since the executable determ_gen is located at the
root of the Docker container being generated and since locations inside a
Docker container must be accessed through absolute paths.

Hence, the following command line instructions:
./determ_gen
./determ_gen +F newParamfile.par
./determ_gen +v
./determ_gen -help
./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10

respectively correspond to the following Docker instructions:
docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image /determ_gen
docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image /determ_gen +F /inout/newParamfile.par
docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image /determ_gen +v
docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image /determ_gen -help
docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image /determ_gen +F /inout/newParamfile.par -stream 1234 -seed 4567 -numbCom 10

The following characteristics are specific to the operation of the program 
through Docker instructions:
- An empty folder absolutePathToProgramFolder/inout must already exist on
the host for the Docker instructions to work correctly.
- Option --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout
connects subfolder /inout of the program folder on the host to folder /inout
of the Docker container. Hence, the connection between 
absolutePathToProgramFolder/inout on the host and subfolder /inout in the
container is used to supply input files to and retrieve output files from
the container.
- The default configuration file determ_config.par specified on line 727 of
main.cpp has been left empty inside the container. Hence, option parameter 
values are successively updated from these locations in this order: (i)
initialization values hard-coded in lines 28-41 of data.cpp, (ii) values 
appearing in configuration file whose name is specified in the Docker
instruction with +F (if it is specified and if it exists), (iii) values
specified in the Docker instruction.

Remarks: 
- Option --rm ensures suppression of the container implicitly generated by
docker run. Note that the container created by docker run for running the
executable determ_gen stops running immediately after the latter finishes 
and that it cannot be reused afterwards, for instance through a docker exec
instruction. Immediate suppression with --rm avoids uselessly cluttering 
Docker's container repository.
- The following Docker instructions may be useful:
docker image ls (list all currently existing docker images)
docker image rm --force imageName (forcefully remove imageName)
docker container ls (list all running containers) 
docker container ls --all (list all containers, running or not)
- See Section 5 below for examples of configuration files and instructions
when the program is run through Docker.

#### 4 Usage of options

Notation:
For the purposes of the current discussion, <...> indicates that some value
or values of the type specified inside the brackets is expected. For
instance, <string> means that a string is expected whereas <> means that no
value is expected.

##### 4.1 Options available in instruction line only (Linux or Docker)

+F <string>; name of configuration file supplying option parameter values; 
this replaces the default file whose name is hard-coded in line 727 of 
main.cpp; any option value supplied in instruction line replaces value of
same option supplied in configuration file

+v <> (no value supplied); requests verbosity while parsing options in
instruction line or configuration file

Remark:
An unknown option key following '+' in the instruction line is disregarded.

##### 4.2 Options available in instruction line (Linux or Docker) and in configuration file

Remarks:
- When the following options are called from the instruction line, dash
prefixes '-' must be present. The latter can be omitted when the options are
called from a configuration file.
- To supply an option, a line of a configuration file must begin with a dash
'-' or a letter. A line beginning with any other character will be
considered a comment.
- The remainder of a line beyond the stated option and its value (if any is
expected) is disregarded and may therefore be used for adding comments.
- Unknown option keys following '-' in the instruction line are disregarded.
- Unknown option keys (following '-' or not) at beginning of lines in a
configuration file are disregarded.

-help <> (no value supplied); display help message

-shortOutFileName <string>; short name (without extension) of output file;
default value is './outfile' (Linux instructions) or './inout/outfile'
(Docker instructions); extension will differ according to format:
dow, std (a.k.a. Canad format), lp, mps, tex; when using Linux instructions, 
absolute path or path relative to the program folder may be added

Remark:
DOW, STD and TEX formats describe the generated network; DOW and STD
(a.k.a. Canad format) are text formats specified in files
DOW-format-desc.txt and STD-format-desc.txt; DOW imposes that (i) for each
arc, capacities and variable costs are identical for all commodities and
that (ii) each commodity has a single source node (at which volume > 0) and
a single sink node (at which volume < 0); TEX format is a graphical
representation that can be processed by a LaTeX text and graphics compiler;
LP (human readable) and MPS formats describe the MCFND linear programming
problem resulting from the generated network

-mpsOut <> (no value supplied); save output with MPS format

-lpOut <> (no value supplied); save output with human readable LP format

-texOut <> (no value supplied); generate and save LaTeX code representing
network; such representations are unavailable (i) if lengthX > 10 or
lengthY > 10, when 1 <= topology <= 3, (ii) if numbNodes > 100, when
topology == 0 or topology >= 4, (iii) if numbCom > 15; other specific
conditions depending on values of oneSrcOneSnk, lengthX, lengthY,
numbNodes and numbCom are managed and reported by the program; ulterior
processing of the TEX file by a LaTeX compiler may require up to several
minutes depending on complexity.

-noStdOut <> (no value supplied); do not save output with STD format

-stream <int: 0, INT_MAX>; random stream

-seed <int: 0, INT_MAX>; random seed

-topology <int: 0, 1, 2, 3, 4, 5>; 0 calls for a graph topology that is
entirely random and where arcs are set according to nbAddArcSampTailHead
and/or nbAddArcPerSampHead; 1 to 5 call for a particular basic graph
topology (before additional, random arcs are eventually added according to
nbAddArcSampTailHead and/or nbAddArcPerSampHead): 1 grid on a torus, 2 grid
on a cylinder, 3 grid on a bounded surface, 4 circular, 5 read (nodes + arcs
without costs and capacities) from file

-longBasicGraphFileName <string>; long name (including extension) of file
where basic graph (excluding random arcs, capacities and costs ) will be
read if topology == 5 and will be written if 1 <= topology == 4; default
value is './basic_graph.gra' (Linux instructions) or 
'./inout/basic_graph.gra' (Docker instructions); when using Linux
instructions, absolute path or path relative to the program folder may be
added

-lengthX <int: 1, INT_MAX>; length along X axis under grid topology
(inactive otherwise)

-lengthY <int: 1, INT_MAX>; length along Y axis under grid topology
(inactive otherwise)

-numbNodes <int: 1, INT_MAX>; number of nodes under random or circular
topology; (overriden by lengthX * lengthY under grid topology; also
overriden when reading basic graph from file)

-oneSrcOneSnk <> (no value supplied); select one source and one sink for
each commodity; (source, sink) pairs must be different across all
commodities; remove differentiation between variable costs across individual
commodities, prior to saving in any format DOW, STD, LP, MPS or TEX; reduce
as follows differentiation between capacities across individual commodities,
prior to saving in any format DOW, LP, STD, MPS or TEX: for all k and a,
capacity for commodity k over arc a is equated to minimum of volume of
commodity k and overall capacity of arc a; will be saved at least using DOW
format

-eqSrcsEqSnks <> (no value supplied); impose identical sources over all
commodities and identical sinks over all commodities

Remark:
oneSrcOneSnk and eqSrcsEqSnks cannot be both active; if oneSrcOneSnk and
eqSrcsEqSnks are both inactive, then sources and sinks are selected
randomly

-inclDesignObjCap <> (no value supplied); when saving in LP or MPS format,
ensure that (i) design variables will appear with fixed costs in objective,
(ii) design variables will appear on the RHS of arc-capacity constraints

-inclDesignObjCommCap <> (no value supplied); when saving in LP or MPS
format, ensure that (i) design variables will appear with fixed costs in
objective, (ii) when there is a single commodity, design variables will
appear on the RHS of arc-capacity constraints, (iii) when there are multiple
commodities, commodity-specific capacity constraints (a.k.a. strong forcing
constraints) will be included and design variables will appear on their
RHS

Remark:
In constrast with DOW, STD and TEX formats, LP and MPS formats describe the
mixed integer linear program (MILP) solving the generated instance of MCFND
problem; hence, inclDesignObjCap and inclDesignObjCommCap determine the
particular forms taken by the LP and MPS representations (see
Graph::outputLP() and Graph::outputMPS() in graph.cpp)

-noComCaps <> (no value supplied); reduce as follows differentiation
between capacities across individual commodities, prior to saving in
any format DOW, LP, STD, MPS or TEX: for all k and a, capacity for
commodity k over arc a is equated to minimum of volume of commodity k
and overall capacity of arc a

-numbCom <int: 0, INT_MAX>; number of commodities

-nbAddArcSampTailHead <int: 0, INT_MAX>; total number of additional random
arcs generated by sampling over tail (origin) and head (destination) nodes;
if this option is inactive, default value is 0

-nbAddArcPerSampHead <int: 0, INT_MAX>; number of additional random arcs
per tail (origin) node generated by sampling over head (destination) nodes;
if this option is inactive, default value is 0

-minNbSrc <int: 1, INT_MAX>; min number of sources; inactive under
oneSrcOneSnk

-maxNbSrc <int: 1, INT_MAX>; max number of sources; inactive under
oneSrcOneSnk

-minNbSnk <int: 1, INT_MAX>; min number of sinks; inactive under
oneSrcOneSnk

-maxNbSnk <int: 1, INT_MAX>; max number of sinks; inactive under
oneSrcOneSnk

-minComVol <int: 1, INT_MAX>; min commodity volume

-maxComVol <int: 1, INT_MAX>; max commodity volume

-minVarCos <int: 0, INT_MAX>; min commodity-specific variable cost

-maxVarCos <int: 0, INT_MAX>; max commodity-specific variable cost

-minArcCap <int: 1, INT_MAX>; min arc overall capacity

-maxArcCap <int: 1, INT_MAX>; max arc overall capacity

-minFixCos <int: 0, INT_MAX>; min fixed cost

-maxFixCos <int: 0, INT_MAX>; max fixed cost

-minComCap <int: 0, INT_MAX>; min commodity-specific arc capacity

-maxComCap <int: 0, INT_MAX>; max commodity-specific arc capacity

-ratZeroFix <int: 0, 100>; percent of random arcs with fixed cost
set to zero; if this option is inactive, default percentage is 0

-ratTotVolCap <int: 0, 100>; percent of random arcs with total
capacity set to total volume; if this option is inactive, default
percentage is 0

-ratZeroComCap <int: 0, 100>; percent of random arcs with one of the
commodity-specific capacities set to zero; if this option is inactive,
default percentage is 0

-ratMaxComCap <int: 0, 100>; percent of random arcs with one of the
commodity-specific capacities set to maxComCap; if this option is inactive,
default percentage is 0

-noParalArc <int: 1, INT_MAX>; when adding random arcs, avoid parallel arcs
and set to this figure the maximum number of failed attempts to add a non-
parallel arc until number of random arcs to be added is decreased by one;
if this option is inactive, no attempt will be made to avoid parallel arcs

-adjCapacsDown <int: 1, INT_MAX>; require uniform downward adjustment of
overall capacities of all arcs (non-random and random) whose magnitude is
proportional to the specified figure (see Graph::adjustArcCapacities() in
graph.cpp); if this option is inactive, no adjustment will be made

Remark:
Setting adjCapacsDown to 1 is not equivalent to no adjustment

-adjFixCostsUp <int: 1, INT_MAX>; require uniform upward adjustment of
fixed costs of all arcs (non-random and random) whose magnitude is
proportional to the specified figure (see Graph::adjustFixedCosts() in
graph.cpp); if this option is inactive, no adjustment will be made

Remark:
Setting adjFixCostsUp to 1 is not equivalent to no adjustment

##### 4.3 Joint restrictions

if oneSrcOneSnk
{
     if 1 <= topology <= 3
         (lengthX * lengthY) * (lengthX * lengthY - 1) >= numbCom
     else
         numbNodes * (numbNodes - 1) >= numbCom
}
else
{
     minNbSrc <= maxNbSrc
     minNbSnk <= maxNbSnk

     if 1 <= topology <= 3
     {
         maxNbSrc + maxNbSnk <= lengthX * lengthY
     }
     else
     {
         maxNbSrc + maxNbSnk <= numbNodes
     }
}

minComVol <= maxComVol
minVarCos <= maxVarCos
minFixCos <= maxFixCos
minArcCap <= maxArcCap
minComCap <= maxComCap

maxComCap <= minComVol
maxComCap <= minArcCap
maxArcCap <= numbCom * minComCap

if 1 <= topology <= 3
     lengthX * lengthY > 1
else
     numbNodes > 1

#### 5 Examples

The archive determ_gen.zip contains the folder determ_gen and the subfolder
/examples. The latter holds examples of configuration files that are 
appropriate when the executable of the program is run directly from the 
Linux command line. It also contains the output files that are generated
from each configuration file.

Similarly, the archive determ_gen_docker.zip contains the folder
determ_gen_docker and the subfolder /examples. The latter holds examples
of configuration files that are appropriate when the executable of the
program is run through Docker instructions. These are identified by the
addition of the prefix 'docker_'. It also contains the output files that are
generated from each configuration file. The latter are identical to the
files that are generated when the executable is run from the Linux command
line.

The output examples contained in the /example subfolders will also be used
to illustrate the operation of the distinct application generating 
stochastic MCFND networks.

In all included examples, the generated MCFND network is characterized by a
(3 x 4) grid topology applied to a bounded surface and by 3 commodities.

##### 5.1 One source and one sink for each commodity

Folder ./examples/oneSrcOneSnk/

In this case, there is one source and one sink for each commodity and no
differentiation of variable costs and capacities according to commodities.
The generated networks can be saved in the DOW and STD formats.

###### 5.1.1 Instructions

Linux command line instruction:
./determ_gen +v +F determ_config_oneSrcOneSnk_10oct2025-18h10.par

Docker instruction:
docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image /determ_gen +v \
+F /inout/docker_determ_config_oneSrcOneSnk_10oct2025-18h10.par

###### 5.1.2 Configuration files (input)

configuration file used with Linux command line:
determ_config_oneSrcOneSnk_10oct2025-18h10.par

configuration file used with Docker instruction:
docker_determ_config_oneSrcOneSnk_10oct2025-18h10.par

###### 5.1.3 Generated files

numerical representation of the network in DOW format:
determ_oneSrcOneSnk_10oct2025-18h10.dow

numerical representation of the network in STD format:
determ_oneSrcOneSnk_10oct2025-18h10.std

numerical representation of the basic graph:
determ_oneSrcOneSnk_10oct2025-18h10.gra

human-readable representation of the associated linear programming problem
in LP format:
determ_oneSrcOneSnk_10oct2025-18h10.lp

numerical representation of the associated linear programming problem in
MPS format:
determ_oneSrcOneSnk_10oct2025-18h10.mps

graphical representation of the network in TEX format:
determ_oneSrcOneSnk_10oct2025-18h10.tex

compiled graphical representation:
determ_oneSrcOneSnk_10oct2025-18h10.pdf

##### 5.2 Multiple but equal sources and multiple but equal sinks

Folder ./examples/eqSrcsEqSnks/

In this case, there are multiple sources and multiple sinks for each
commodity. However, sources and sinks are identical across commodities.
There is differentiation of variable costs and capacities according to
commodities. The generated networks cannot be saved in the DOW format.

###### 5.2.1 Instructions

Linux command line instruction:
./determ_gen +v +F determ_config_eqSrcsEqsnks_10oct2025-18h10.par

Docker instruction:
docker run --rm --mount type=bind,src=absolutePathToProgramFolder/inout,dst=/inout determ_gen_image /determ_gen +v \
+F /inout/docker_determ_config_eqSrcsEqsnks_10oct2025-18h10.par

###### 5.2.2 Configuration files (input)

configuration file used with Linux command line:
determ_config_eqSrcsEqsnks_10oct2025-18h10.par

configuration file used with Docker instruction:
docker_determ_config_eqSrcsEqsnks_10oct2025-18h10.par

###### 5.2.3 Generated files

numerical representation of the network in STD format:
determ_eqSrcsEqsnks_10oct2025-18h10.std

numerical representation of the basic graph:
determ_eqSrcsEqsnks_10oct2025-18h10.gra

human-readable representation of the associated linear programming problem
in LP format:
determ_eqSrcsEqsnks_10oct2025-18h10.lp

numerical representation of the associated linear programming problem in
MPS format:
determ_eqSrcsEqsnks_10oct2025-18h10.mps

graphical representation of the network in TEX format:
determ_eqSrcsEqsnks_10oct2025-18h10.tex

compiled graphical representation:
determ_eqSrcsEqsnks_10oct2025-18h10.pdf

