// Copyright (c) 2026 Eric Larsen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ****************************************************************************
// ****************************************************************************
//
// GENERATOR OF INSTANCES OF THE LINEAR, DETERMINISTIC, MULTI-COMMODITY, 
// CAPACITATED, FIXED CHARGE NETWORK DESIGN PROBLEM
//
// USER GUIDE
//
// This application generates instances of the linear, deterministic, multi-
// commodity, capacitated, fixed charge network design problem (MCFNDP). The
// generation process is based on pseudo-random determinations governed by user-
// specified configuration settings.
//
// For detailed background, see
// Larsen, E., Bisaillon, S., Cordeau, J.F. and Frejinger, E.
// Pseudo-random Instance Generators in C++ for Deterministic and Stochastic
// Multi-commodity Network Design Problems.
//
// This user guide comprises the following sections:
// 1 Pseudo-random generation and available topologies
// 2 Building and running the program directly from the Linux command line
// 3 Running the program through Docker instructions
// 4 Usage of options
// 5 Detailed examples
//
// 1 Pseudo-random generation and available topologies
// ***************************************************
//
// All pseudo-random sampling operations occurring in the program are performed
// with the permuted congruential generator (PCG). (See O'Neill, M. PCG, a
// family of better random number generators.
// https://www.pcg-random.org/, 2023.) All generated numerical characteristics
// are integer-valued: commodity volumes, fixed costs, overall capacities,
// commodity-specific variable costs, commodity-specific capacities.
//
// Two primary topologies are available: point-to-point and 
// hubs-and-satellites. Notice that the hub-and-spoke topology is a special
// case of the latter.
// - Under the point-to-point primary topology, any node can be a source or a
// sink of commodity volume. The secondary topology option specifies basic
// connections between nodes. Additional arcs can be created randomly as
// specified.
// - Under the hubs-and-satellites primary topology (option hubsAndSats),
// there are two classes of nodes: hub nodes and satellite nodes. Only 
// satellite nodes can be sources or sinks of commodity volumes. Two classes
// of arcs must also be distinguished: (i) hub arcs connecting hub nodes and
// (ii) satellite arcs connecting two satellite nodes or a satellite node and
// a hub node. In this context, the secondary topology option specifies basic
// connections between hub nodes. Sets of satellite nodes, called rounds, are
// assigned to each hub node. The satellite nodes in a round are connected
// sequentially with satellite arcs. The cardinality of each round is 
// determined randomly according to specified option values. The end satellite
// nodes in the round(s) are connected to the hub node through satellite arcs.
// Additional hub arcs and satellite arcs can be created randomly if 
// specified.
//
// The secondary topology (option secTopology) concerns all nodes in the 
// point-to-point primary topology and concerns only hub nodes in the
// hubs-and-satellites primary topology. It can either (i) be grid-like (the
// grid can be laid either on a torus, a cylinder or a flat bounded surface), 
// (ii) be based on a circle, (iii) be specified in a file, or (iv) be
// entirely random generated. (ia) Under the toral secondary topology, non-
// random arcs are initially created to build the structure of a torus with
// major and minor circumferences respectively equal to values of options 
// lengthX and lengthY. (ib) Under the cylindrical secondary topology, non-
// random arcs are initially created to build the structure of a cylinder with
// circumference and height respectively equal to values of options lengthX and
// lengthY. (ic) Under the flat secondary topology, non-random arcs are
// initially created to build the structure of a bounded flat surface of width
// and height respectively equal to values of options lengthX and lengthY. (ii)
// Under the circular secondary topology, non-random arcs are initially created
// in order to chain the nodes. (iii) A basic graph (including nodes and arcs
// but excluding costs and capacities) can be read from a file. Each non-random
// arc initially created in connection with (i), (ii) and (iii) has overall 
// capacity equal to total volume over all commodities and fixed cost equal to
// the value specified as the maximum fixed cost (value of option maxFixedCos).
// Over these non-random arcs, overall capacities are equal to total volume 
// over all commodities, each commodity-specific capacity is equal to commodity
// volume and commodity-specific variable costs are equal to the value 
// specified as the maximum commodity-specific variable cost (value of option 
// maxVarCos).
//
// Addition of random arcs to the aforementioned structures can be performed
// in one or both of the two following ways: (i) A specified total number of
// additional arcs whose head and tail are generated pseudo-randomly. (ii) A
// specified number of additional arcs starting from each existing node are
// created whereas only their heads are generated pseudo-randomly. Each one of
// the arcs in (i) and (ii) has fixed cost pseudo-randomly generated
// independently and uniformly within the interval specified by the values of
// options minFixCos and maxFixCos and overall capacity pseudo-randomly
// generated independently and uniformly within the interval specified by
// the values of options minArcCap and maxArcCap. The commodity-specific 
// variable costs and capacities are pseudo-randomly generated independently
// and uniformly between the values of options minVarCos and maxVarCos and
// the values of options minComCap and maxComCap, respectively.
//
// Section 4 below details the usage of every option available under the
// point-to-point and hubs-and-satellites primary topologies.
//
// 2 Building and running the program directly from the Linux command line
// ***********************************************************************
//
// 2.1 Building the program from the Linux command line
// ****************************************************
//
// Follow these steps to build the program:
//
// a- Either (i) use Git to clone locally the remote repository
// https://github.com/larseeri/determ-MCFNDP-gener, or (ii) generate, download
// locally to an appropriate folder and uncompress a .zip archive containing
// the remote repository file tree.
// b- In a Linux terminal, go to the /program subfolder just created.
// c- Make sure that the gcc C++ compiler and the make C++ build tool are
// available.
// d- Run this instruction to build the program. An executable file named
// 'determ_gen' will be created inside the /program folder.
//
// make
//
// Building with versions 11.5.0, 12.2.0, 13.2.0, 14.2.0 and 15.1.0 of the gcc
// compiler has been verified.
//
// 2.2 Running the program with Linux command line instructions
// ************************************************************
//
// The program can be run as follows from the /program folder just created
// along with default option parameter values:
//
// ./determ_gen
//
// Options for running the program can be specified directly in the command
// line or in a configuration file. By default, the program will read option
// values from file determ_config.par. (This default setting can be updated by
// modifying line 1210 of file main.cpp.) The default file can be 
// overridden with option +F:
//
// ./determ_gen +F newParamfile.par
//
// This assumes that determ_config.par and newParamfile.par are located at the
// root of the /program folder. Otherwise, their absolute paths or their paths
// relative to the /program folder must be specified.
//
// The default configuration file determ_config.par has initially been filled
// with illustrative values and should be edited appropriately if no 
// replacement configuration file is indicated on the command line with option
// key +F.
//
// Verbosity option +v should be used to obtain more information about all
// options that have been set in the parameter file or in the command line:
//
// ./determ_gen +v +F newParamfile.par
//
// A configuration file should have this format:
//
// optionKey1	value1
// optionKey2   value2
// .
// .
// optionKeyN   value3
//
// It may specify at most one option per line. Option keys may be prefixed or
// not with '-'. Comments may appear either at the beginning or the end of a
// line, provided they start with '#'.
//
// Once the configuration file has been processed, options appearing in the
// command line (other than +F, +v and +H) are processed. Option values specified
// in the command line supersede those specified in the configuration file.
// That is, if included, they replace, modify and supplement the latter.
// Option keys appearing in the command line must be prefixed with '-' as
// follows:
//
// -optionkeyA valueA -optionKeyB valueB...
//
// By default, boolean values are set to false and presence of option key is
// sufficient to set the corresponding value to true. For other options
// expecting other types of values, values should be specified.
//
// Option values interpreted as strings (e.g., file names or paths) must NOT
// include blanks. Enclosing such values between quotes does NOT obviate this
// requirement.
//
// Example of a valid command line:
//
// ./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10
//
// In this example, the final value for parameters stream, seed and
// numbCom will be retrieved from command line, and all other parameters
// appearing in file newParamfile.par will have their value taken from there.
// Parameters that appear neither in command line, nor in this parameter file
// will be set to the initialization values hard-coded in lines 28-48 of
// data.cpp.
//
// Setting a parameter value more than once in a location will not cause an
// error. In this case, the last value assigned will be retained. For example,
// from the command line
//
// ./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10 -seed 1212
//
// the value retained for option parameter seed will be 1212 (not 4567).
//
// To display the complete documentation about the application and the
// available options, run
//
// ./determ_gen +H
//
// Summing up, option values are successively updated from these locations in
// this order:
// (i) hard-coded initialization values in lines 28-48 of data.cpp
// (ii) values appearing in configuration file whose name is specified in
// command line with +F (if it is specified and if it exists); if no valid
// configuration file is specified with +F in command line, values will be
// taken from default configuration file whose name is specified in line 1210
// of main.cpp; at this point, the latter should exist
// (iii) values specified in the command line
// Notice that location (i) is insufficiently transparent to the user and
// should not be relied upon. Parameter values should be set through (ii) and 
// (iii).
//
// Remark:
// See Section 5 below for examples of configuration files and instructions
// when the program is run from the Linux command line.
//
// 2.2.1 Reading and writing the basic graph from and to a file
// ************************************************************
//
// Setting secondary topology option to 5 instructs the program to read the
// basic graph (nodes + arcs without costs and capacities) from a plain text
// file. The default name of this file is basic_graph.gra. An alternative 
// long name (including extension) may be specified with the option
// longBasicGraphFileName. To be read correctly, this file must be structured
// as follows:
//
// (beginning of file)
// Two blank or comment rows (these are disregarded when reading)
// low = lowNodeNumber (lowNodeNumber is smallest node number appearing in rows below)
// high = highNodeNumber (highNodeNumber is highest node number appearing in rows below)
// Three blank or comment rows (these are disregarded when reading)
// origNodeNumberA destNodeNumberA (a row identifies an arc from origin to destination)
// origNodeNumberB destNodeNumberB
// origNodeNumberC destNodeNumberC
// .
// .
// .
// (end of file)
//
// When secondary topology is set between 1 and 4, the program will save the
// basic graph (i.e., including nodes and arcs, but excluding random arcs,
// costs and capacities) in a plain text file whose default name, 
// basic_graph.gra, may be replaced using the parameter longBasicGraphFileName.
// The name specified using the latter should include the extension. The 
// content of the file will appear like this:
//
// (beginning of file)
// 1 Nodes
// Nodes are identified with integers in closed interval [low, high].  Program will perform appropriate renumbering.
// low = 0
// high = 11
//
// 2 Arcs
// NodeFrom  NodeTo
// 0  1
// 0  2
// 0  3
// 0  9
// .
// .
// .
// (end of file)
//
// Remark:
// See Section 5 below for examples of basic graph files.
//
// 3 Running the program through Docker instructions
// *************************************************
//
// An advantage of running the executable determ_gen inside a Docker container
// is the possibility to avoid selection and installation of suitable C++
// compiler, C++ make build tool and required C++ libraries. It suffices that
// the Docker engine be installed on the host where the computation will be
// run. For this, see https://docs.docker.com/engine/install/.
//
// Once the Docker engine is installed, follow these steps:
//
// a- Open a terminal and retrieve from the GitHub container registry the
// Docker image ghcr.io/larseeri/determ_gen_image:latest as follows:
//
// docker pull ghcr.io/larseeri/determ_gen_image:latest
//
// (From this image, Docker containers running the executable 'determ_gen' will
// be generated.)
//
// b- For simplicity, define the alias 'determ_gen_image' through
//
// docker tag ghcr.io/larseeri/determ_gen_image:latest determ_gen_image
//
// Remark:
// If the Docker image determ_gen_image were instead available as the tar ball
// determ_gen_image.tar, then it could be loaded through
//
// docker load --input determ_gen_image.tar
//
// c- Either (i) use Git to clone locally the remote repository
// https://github.com/larseeri/determ-MCFNDP-gener, or (ii) generate, download
// locally to an appropriate folder and uncompress a .zip archive containing
// the remote repository file tree. The /docker folder contains (i) the
// primitive files named 'Dockerfile' and 'makefile' based on which the
// Docker image was originally generated, (ii) a subfolder /inout used
// for exchanging files between the program running inside a Docker container
// and the directory structure of the host, (iii) some auxiliary files, and
// (iv) the subfolder /examples containing a number of configuration files and
// their resulting outputs (examined in Section 5 below).
//
// d- In a terminal, go to the /docker folder just created.
//
// Then, instructions similar to those appearing in Section 2.2 can be run from
// the terminal on AMD and Intel CPUs (other CPUs are addressed further below).
// For example, the default Docker instruction
//
// docker run --init --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen
//
// is equivalent to the default Linux command line instruction
//
// ./determ_gen
//
// In general, Docker instructions can be formulated from those appearing in
// Section 2.2 by (i) adding the prefix 
//
// docker run --init --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image
//
// and by (ii) replacing the executable location ./determ_gen with /determ_gen
// (i.e. removing the initial dot), since the executable determ_gen is located
// at the root of the Docker container being generated and since locations 
// inside a Docker container must be accessed through absolute paths.
// 
// Hence, the following Linux command line instructions:
//
// ./determ_gen +v +F newParamfile.par
//
// ./determ_gen +H
//
// ./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10
//
// respectively correspond to the following Docker instructions:
//
// docker run --init --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +v +F /inout/newParamfile.par
//
// docker run --init --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +H
//
// docker run --init --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +F /inout/newParamfile.par -stream 1234 -seed 4567 -numbCom 10
//
// Important: 
// When running the program through Docker instructions, all input and output
// files must be located inside the docker/inout subfolder. Within the Docker 
// instructions and the configuration files, this subfolder must be denoted
// /inout since it is located at the root of the Docker container. Hence, a
// replacement configuration file located at docker/inout/newParamFile.par
// will be specified in the Docker instruction with +F /inout/newParamFile.par.
// Similarly, a short output file name ./shortOutputFileName will be specified
// in the Docker instruction or inside the configuration file with
// -shortOutFileName /inout/shortOutputFileName.
//
// The default configuration file is located at docker/inout/determ_config.par.
// It has initially been filled with illustrative values and should be edited
// appropriately if no replacement configuration file is indicated on the
// Docker instruction line with option key +F.
// 
// Option parameter values are successively updated from these locations in
// this order:
// (i) initialization values hard-coded in lines 28-48 of data.cpp,
// (ii) either replacement configuration file specified in the Docker
// instruction line with option key +F, or default configuration file 
// ./inout/determ_config.par if no replacement configuration file is 
// specified in the Docker instruction line with option key +F,
// (iii) values specified in the Docker instruction.
// Notice that location (i) is insufficiently transparent to the user and
// should not be relied upon. Parameter values should be set through (ii) and
// (iii).
//
// Important:
// The supplied Docker image conforms to the amd64 CPU instructions standard
// and will run natively on AMD and Intel CPUs. On CPUs conforming instead to
// the arm64 standard, e.g., on recent Apple computers, the supplied Docker 
// image can be run through emulation. Essentially, this may be achieved as
// follows:
//
// a- Running the following preliminary instruction once. (For details, see
// https://hub.docker.com/r/tonistiigi/binfmt.)
//
// docker run --init --privileged --rm tonistiigi/binfmt --install all 
//
// b- Adding the option --platform linux/amd64 to the Docker instruction line
// prefix as follows:
//
// docker run --init --platform linux/amd64 --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image
//
// The following characteristics are specific to the operation of the program
// through Docker instructions:
// - An folder absolutePathToDockerFolder/inout must already exist on
// the host for the Docker instructions to work correctly. This folder
// already contains the default configuration file.
// - Option --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout
// connects subfolder docker/inout on the host to folder /inout
// of the Docker container. Hence, the connection between
// absolutePathToDockerFolder/inout on the host and folder /inout in the
// container is used to supply input files to and retrieve output files from
// the container.
// - Option --rm ensures suppression of the container implicitly generated by
// docker run. Note that the container created by docker run for running the
// executable determ_gen stops running immediately after the latter finishes
// and that it cannot be reused afterwards, for instance through a docker exec
// instruction. Immediate suppression with --rm avoids uselessly cluttering
// Docker's container repository.
// - Option --init creates a small additional Docker process. This has the
// effect of permitting a more orderly exit of the main Docker process in case
// of error.
//
// The following Docker instructions may be useful:
//
// docker image ls (list all currently existing docker images)
//
// docker image rm --force imageName (forcefully remove imageName)
//
// docker container ls (list all running containers)
//
// docker container ls --all (list all containers, running or not)
//
// See Section 5 below for examples of configuration files and instructions
// when the program is run through Docker.
//
// 4 Detailed usage of options
// ***************************
//
// Notation:
// For the purposes of the current discussion, <...> indicates that some value
// or values of the type specified inside the brackets is expected. For
// instance, <string> means that a string is expected whereas <> means that no
// value is expected.
//
// Important:
// File paths specified in the instruction line or in a configuration file
// must not contain any blank space.
//
// 4.1 Options available in instruction line only (Linux or Docker)
// ****************************************************************
//
// +F <string>; name of configuration file supplying option parameter values;
// this replaces the default file whose name is hard-coded in line 1210 of
// main.cpp; any option value supplied in instruction line replaces value of
// same option supplied in configuration file
//
// +v <> (no value supplied); requests verbosity while parsing options in
// instruction line or configuration file
//
// +H <> (no value supplied); display complete documentation
//
// Remark:
// An unknown option key following '+' in the instruction line is disregarded.
//
// 4.2 Options available in instruction line (Linux or Docker) and in configuration file
// *************************************************************************************
//
// Remarks:
// - When the following options are called from the instruction line, dash
// prefixes '-' must be present. The latter can be omitted when the options are
// called from a configuration file.
// - To supply an option, a line of a configuration file must begin with a dash
// '-' or a letter. A line beginning with any other character will be
// disregarded.
// - Comments may appear either at the beginning or the end of a line, provided
// they start with '#'.
// - Unknown option keys following '-' in the instruction line are disregarded.
// - Unknown option keys (following '-' or not) at beginning of lines in a
// configuration file are disregarded.
// - After the instruction line has been parsed, [key] [value] pairs that have
// been left unconsumed are listed: (i) They may be currently inactive and
// harmlessly present in the instruction line. (ii) They might also MISSPELL
// active keys that were intended to be included, in which case the INTENDED
// KEYS WILL BE GIVEN ERRONEOUS DEFAULT VALUES.	
// - After the configuration file has been parsed, [key] [value] pairs that
// have been left unconsumed are listed: (i) They may be currently inactive and
// harmlessly present in the configuration file. (ii) They might have been 
// superseded by similar pairs from the instruction line. (iii) They might also
// MISSPELL active keys that were intended to be included, in which case the
// INTENDED KEYS WILL BE GIVEN ERRONEOUS DEFAULT VALUES.
//
// -hubsAndSats <> (no value supplied); implement hubs-and-satellites primary
// topology; if absent or explicitly set to false, the point-to-point primary
// topology is instead implemented
//
// 4.2.1 Options that are available under the point-to-point or hubs-and-satellites primary topologies
// ***************************************************************************************************
// (That is, when the option hubsAndSats is either (i) absent and implicitly set
// to false, or (ii) present without value and implicitly set to true or (iii) 
// present and explicitly set to true or false.)	
//
// -shortOutFileName <string>; short name (without extension) of output file;
// default value is './outfile' (Linux instructions) or './inout/outfile'
// (Docker instructions); extension will differ according to format:
// dow, std (a.k.a. Canad format), lp, mps, tex; when using Linux instructions,
// absolute path or path relative to the /program folder may be added
//
// Remark:
// DOW, STD and TEX formats describe the generated instance; DOW and STD
// (a.k.a. Canad format) are text formats whose canonical descriptions are supplied in files 
// DOW-format-desc.txt and STD-format-desc.txt; STD is a generic format valid
// for any MCFNDP, DOW requires that (i) for each arc, capacities and variable
// costs are identical for all commodities and that (ii) each commodity has a
// single source node (at which volume > 0) and a single sink node (at which 
// volume < 0); TEX format is a graphical representation that can be processed
// by a LaTeX text and graphics compiler; LP (human readable) and MPS formats
// state linear programming formulations of the generated MCFNDP instance.
//
// Important:
// The DOW and STD format file writers are more restrictive than allowed by the DOW and STD formats 
// canonical specifications in this way: zero-valued fixed costs aret be omitted.
//
// Remarks:
// - The output will automatically be saved in STD format unless option
// noStdOut is specified.
// - Under the point-to-point primary topology, the output will automatically
// be saved in DOW format if the options oneSrcOneSnk, equalComCaps, 
// equalVarCosts all hold.
// - Under the hubs-and-satellites primary topology, the output will 
// automatically be saved in DOW format if the options oneSrcOneSnk, 
// equalComCaps, equalVarCosts, equalSatComCaps, equalSatVarCosts all hold.
//
// -mpsOut <> (no value supplied); save output with MPS format
//
// -lpOut <> (no value supplied); save output with human readable LP format
//
// -texOut <> (no value supplied); generate and save LaTeX code for 
// representing the instance;
//
// Remarks:
// - Under the point-to-point primary topology, the LaTeX representations are
//  available
// (i) if lengthX <=10, lengthY <= 10 and numbCom <= 15  when 1 <= secTopology <= 3,
// (ii) if numbNodes <= 36 and numbCom <= 15 when secTopology == 4.
// - Subsequent processing of the TEX file by a LaTeX compiler may require up to
// several minutes depending on complexity.
// - The LaTeX representations produced in connection with 
// (i) hubs-and-satellites primary topologies (configuration option hubsAndSats),
// (ii) random secondary topologies (configuration option secTopology 0),
// (iii) topologies that are read from a file (configuration option 
// secTopology 5) are treated as generic, unstructured graphs. For legibility,
// these representations are only available when total number of nodes is not
// greater than 12 and numbCom <= 4.
//
// -noStdOut <> (no value supplied); do not save output with STD format
//
// -seed <int: 0, INT_MAX>; seed of the pseudo-random generator
//
// -stream <int: 0, INT_MAX>; stream of the pseudo-random generator
//
// -numbCom <int: 1, 1000>; number of commodities
//
// -oneSrcOneSnk <> (no value supplied); select randomly one source and one 
// sink for each commodity; (source, sink) pairs must be different across all
// commodities
//
// -eqSrcsEqSnks <> (no value supplied); If commodity volumes that
// are randomly generated according to minComVol, maxComVol are all at least
// equal to the numbers of source nodes and sink nodes that are randomly
// generated according to minNbSrc, maxNbSrc, minNbSnk, maxNbSnk, then all
// commodities will share common source nodes and common sink nodes.
// More generally: 
// (i) Commodities whose volumes are not smaller than the number of source
// nodes will share the same source nodes. However, commodities that have 
// volumes smaller than the number of source nodes will only share source
// nodes for which they have positive volume.
// (ii) Commodities whose volumes are not smaller than the number of sink
// nodes will share the same sink nodes. However, commodities that have 
// volumes smaller than the number of sink nodes will only share sink
// nodes for which they have negative volume.
// (iii) These conditions implicitly exclude that a node be simultaneously
// a sink and a source.
//
// Remarks:
// - oneSrcOneSnk and eqSrcsEqSnks cannot be both active; if oneSrcOneSnk and
// eqSrcsEqSnks are both inactive, then sources and sinks are selected randomly
// according to minNbSrc, maxNbSrc, minNbSnk, maxNbSnk.
// - When sources and sinks are selected randomly, any node can be
// simultaneously a sink for one or more commodities and source for one or more
// other commodities.
//
// -minNbSrc <int: 1, 1000>; min number of sources; inactive under
// oneSrcOneSnk
//
// -maxNbSrc <int: 1, 1000>; max number of sources; inactive under
// oneSrcOneSnk
//
// -minNbSnk <int: 1, 1000>; min number of sinks; inactive under
// oneSrcOneSnk
//
// -maxNbSnk <int: 1, 1000>; max number of sinks; inactive under
// oneSrcOneSnk
//
// -inclDesignObjCap <> (no value supplied); when saving in LP or MPS format,
// (i) design variables will appear with fixed costs in objective,
// (ii) design variables will appear in overall arc-capacity constraints
//
// -inclDesignObjCommCap <> (no value supplied); when saving in LP or MPS
// format, (i) design variables will appear with fixed costs in objective,
// (ii) design variables will appear in commodity-specific capacity 
// constraints.
//
// Important:
// Whereas the DOW, STD and TEX formats store sufficient basic information 
// to uniquely identify a MCFNDP instance, the LP and MPS formats state a
// formulation of this problem (objective and constraints). LP and MPS are
// respectively human- and machine-readable. Options inclDesignObjCap and
// inclDesignObjCommCap allow to select particular formulations of the 
// linear programming statement of the MCFNDP with LP and MPS.
//
// By default, (i.e., when options inclDesignObjCap and inclDesignObjCommCap
// are omitted or explicitly set to false) design variables are altogether
// absent: (i) the objective omits fixed costs as if all existing arcs are
// costlessly opened), (ii) overall capacity constraints for all arcs are
// included, (iii) commodity-specific capacity constraints for all arcs
// and commodities are included.
//
// When only inclDesignObjCap is active, design variables are included: (i)
// objective includes fixed costs of opening arcs, (ii) overall capacity
// constraints where design variables multiply overall capacity are included
// for each arc, (iii) commodity-specific capacity constraints, where design
// variables do not appear, are included.
//
// When only inclDesignObjCommCap is active, design variables are included:
// (i) the objective includes fixed costs of opening arcs, (ii) overall
// capacity constraints where design variables do not appear are included
// for each arc, (iii) commodity-specific capacity constraints, where design
// variables multiply overall capacity are included for each arc and commodity.
//
// Producing the standard formulation of the MCFNDP requires that both
// inclDesignObjCap and inclDesignObjCommCap be active.
//
// 4.2.2 Options available under the point-to-point primary topology
// *****************************************************************
// (That is, when option hubsAndSats is either (i) absent and implicitly set to
// false, or (ii) present and explicitly set to false.)
//	
// -secTopology <int: 0, 1, 2, 3, 4, 5>; 
// 0 calls for a base secondary topology that is entirely random and where arcs
// are set between nodes according to nbAddArcSampTailHead and/or 
// nbAddArcPerSampHead; 1 to 5 call for a particular base secondary graph
// topology (before additional, random arcs are eventually added according to
// nbAddArcSampTailHead and/or nbAddArcPerSampHead): 1 grid on a torus, 2 grid
// on a cylinder, 3 grid on a bounded surface, 4 circular, 5 read (nodes + arcs
// without costs and capacities) from file
//
// Important:
// - Whenever secTopology is set to 5 so that a basic graph is read from a file
// (whose name is specified with option longBasicGraphFileName), value of
// option numbNodes must be set to a number not smaller than the exact number
// of nodes found in this file.
// - Whenever secTopology is set to 0 so that arcs are all randomly generated,
// no guarantee is made that the resulting instance will be feasible. This
// contrasts with settings 1 to 4 where the non-random arcs that are initially 
// generated produce a strongly connected graph and have sufficient capacity to
// carry all outstanding commodity volumes.
//
// -longBasicGraphFileName <string>; long name (including extension) of file
// where basic graph (excluding random arcs, capacities and costs ) will be
// read if secTopology == 5 and will be written if 1 <= secTopology <= 4;
// default value is './basic_graph.gra' (Linux instructions) or
// './inout/basic_graph.gra' (Docker instructions); when using Linux
// instructions, absolute path or path relative to the /program folder may be
// added
//
// -lengthX <int: 1, 1000>; length along X axis if secTopology is in
// {1, 2, 3} (inactive otherwise)
//
// -lengthY <int: 1, 1000>; length along Y axis if secTopology is in
// {1, 2, 3} (inactive otherwise)
//
// -numbNodes <int: 1, 10000>; number of nodes under random or circular
// secondary topology; (overridden by lengthX * lengthY under grid secondary
// topology)
//
// -nbAddArcSampTailHead <int: 0, 1000>; total number of additional random
// arcs generated by sampling over tail (origin) and head (destination) nodes;
// if this option is inactive, default value is 0
//
// -nbAddArcPerSampHead <int: 0, 1000>; number of additional random arcs
// per tail (origin) node generated by sampling over head (destination) nodes;
// if this option is inactive, default value is 0
//
// -minComVol <int: 1, 10000>; min commodity volume
//
// -maxComVol <int: 1, 10000>; max commodity volume
//
// -minVarCos <int: 0, 10000>; min commodity-specific variable cost
//
// -maxVarCos <int: 0, 10000>; max commodity-specific variable cost
//
// -minArcCap <int: 1, 10000>; min arc overall capacity
//
// -maxArcCap <int: 1, 10000>; max arc overall capacity
//
// -minFixCos <int: 0, 10000>; min fixed cost
//
// -maxFixCos <int: 0, 10000>; max fixed cost
//
// -minComCap <int: 1, 10000>; min commodity-specific arc capacity
//
// -maxComCap <int: 1, 10000>; max commodity-specific arc capacity
//
// -ratZeroFix <int: 0, 100>; percent of random arcs with fixed cost
// set to zero; if this option is inactive, default percentage is 0
//
// -ratTotVolCap <int: 0, 100>; percent of random arcs with total
// capacity set to total volume; if this option is inactive, default
// percentage is 0
//
// -ratZeroComCap <int: 0, 100>; percent of random arcs with one of the
// commodity-specific capacities set to zero; if this option is inactive,
// default percentage is 0
//
// -ratMaxComCap <int: 0, 100>; percent of random arcs with one of the
// commodity-specific capacities set to maxComCap; if this option is inactive,
// default percentage is 0
//
// -noParalArc <int: 1, 100>; when adding random arcs, avoid parallel arcs
// and set to this figure the maximum number of failed attempts to add a non-
// parallel arc. If reached, decrease the number of random arcs to be added
// by one; if this option is inactive, no attempt will be made to avoid 
// parallel arcs
//
// -adjCapacsDown <dbl: 0.0001, 10000>; normalize overall capacities of all
// arcs (non-random and random) by uniform adjustment (downward if value of 
// adjCapacsDown >= 1, upward otherwise), whose magnitude is proportional to
// the specified figure (see Graph::adjustArcCapacities() in graph.cpp); if
// this option is omitted, no adjustment will be made
//
// Remark:
// Setting adjCapacsDown to 1 is not equivalent to no adjustment. This 
// setting balances the sum of total commodity volumes flowing on all arcs
// with the sum of all arc capacities. It might be viewed as a neutral
// setting. 
//
// -adjFixCostsUp <dbl: 0.0001, 10000>; normalize fixed costs of all arcs
// (non-random and random) by uniform adjustment (downward if value of 
// adjFixCostsUp <= 1, upward otherwise) whose magnitude is proportional to
// the specified figure (see Graph::adjustFixedCosts() in graph.cpp); if this
// option is inactive, no adjustment will be made
//
// Remark:
// Setting adjFixCostsUp to 1 is not equivalent to no adjustment. This 
// setting balances the sum of fixed costs over all arcs with the total 
// variable cost that would be incurred by circulating average commodity
// volume on all arcs. It might be viewed as a neutral setting. 
//
// -equalComCaps <> (no value supplied); equalize commodity-specific
// capacities over arcs as follows: for all k and a, capacity for
// commodity k over arc a is equated to minimum of volume of commodity k
// and overall capacity of arc a
//
// -equalVarCosts <> (no value supplied); equalize commodity-specific
// variable costs over arcs as follows: for all k and a, variable cost for
// commodity k over arc a is equated to variable cost for commodity 0 over
// arc a
//
// 4.2.3 Options available under the hubs-and-satellites primary topology
// **********************************************************************
// (That is, when option hubsAndSats is (i) present and implicitly set to true, or
// (ii) present and explicitly set to true.)
//
// -minComVol <int: 1, 10000>; min commodity volume
//
// -maxComVol <int: 1, 10000>; max commodity volume
//
// -longBasicGraphFileName <string>; long name (including extension) of file
// where basic graph (excluding random arcs, capacities and costs ) will be
// written if 1 <= secTopology <= 4; default value is './basic_graph.gra'
// (Linux instructions) or './inout/basic_graph.gra' (Docker instructions);
// when using Linux instructions, absolute path or path relative to the
// /program folder may be added
//
// Important:
// Under the hubs-and-satellites primary topology, secTopology cannot be
// set to 5. That is, the topology cannot be read from a basic graph file.
// However, a basic graph file will be written if 1 <= secTopology <= 4.
// In this case, the graph will be saved unstructured, without information
// about its original (i.e., hubs-and-satellites) primary topology.
// Subsequently, this basic graph file could be read in a new run in which 
// the primary topology would be set to point-to-point. Obviously, the
// original hubs-and-satellites characterization would be lost.
//
// -noParalArc <int: 1, 100>; when adding random arcs, avoid parallel arcs
// and set to this figure the maximum number of failed attempts to add a non-
// parallel arc. If reached, decrease the number of random arcs to be added
// by one; if this option is inactive, no attempt will be made to avoid 
// parallel arcs
//
// 4.2.3.1 Options describing hub nodes and hub arcs
// *************************************************
//
// Definition:
// A hub arc connects two hub nodes.
//
// -secTopology <int: 0, 1, 2, 3, 4>; 0 calls for a secondary topology that is
// entirely random and where arcs are set between hub nodes according to
// nbAddArcSampTailHead and/or nbAddArcPerSampHead; 1 to 4 call for a 
// particular basic secondary topology (before additional, random arcs are 
// eventually added according to nbAddArcSampTailHead and/or
// nbAddArcPerSampHead): 1 grid on a torus, 2 grid on a cylinder, 3 grid on a
// bounded surface, 4 circular.
//
// Important:
// Under the hubs-and-satellites primary topology, option secTopology cannot be
// set to 5. That is, the topology cannot be read from a basic graph file.
//
// -lengthX <int: 1, 1000>; length along X axis if hub topology option 
// secTopology is in {1, 2, 3} (inactive otherwise)
//
// -lengthY <int: 1, 1000>; length along Y axis if hub topology option
// secTopology is in {1, 2, 3} (inactive otherwise)
//
// -numbNodes <int: 1, 10000>; number of hub nodes under random or circular
// hub topology; (overridden by lengthX * lengthY under grid secondary topology)
//
// -nbAddArcSampTailHead <int: 0, 1000>; total number of additional random
// arcs between hub nodes generated by sampling over tail (origin) and head 
// (destination) hub nodes; if this option is inactive, default value is 0
//
// -nbAddArcPerSampHead <int: 0, 1000>; number of additional random arcs
// per tail (origin) hub node generated by sampling over head (destination)
// hub nodes; if this option is inactive, default value is 0
//
// -minVarCos <int: 0, 10000>; min commodity-specific variable cost over hub
// arc
//
// -maxVarCos <int: 0, 10000>; max commodity-specific variable cost over hub
// arc
//
// -minArcCap <int: 1, 10000>; min arc overall capacity over hub arc
//
// -maxArcCap <int: 1, 10000>; max arc overall capacity over hub arc
//
// -minFixCos <int: 0, 10000>; min fixed cost over hub arc
//
// -maxFixCos <int: 0, 10000>; max fixed cost over hub arc
//
// -minComCap <int: 1, 10000>; min commodity-specific capacity over hub arc
//
// -maxComCap <int: 1, 10000>; max commodity-specific capacity over hub arc
//
// -ratZeroFix <int: 0, 100>; percent of random hub arcs with fixed cost set to
// zero; if this option is inactive, default percentage is 0
//
// -ratTotVolCap <int: 0, 100>; percent of random hub arcs with total capacity
// set to total volume; if this option is inactive, default percentage is 0
//
// -ratZeroComCap <int: 0, 100>; percent of random hub arcs with one of the 
// commodity-specific capacities set to zero; if this option is inactive,
// default percentage is 0
//
// -ratMaxComCap <int: 0, 100>; percent of random hub arcs with one of the 
// commodity-specific capacities set to maxComCap; if this option is inactive,
// default percentage is 0
//
// -adjCapacsDown <dbl: 0.0001, 10000>; normalize overall capacities of all hub
// arcs (non-random and random) by uniform adjustment (downward if value of 
// adjCapacsDown >= 1, upward otherwise), whose magnitude is proportional to
// the specified figure (see Graph::adjustArcCapacities() in graph.cpp); if 
// this option is omitted, no adjustment will be made
//
// Remark:
// Setting adjCapacsDown to 1 is not equivalent to no adjustment. This 
// setting balances the sum of total commodity volumes flowing on all hub arcs
// with the sum of all hub arc capacities. It might be viewed as a neutral
// setting. 
//
// -adjFixCostsUp <dbl: 0.0001, 10000>; normalize fixed costs of all hub arcs
// (non-random and random) by uniform adjustment (downward if value of 
// adjFixCostsUp <= 1, upward otherwise) whose magnitude is proportional to the
// specified figure (see Graph::adjustFixedCosts() in graph.cpp); if this
// option is inactive, no adjustment will be made
//
// Remark:
// Setting adjFixCostsUp to 1 is not equivalent to no adjustment. This 
// setting balances the sum of fixed costs over all hub arcs with the total 
// variable cost that would be incurred by circulating average commodity
// volume on all hub arcs. It might be viewed as a neutral setting.
//
// -equalComCaps <> (no value supplied); equalize commodity-specific capacities
// over hub arcs as follows: for all k and a, capacity for commodity k over arc
// a is equated to minimum of volume of commodity k and overall capacity of hub
// arc a
//
// -equalVarCosts <> (no value supplied); equalize commodity-specific
// variable costs over hub arcs as follows: for all k and a, variable cost for
// commodity k over arc a is equated to variable cost for commodity 0 over hub
// arc a
//
// 4.2.3.2 Options describing satellite nodes and satellite arcs
// *************************************************************
//
// Definitions:
// (i) A satellite arc connects either two satellite nodes or one hub node and
// one satellite node.
// (ii) A round is a set of satellite nodes that are assigned to a hub node and
// that are sequentially connected. End nodes in the round are connected to the
// hub node.
//
// -minSatsPerHub <int: 0, 1000); minimum number of satellites per hub
//
// -maxSatsPerHub <int: 0, 1000); maximum number of satellites per hub
//
// -minSatsRoundSize <int: 1, 1000); minimum number of satellites in a round
//
// -maxSatsRoundSize <int: 1, 1000);	maximum number of satellites in a round
//	
// -nbAddSatArcSampTailHead <int: 0, 10000>; total number of additional random
// arcs between satellite nodes generated by sampling over tail (origin) and
// head (destination) nodes; if this option is inactive, default value is 0
//
// -nbAddSatArcPerSampHead <int: 0, 10000>; number of additional random arcs 
// between satellite nodes per tail (origin) node generated by sampling over 
// head (destination) nodes; if this option is inactive, default value is 0
//	
// -minSatVarCos <int: 0, 10000>; min commodity-specific variable cost for
// arcs involving at least one satellite node
//
// -maxSatVarCos <int: 0, 10000>; max commodity-specific variable cost for
// arcs involving at least one satellite node
//
// -minSatArcCap <int: 1, 10000>; min overall capacity for arcs involving
// at least one satellite node
//
// -maxSatArcCap <int: 1, 10000>; max overall capacity for arcs involving at
// least one satellite node
//
// -minSatFixCos <int: 0, 10000>; min fixed cost for arcs involving at least
// one satellite node
//
// -maxSatFixCos <int: 0, 10000>; max fixed cost for arcs involving at least
// one satellite node
//
// -minSatComCap <int: 1, 10000>; min commodity-specific capacity for arcs 
// involving at least one satellite node
//
// -maxSatComCap <int: 1, 10000>; max commodity-specific capacity for arcs
// involving at least one satellite node
//
// -ratSatZeroFix <int: 0, 100>; percentage of random satellite arcs with fixed
// cost forced to zero; if this option is inactive, default percentage is 0
//
// -ratSatTotVolCap <int: 0, 100>; percentage of random satellite arcs with 
// total capacity forced to total volume; if this option is inactive, default 
// percentage is 0
//
// -ratSatZeroComCap <int: 0, 100>; percentage of random satellite arcs with
// one of the commodity-specific capacities forced to zero; if this option is
// inactive, default percentage is 0
//
// -ratSatMaxComCap <int: 0, 100>; percentage of random satellite arcs with one
// of the commodity-specific capacities forced to maxComCap; if this option is
// inactive, default percentage is 0
//
// -adjSatCapacsDown <dbl: 0.0001, 10000>; normalize overall capacities of all
// satellite arcs (non-random and random) by uniform adjustment (downward if
// value of adjSatCapacsDown >= 1, upward otherwise), whose magnitude is
// proportional to the specified figure (see Graph::adjustArcCapacities_sat() 
// in graph.cpp); if this option is omitted, no adjustment will be made
//
// Remark:
// Setting adjSatCapacsDown to 1 is not equivalent to no adjustment. This 
// setting balances the sum of total commodity volumes flowing on all arcs with
// the sum of all arc capacities. It might be viewed as a neutral setting. 
//
// -adjSatFixCostsUp <dbl: 0.0001, 10000>; normalize fixed costs of all 
// satellite arcs (non-random and random) by uniform adjustment (downward if
// value of adjSatFixCostsUp <= 1, upward otherwise) whose magnitude is
// proportional to the specified figure (see Graph::adjustFixedCosts_sat() in
// graph.cpp); if this  option is inactive, no adjustment will be made
//
// Remark:
// Setting adjSatFixCostsUp to 1 is not equivalent to no adjustment. This 
// setting balances the sum of fixed costs over all arcs with the total
// variable cost that would be incurred by circulating average commodity
// volume on all arcs. It might be viewed as a neutral setting.
//
// -equalSatComCaps <> (no value supplied); equalize commodity-specific
// capacities over satellite arcs as follows: for all k and a, capacity for
// commodity k over satellite arc a is equated to minimum of volume of
// commodity k and overall capacity of satellite arc a
//
// -equalSatVarCosts <> (no value supplied); equalize commodity-specific
// variable costs over satellite arcs as follows: for all k and a, variable 
// cost for commodity k over satellite arc a is equated to variable cost for
// commodity 0 over satellite arc a
//
// 4.3 Joint restrictions
// **********************
//
// minComVol <= maxComVol
// minVarCos <= maxVarCos
// minFixCos <= maxFixCos
// minArcCap <= maxArcCap		
// 
// if (!equalComCaps)
// {
// 		minComCap <= maxComCap
// 		maxComCap <= minComVol
// 		maxComCap <= minArcCap
// 		maxArcCap <= numbCom * minComCap			
// }	
// 	
// if (1 <= secTopology <= 3)
// {
//     	lengthX * lengthY >= 2            
// }
// else
// {
//     	numbNodes >= 2            
// }	
// 	
// primary topology is point-to-point
// if (!hubsAndSats)
// {
//		if (oneSrcOneSnk)
// 		{
//			!eqSrcsEqSnks
//			
// 			if (1 <= secTopology <= 3)
//			{
//				(lengthX * lengthY) * ((lengthX * lengthY) - 1) >= numbCom					
//			}
//			else
//			{
//				numbNodes * (numbNodes - 1) >= numbCom					
//			}
//		}
//		else
//		{
//			minNbSrc <= maxNbSrc
//			minNbSnk <= maxNbSnk				
//
//			if (1 <= secTopology <= 3)
//			{
//				maxNbSrc + maxNbSnk <= lengthX * lengthY					
//			}
//			else
//			{
//				maxNbSrc + maxNbSnk <= numbNodes					
//			}
//		}
// }
// // primary topology is hubs-and-satellites
// else
// {
//		secTopology != 5
//		minSatsPerHub <= maxSatsPerHub			
//		minSatsRoundSize <= maxSatsRoundSize
//		minSatFixCos <= maxSatFixCos			
//		minSatVarCos <= maxSatVarCos		
//		minSatArcCap <= maxSatArcCap			
//		
//		if (!equalSatComCaps)
//		{
//			minSatComCap <= maxSatComCap			
//			maxSatComCap <= minComVol
//			maxSatComCap <= minSatArcCap
//			maxSatArcCap <= numbCom * minSatComCap				
//		}
//			
//		if (oneSrcOneSnk)
//		{
//			!eqSrcsEqSnks
//				
//			if (1 <= secTopology <= 3)
//			{
//				lengthX * lengthY * minSatsPerHub * (lengthX * lengthY * minSatsPerHub - 1) >= numbCom					
//			}
//			else
//			{
//				numbNodes * minSatsPerHub * (numbNodes * minSatsPerHub - 1) >= numbCom					
//			}				
//		}
//		else
//		{
//			minNbSrc <= maxNbSrc
//			minNbSnk <= maxNbSnk				
//
//			if (1 <= secTopology <= 3)
//			{
//				maxNbSrc + maxNbSnk <= lengthX * lengthY * minSatsPerHub					
//			}
//			else
//			{
//				maxNbSrc + maxNbSnk <= numbNodes * minSatsPerHub					
//			}
//		}
// }	
//
// 5 Detailed examples
// *******************
//
// The program/examples subfolder holds examples of configuration files and 
// instruction lines occurring when the executable of the program is run
// directly from the Linux instruction line. It also contains the output files
// resulting from each pair of configuration file and instruction line.
//
// Similarly, the docker/examples subfolder holds examples of configuration
// files and instruction lines occurring when the executable of the program is
// run through Docker instructions. These examples are identified by the
// addition of the prefix 'docker_'. The subfolder also contains the output
// files resulting from each pair of configuration file and instruction line.
// The former are identical to the files generated when the executable is run
// from the Linux instruction line.
//
// Remark:
// The output examples contained in the /examples subfolders will also be used
// to illustrate the operation of the separate application generating 
// stochastic MCFNDP instances.
//
// In all included examples, the generated MCFNDP instance is characterized by
// a point-to-point primary topology, a (3 x 4) grid secondary topology applied
// to a bounded surface and by 3 commodities.
//
// 5.1 One source and one sink for each commodity
// **********************************************
//
// Folder /examples/oneSrcOneSnk/
//
// In this case, there is one source and one sink for each commodity and no
// differentiation of variable costs and capacities according to commodities.
// The generated instances can be saved in the DOW and STD formats.
//
// 5.1.1 Instructions
// ******************
//
// Linux command line instruction:
//
// ./determ_gen +v +F determ_config_oneSrcOneSnk_12oct2026-17h09.par
//
// Docker instruction:
//
// docker run --init --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +v +F /inout/docker_determ_config_oneSrcOneSnk_12oct2026-17h09.par
//
// 5.1.2 Configuration files (input)
// *********************************
//
// configuration file used with Linux command line:
// determ_config_oneSrcOneSnk_12oct2026-17h09.par
//
// configuration file used with Docker instruction:
// docker_determ_config_oneSrcOneSnk_12oct2026-17h09.par
//
// 5.1.3 Generated files
// *********************
//
// numerical representation of the instance in DOW format:
// determ_oneSrcOneSnk_12oct2026-17h09.dow
//
// numerical representation of the instance in STD format:
// determ_oneSrcOneSnk_12oct2026-17h09.std
//
// numerical representation of the basic graph:
// determ_oneSrcOneSnk_12oct2026-17h09.gra
//
// human-readable representation of the associated linear programming problem
// in LP format:
// determ_oneSrcOneSnk_12oct2026-17h09.lp
//
// numerical representation of the associated linear programming problem in
// MPS format:
// determ_oneSrcOneSnk_12oct2026-17h09.mps
//
// graphical representation of the instance in TEX format:
// determ_oneSrcOneSnk_12oct2026-17h09.tex
//
// compiled graphical representation:
// determ_oneSrcOneSnk_12oct2026-17h09.pdf
//
// 5.2 Multiple but equal sources and multiple but equal sinks
// ***********************************************************
//
// Folder /examples/eqSrcsEqSnks/
//
// In this case, there are multiple sources and multiple sinks for each
// commodity. However, sources and sinks are identical across commodities.
// There is differentiation of variable costs and capacities according to
// commodities. The generated instances cannot be saved in the DOW format.
//
// 5.2.1 Instructions
// ******************
//
// Linux command line instruction:
//
// ./determ_gen +v +F determ_config_eqSrcsEqSnks_12oct2026-17h09.par
//
// Docker instruction:
//
// docker run --init --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +v +F /inout/docker_determ_config_eqSrcsEqSnks_12oct2026-17h09.par
//
// 5.2.2 Configuration files (input)
// *********************************
//
// configuration file used with Linux command line:
// determ_config_eqSrcsEqSnks_12oct2026-17h09.par
//
// configuration file used with Docker instruction:
// docker_determ_config_eqSrcsEqSnks_12oct2026-17h09.par
//
// 5.2.3 Generated files
// *********************
//
// numerical representation of the instance in STD format:
// determ_eqSrcsEqSnks_12oct2026-17h09.std
//
// numerical representation of the basic graph:
// determ_eqSrcsEqSnks_12oct2026-17h09.gra
//
// human-readable representation of the associated linear programming problem
// in LP format:
// determ_eqSrcsEqSnks_12oct2026-17h09.lp
//
// numerical representation of the associated linear programming problem in
// MPS format:
// determ_eqSrcsEqSnks_12oct2026-17h09.mps
//
// graphical representation of the instance in TEX format:
// determ_eqSrcsEqSnks_12oct2026-17h09.tex
//
// compiled graphical representation:
// determ_eqSrcsEqSnks_12oct2026-17h09.pdf
//
//

#include "classes.h"

int main(int argc, char **argv)
{	
   Data data(argc, argv, "./determ_config.par");

   Graph graph(&data);

   graph.output();
}
