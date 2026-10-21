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
// FEASIBILITY TESTER FOR INSTANCES OF THE DETERMINISTIC LINEAR, TWO-STAGE,
// MULTI-COMMODITY, CAPACITATED, FIXED CHARGE, NETWORK DESIGN PROBLEM
//
// USER GUIDE
//
// This application verifies if a specified instance of the deterministic
// multi-commodity, capacitated, fixed charge network design problem (MCFNDP) is
// feasible. It does so by (i) opening all existing arcs in the network,
// (ii) setting the fixed cost over each arc to 0, (iii) setting the variable
// cost over each arc-commodity pair to 1, (iv) leaving unchanged demanded and
// supplied commodity volumes at every node as well as overall capacities and
// commodity-specific capacities, and then (v) verifying if a minimum cost
// solution is available under these conditions.
//
// This user guide comprises the following sections:
// 1 Building and running the program directly from the Linux command line
// 2 Running the program through Docker instructions
// 3 Format of the deterministic MCFNDP instance file
// 4 Detailed examples
//
// 1 Building and running the program directly from the Linux command line
// ***********************************************************************
//
// 1.1 Building the program from the Linux command line
// ****************************************************
//
// Under Linux, follow these steps to build the program:
//
// a- Install the CPLEX solver and/or the HIGHS solver (if not already done)
// and note the installation folder(s). According to our verifications, CPLEX
// and HIGHS work equally well in our application. HIGHS is freely available
// under the MIT license at https://highs.dev/. See Huangfu, Q., Hall, J.A.J.
// Parallelizing the dual revised simplex method. Math. Prog. Comp. 10, 119-142
// (2018).
// b- If not already done, either (i) use Git to clone locally the remote
// repository https://github.com/larseeri/determ-MCFNDP-gener, or (ii)
// generate, download locally to an appropriate folder and uncompress a .zip
// archive containing the remote repository file tree.
// c- In a Linux terminal, go to the /check_feasib/program folder just created.
// d- Edit the file named "makefile" to set the variables indicating the
// locations relevant to the CPLEX solver and/or the HIGHS solver, as noted in
// step a-. These locations are chiefly CPLEX_HOME and HIGHS_HOME. Save the
// modifications.
// e- Make sure that the gcc C++ compiler and the make C++ build tool are
// available.
// f- In the terminal, run ONLY ONE of these commands to build the program
// equipped either with the CPLEX solver or with the HIGHS solver:
//
// make CPLEX_SOLVER=1
//
// make HIGHS_SOLVER=1
//
// An executable file named "determ_feasib" will be created inside the
// /check_feasib/program folder. Building with versions 11.5.0, 12.2.0, 13.2.0,
// 14.2.0 and 15.1.0 of the gcc compiler has been verified.
//
// Remarks:
// - To build once again with HIGHS if the program has already been built with
// CPLEX, run the following:
//
// make clean CPLEX_SOLVER=1
// make HIGHS_SOLVER=1
//
// - To build once again with CPLEX if the program has already been built with
// HIGHS, run the following:
//
// make clean HIGHS_SOLVER=1
// make CPLEX_SOLVER=1
//
// 1.2 Running the program with Linux command line instructions
// ************************************************************
//
// The general command line for running the program from the
// /check_feasib/program folder just created is as follows:
//
// ./determ_feasib [+v] -F formatOfInstance -I pathOfInstanceFile
//
// where the two available main options are
//
// F <char>; format of file containing the deterministic MCFNDP instance;
// D: DOW, R: RSTD, G: GSTD, default: G (see Section 3 below for details)
//
// I <string>; path of file where the deterministic MCFNDP instance is to be
// read; default value is './default_instance.std'; (see further
// below about Docker instructions)
//
// Important:
// The path specified with option key I must not contain any blank space.
//
// The result is displayed on standard output as follows:
//
// Verifying feasibility: SUCCESS
//
// or
//
// Verifying feasibility: FAILURE
//
// The verbosity option +v can be used to obtain more information about the
// option parsing process.
//
// To display the complete documentation about the application and the
// available options, run
//
// ./determ_feasib +H
//
// See Section 4 below for examples of instructions when the program is run
// with command line instructions.
//
// 2 Running the program through Docker instructions
// *************************************************
//
// An advantage of running the executable determ_feasib inside a Docker 
// container is the possibility to avoid selection and installation of suitable
// C++ compiler, C++ make build tool and required C++ libraries. It suffices
// that the Docker engine be installed on the host where the computation will
// be run. For this, see https://docs.docker.com/engine/install/.
//
// Once the Docker engine is installed, follow these steps:
//
// a- Open a terminal and retrieve from the GitHub container registry the
// Docker image ghcr.io/larseeri/determ_feasib_image:latest as follows:
//
// docker pull ghcr.io/larseeri/determ_feasib_image:latest
//
// (From this image, Docker containers running the executable 'determ_feasib'
// will be generated.)
// b- For simplicity, define the alias 'determ_feasib_image' through
//
// docker tag ghcr.io/larseeri/determ_feasib_image:latest determ_feasib_image
//
// Remark:
// If the Docker image determ_feasib_image were instead available as the tar 
// ball determ_feasib_image.tar, then it could be loaded through
//
// docker load --input determ_feasib_image.tar
//
// c- If not already done, either (i) use Git to clone locally the remote
// repository https://github.com/larseeri/determ-MCFNDP-gener, or (ii)
// generate, download locally to an appropriate folder and uncompress a .zip
// archive containing the remote repository file tree. The /check_feasib/docker
// folder contains (i) the primitive files named 'Dockerfile' and 'makefile' 
// based on which the Docker image was originally generated, (ii) a subfolder 
// /inout used for exchanging files between the program running inside a Docker 
// container and the directory structure of the host and (iii) a subfolder
// /examples containing the examples examined in Section 5 below.
// d- In a terminal, go to the /check_feasib/docker folder just created.
//
// Then, instructions similar to those appearing in Section 1.2 can be run from
// the terminal on AMD and Intel CPUs (other CPUs are addressed further below).
// 
// In general, Docker instructions can be formulated from those appearing in
// Section 1.2 by (i) adding the prefix 
//
// docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image
//
// and by (ii) replacing the executable location ./determ_feasib with /determ_feasib
// (i.e. removing the initial dot), since the executable determ_feasib is located
// at the root of the Docker container being generated and since locations 
// inside a Docker container must be accessed through absolute paths.
//
// Hence, the following command line instruction:
//
// ./determ_feasib +v -F formatOfInstance -I pathOfInstanceFile
//
// corresponds to the following Docker instruction:
//
// docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F formatOfInstance -I /inout/instanceFile
//
// Important: 
// When running the program through Docker instructions, all input and output
// files must be located inside the docker/inout subfolder. Within the Docker 
// instructions and the configuration files, this subfolder must be denoted
// /inout since it is located at the root of the Docker container. Hence, an
// instance file located at docker/inout/networkFile.std will be
// specified in the Docker instruction or inside the configuration file with
// -I /inout/networkFile.std.
//
// Important:
// The supplied Docker image conforms to the amd64 CPU instructions standard
// and will run natively on AMD and Intel CPUs. On CPUs conforming instead to
// the arm64 standard, e.g., on recent Apple computers, the supplied Docker 
// image can be run through emulation. Essentially, this may be achieved as
// follows:
// a- Running the following preliminary instruction once. (For details, see
// https://hub.docker.com/r/tonistiigi/binfmt.)
//
// docker run --privileged --rm tonistiigi/binfmt --install all 
//
// b- Adding the option --platform linux/amd64 to the Docker instruction line
// prefix as follows:
//
// docker run --platform linux/amd64 --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image
//
// The following characteristics are specific to the operation of the program
// through Docker instructions:
// - An empty folder absolutePathToDockerFolder/inout must already exist on
// the host for the Docker instructions to work correctly.
// - Option --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout
// connects subfolder docker/inout on the host to folder /inout
// of the Docker container. Hence, the connection between
// absolutePathToDockerFolder/inout on the host and folder /inout in the
// container is used to supply input files to and retrieve output files from
// the container.
// - Option --rm ensures suppression of the container implicitly generated by
// docker run. Note that the container created by docker run for running the
// executable determ_feasib stops running immediately after the latter finishes
// and that it cannot be reused afterwards, for instance through a docker exec
// instruction. Immediate suppression with --rm avoids uselessly cluttering
// Docker's container repository.
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
// See Section 4 below for examples of configuration files and instructions
// when the program is run through Docker.
//
// 3 Format of the deterministic MCFNDP instance file
// **************************************************
//
// By default, the name of the file describing the deterministic MCFNDP
// instance is './default_instance.std' (Linux instructions),
// './inout/network.std' (Docker instructions) and the default input format
// is STD. This name may be changed with option I and the input format can
// be indicated accordingly with option F.
//
// Important:
// It is critical that the content of the file describing the base
// deterministic MCFNDP instance and the indicated format of its content match
// together.
//
// DOW and STD formats are commonly used to store deterministic MCFNDP instances
// in text files. These formats are described in files DOW-format-desc.txt and
// STD-format-desc.txt. The application can read from these two formats and also
// recognizes a specialization of STD.
// The three following formats may be selected with option F.
// a- DOW (option value D) presupposes that (i) for each arc, capacities and variable
// costs are identical for all commodities and (ii) each commodity has a
// single source node (at which volume > 0) and a single sink node (at which
// volume < 0).
// b- GSTD (option value G) designates the STD format. STD is fully generic 
// and imposes neither (i), nor (ii).
// c- RSTD (option R) designates a specialization of STD in which condition (ii)
// above is imposed. It is defined for convenience and for the purposes only of
// the present application.
//
// Important:
// The STD and DOW formats specify that negative arc capacities and negative
// commodity-specific capacities be interpreted as infinite. When reading
// instance files in GSTD, RSTD or DOW format, negative capacities are set
// to 1E+9.
//
// Example
// *******
//
// Consider this hypothetical Linux command line instruction:
//
// ./determ_feasib -I instB.std -F G 
//
// It is interpreted as follows:
// -I instB.std: instance to be read from file instB.std
// -F G: format of the latter is GSTD, that is, generic STD
//
// 4 Detailed examples
// *******************
//
// The /check_feasib/program/examples subfolder contains examples of
// instruction lines running the executable of the program.
//
// Similarly, the /check_feasib/docker/examples subfolder holds examples of
// instruction lines occurring when the executable of the program is run 
// through Docker instructions. These examples are identified by the addition
// of the prefix 'docker_'.
//
// 4.1 One source and one sink for each commodity
// **********************************************
//
// Folder /examples/oneSrcOneSnk/
//
// In the MCFNDP instance, there is one source and one sink for each 
// commodity and no differentiation of variable costs and capacities according
// to commodities.
//
// 4.1.1 DOW format
// ****************
//
// The instance is received in the DOW format.
//
// 4.1.1.1 Operation of the feasibility tester
// *******************************************
//
// Linux command line instruction:
//
// ./determ_feasib +v -F D -I determ_oneSrcOneSnk_12oct2026-17h09.dow
//
// Docker instruction:
//
// docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F D -I /inout/determ_oneSrcOneSnk_12oct2026-17h09.dow
//
// Location of instance file (input):
// /examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.dow
//
// Displayed output:
//
// Verifying feasibility: SUCCESS
//
// 4.1.1.2 Origin of the instance
// ******************************
//
// The instance was originally produced by the deterministic
// generator as follows.
//
// Linux command line instruction:
//
// ./determ_gen +v +F determ_config_oneSrcOneSnk_12oct2026-17h09.par
//
// Configuration file:
// determ_config_oneSrcOneSnk_12oct2026-17h09.par
//
// Outputs in DOW, STD, LP, MPS, GRA (basic graph), TEX (code of graphical
// representation), pdf (compiled image) formats:
// determ_oneSrcOneSnk_12oct2026-17h09.dow
// determ_oneSrcOneSnk_12oct2026-17h09.std
// determ_oneSrcOneSnk_12oct2026-17h09.mps
// determ_oneSrcOneSnk_12oct2026-17h09.lp
// determ_oneSrcOneSnk_12oct2026-17h09.gra
// determ_oneSrcOneSnk_12oct2026-17h09.tex
// determ_oneSrcOneSnk_12oct2026-17h09.pdf
//
// 4.1.2 STD format
// ****************
//
// Similar to 4.1.1, except that the instance is received in the STD format
//
// 4.1.2.1 Operation of the feasibility tester
// *******************************************
//
// Linux command line instruction:
//
// ./determ_feasib +v -F G -I determ_oneSrcOneSnk_12oct2026-17h09.std
//
// Docker instruction:
//
// docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F G -I /inout/determ_oneSrcOneSnk_12oct2026-17h09.std
//
// Location of instance file (input):
// /examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.std
//
// Displayed output:
//
// Verifying feasibility: SUCCESS
//
// 4.1.2.2 Origin of the instance
// ******************************
//
// Similar to 4.1.1.2.
//
// 4.2 Multiple but equal sources and multiple but equal sinks
// ***********************************************************
//
// Folder /examples/eqSrcsEqSnks/
//
// In the MCFNDP instance, there are multiple sources and multiple sinks for each
// commodity. However, sources and sinks are identical across commodities.
// There is differentiation of variable costs and capacities according to
// commodities. The instance must be received in the STD format (as the DOW
// format is unsuitable in this situation).
//
// 4.2.1 Operation of the feasibility tester
// *****************************************
//
// Linux command line instruction:
//
// ./determ_feasib +v -F G -I determ_eqSrcsEqSnks_12oct2026-17h09.std
//
// Docker instruction:
//
// docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F G -I /inout/determ_eqSrcsEqSnks_12oct2026-17h09.std
//
// Location of instance file (input):
// /examples/eqSrcsEqSnks/determ_eqSrcsEqSnks_12oct2026-17h09.std
//
// Displayed output:
//
// Verifying feasibility: SUCCESS
//
// 4.2.2 Origin of the instance
// ****************************
//
// The MCFNDP instance was originally produced by the deterministic
// generator as follows.
//
// Linux command line instruction:
//
// ./determ_gen +v +F determ_config_eqSrcsEqSnks_12oct2026-17h09.par
//
// Configuration file:
// determ_config_eqSrcsEqSnks_12oct2026-17h09.par
//
// Outputs in STD, LP, MPS, GRA (basic graph), TEX (code of graphical
// representation), pdf (compiled image) formats:
// determ_eqSrcsEqSnks_12oct2026-17h09.std
// determ_eqSrcsEqSnks_12oct2026-17h09.mps
// determ_eqSrcsEqSnks_12oct2026-17h09.lp
// determ_eqSrcsEqSnks_12oct2026-17h09.gra
// determ_eqSrcsEqSnks_12oct2026-17h09.tex
// determ_eqSrcsEqSnks_12oct2026-17h09.pdf
//
//
#include <configuration.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "./instance.h"
#include "./model.h"

using namespace std;

typedef vector<int> VectInt;

const int numbMoments = 4;

int CommodOnArc::count = 0;
double *CommodOnArc::ptrCap = NULL;
double *CommodOnArc::ptrCost = NULL;

int Commodity::count = 0;
double *Commodity::ptr = NULL;

int Arc::count = 0;
double *Arc::ptrCap = NULL;
double *Arc::ptrCost = NULL;

int main(int argc, char **argv)
{
    // process configuration options
    Configuration params(argc, argv, "./determ_config.par");
    Instance *inst;

    // initialize computational instance
    if (params.instanceFormat == DOW)
        inst = new InstanceDow(&params);
    else if (params.instanceFormat == RSTD)
        inst = new InstanceRStd(&params);
    else if (params.instanceFormat == GSTD)
        inst = new InstanceGStd(&params);

    // create input and output streams
    ifstream is(params.inputInstFileName);
	
	try
	{
		// read deterministic MCFNDP instance from source;
		// input operator is defined in instance.h;
		// InstanceDow::read() or InstanceStd::read() is called
		is >> *inst;
	}	
	catch (...)
	{
		cerr << "ERROR:\n"
			<< "An exception occurred while attempting to read file " << params.inputInstFileName << "\n"
			<< "The expected format of this file according to value of option key F in instruction line is "
			<< to_string(params.instanceFormat) << ".\n"
			<< "Ascertain that file and specified format are consistent.\n"
			<< "EXECUTION ABORTED" << endl;
        exit(1);		
	}
	
    // create solution model for feasibility testing
    Model *model;

#if CPLEX

    // case GSTD: not necessarily one source node and one sink node per commodity
    if (params.instanceFormat == GSTD)
        model = new CPXModel_GStd(inst);
    // cases DOW or RSTD: one source node and one sink node per commodity
    else
        model = new CPXModel_DowRStd(inst);

#elif HIGHS

    // case GSTD: not necessarily one source node and one sink node per commodity
    if (params.instanceFormat == GSTD)
        model = new HIGHSModel_GStd(inst);
    // cases DOW or RSTD: one source node and one sink node per commodity
    else
        model = new HIGHSModel_DowRStd(inst);

#endif

    cout << "\nVerifying feasibility: ";

    // check if instance is feasible when all arcs
    // are opened
    if (model->testInstance())
    {            
        cout << "SUCCESS" << endl;
    }
	else
	{
		cout << "FAILURE" << endl;
	}    
	
    delete model;
    delete inst;
}
