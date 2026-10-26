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
//
#include "configuration.h"

// Constructor
Configuration::Configuration(int argc, char **argv, string _configFileName)
    : nbArg(argc), vectArg(argv), verbose(false), instanceFormat(GSTD)      
{	
	parseConfigModesFromCmdLine();
	
	inputInstFileName = "./default_instance.std";	
   
    allTypes.push_back(DEMAND);                
    allTypes.push_back(CAP_ARC);                
    allTypes.push_back(CAP_COMMOD);                
    allTypes.push_back(FIXED_COST);                
    allTypes.push_back(VAR_COST);
	
    parseCmdLine();
	
	verifyInputInstance();	
}

// Parses command line.
void Configuration::parseCmdLine()
{
    int a = 1;
    const char *key;
    const char *value;
		
	if (verbose)
		cout << "****** Begin parsing configuration parameters from the command line.\n" << endl;
	
    while (a < nbArg)
    {
        if ((vectArg[a][0] == PARAM_PREFIX))
        {

            key = &vectArg[a][1];
			
			if (strlen(vectArg[a]) > 2)
			{
				cerr << "\nERROR:\n"
					<< "Option key " << vectArg[a] << " was received in instruction line.\n"
					<< "However, it comprises MORE than one character whereas it must have ONLY ONE.\n"			
					<< "EXECUTION ABORTED" << endl;
					exit(1);     
			}			

            if (a < nbArg - 1)
            {
                value = vectArg[a + 1];

                if ((vectArg[a + 1][0] == PARAM_PREFIX) &&
                    isalpha(vectArg[a + 1][1]))
                    value = "";
                else
                    a++;
            }
            else
                value = "";

            if (verbose)
                cout << "[" << key << "] [" << value << "]" << endl;

            switch (*key)
            {
            case 'F':
                if (*value == 'D')
                {
                    instanceFormat = DOW;
                    break;
                }
                else if (*value == 'G')
                {
                    instanceFormat = GSTD;
                    break;
                }
                else if (*value == 'R')
                {
                    instanceFormat = RSTD;
                    break;
                }
				else
				{				
					cerr << "\nERROR:\n"
						<< "Option key F in instruction line can only be followed\n"
						<< "by one of the three following values: D, G, R.\n"
						<< "However, value [" << *value <<  "] was received.\n"
						<< "EXECUTION ABORTED" << endl;
					exit(1);                
					break;    
				}					
            case 'I':
                inputInstFileName = value;				
                break;
            default:
				cerr << "\nERROR:\n"
					<< "Only the following option keys are admissible in\n"
					<< "the instruction line: F, I.\n"
					<< "However, the key " << *key <<  " was received.\n"
					<< "EXECUTION ABORTED" << endl;
				exit (1);                
				break;   
            }
        }

        a++;
    }
	
	if (verbose)
		cout << "\n****** End parsing configuration parameters from the command line." << endl;
}

// While traversing command line, seeks flags for verbose or help modes.
void Configuration::parseConfigModesFromCmdLine()
{	
    for (int a = 1; a < nbArg; a++)
    {
        if (vectArg[a][0] == CONFIG_PREFIX)
        {
            if (strcmp(vectArg[a], CONFIG_VERBOSE_MODE) == 0)
            {
                verbose = true;
            }			
			else if (strcmp(vectArg[a], CONFIG_HELP_MODE) == 0)
            {
                printHelpAndExit();
            }            
            else if (isalpha(vectArg[a][1]))
				cout << "\nWARNING:\n"
					<< "configuration prefix [" << vectArg[a] << "] unknown." << endl;
        }
    }
	
    if (verbose)
    {
        cout << "\n********* VERBOSE MODE *********\n" << endl;        
    }
}

void Configuration::verifyInputInstance()
{	
    if (inputInstFileName == "")
    {
        cerr << "\nERROR:\n"
			<< "No instance file was specified.\n"
			<< "Use option -I\n"
			<< "EXECUTION ABORTED" << endl;
        exit(1);
    }
    else if (!filesystem::exists(inputInstFileName))
    {
        cerr << "\nERROR:\n"
			<< "The specified instance file " << inputInstFileName << "\n"
			<< "could not be found. Use option -I\n"
			<< "EXECUTION ABORTED" << endl;
        exit(1);
    }	
}

// unused
void Configuration::printHelpAndExit()
{
    printf("\nHELP REQUESTED: DISPLAYING USER GUIDE\n");
	printf("\n");
	printf("FEASIBILITY TESTER FOR INSTANCES OF THE DETERMINISTIC LINEAR, TWO-STAGE,\n");printf("MULTI-COMMODITY, CAPACITATED, FIXED CHARGE, NETWORK DESIGN PROBLEM\n");
	printf("\n");
	printf("USER GUIDE\n");
	printf("\n");
	printf("This application verifies if a specified instance of the deterministic\n");
	printf("multi-commodity, capacitated, fixed charge network design (MCFND) problem is\n");
	printf("feasible. It does so by (i) opening all existing arcs in the network,\n");
	printf("(ii) setting the fixed cost over each arc to 0, (iii) setting the variable\n");
	printf("cost over each arc-commodity pair to 1, (iv) leaving unchanged demanded and\n");
	printf("supplied commodity volumes at every node as well as overall capacities and\n");
	printf("commodity-specific capacities, and then (v) verifying if a minimum cost\n");
	printf("solution is available under these conditions.\n");
	printf("\n");
	printf("This user guide comprises the following sections:\n");
	printf("1 Building and running the program directly from the Linux command line\n");
	printf("2 Running the program through Docker instructions\n");printf("3 Format of the deterministic MCFND instance file\n");
	printf("4 Detailed examples\n");
	printf("\n");
	printf("1 Building and running the program directly from the Linux command line\n");
	printf("***********************************************************************\n");
	printf("\n");
	printf("1.1 Building the program from the Linux command line\n");
	printf("****************************************************\n");
	printf("\n");
	printf("Under Linux, follow these steps to build the program:\n");
	printf("\n");
	printf("a- Install the CPLEX solver and/or the HIGHS solver (if not already done)\n");
	printf("and note the installation folder(s). According to our verifications, CPLEX\n");
	printf("and HIGHS work equally well in our application. HIGHS is freely available\n");
	printf("under the MIT license at https://highs.dev/. See Huangfu, Q., Hall, J.A.J.\n");
	printf("Parallelizing the dual revised simplex method. Math. Prog. Comp. 10, 119-142\n");
	printf("(2018).\n");	
	printf("b- If not already done, either (i) use Git to clone locally the remote\n");
	printf("repository https://github.com/larseeri/determ-MCFNDP-gener, or (ii)\n");
	printf("generate, download locally to an appropriate folder and uncompress a .zip\n");
	printf("archive containing the remote repository file tree.\n");	
	printf("c- In a Linux terminal, go to the /check_feasib folder just created.\n");
	printf("d- Edit the file named \"makefile\" to set the variables indicating the\n");
	printf("locations relevant to the CPLEX solver and/or the HIGHS solver, as noted in\n");
	printf("step a-. These locations are chiefly CPLEX_HOME and HIGHS_HOME. Save the\n");
	printf("modifications.\n");	
	printf("e- Make sure that the gcc C++ compiler and the make C++ build tool are\n");
	printf("available.\n");
	printf("f- In the terminal, run ONLY ONE of these commands to build the program\n");
	printf("equipped either with the CPLEX solver or with the HIGHS solver:\n");
	printf("\n");
	printf("make CPLEX_SOLVER=1\n");
	printf("\n");
	printf("make HIGHS_SOLVER=1\n");
	printf("\n");
	printf("An executable file named \"determ_feasib\" will be created inside the\n");	
	printf("/check_feasib/program folder. Building with versions 11.5.0, 12.2.0, 13.2.0,\n");
	printf("14.2.0 and 15.1.0 of the gcc compiler has been verified.\n");
	printf("\n");
	printf("Remarks:\n");
	printf("- To build once again with HIGHS if the program has already been built with\n");
	printf("CPLEX, run the following:\n");
	printf("\n");
	printf("make clean CPLEX_SOLVER=1\n");
	printf("make HIGHS_SOLVER=1\n");
	printf("\n");
	printf("- To build once again with CPLEX if the program has already been built with\n");
	printf("HIGHS, run the following:\n");
	printf("\n");
	printf("make clean HIGHS_SOLVER=1\n");
	printf("make CPLEX_SOLVER=1\n");
	printf("\n");
	printf("1.2 Running the program with Linux command line instructions\n");
	printf("************************************************************\n");
	printf("\n");	
	printf("The general command line for running the program from the\n");
	printf("/check_feasib/program folder just created is as follows:\n");	
	printf("\n");
	printf("./determ_feasib [+v] -F formatOfInstance -I pathOfInstanceFile\n");
	printf("\n");
	printf("where the two available main options are\n");
	printf("\n");
	printf("F <char>; format of file containing the deterministic MCFND network;\n");
	printf("D: DOW, G: STD; default: G\n");
	printf("\n");
	printf("I <string>; path of file where the deterministic MCFND network is to be\n");
	printf("read; default value is './network.std'; (see further\n");
	printf("below about Docker instructions)\n");	
	printf("\n");
	printf("The result is displayed on standard output as follows:\n");
	printf("\n");
	printf("Verifying feasibility: SUCCESS\n");
	printf("\n");
	printf("or\n");
	printf("\n");
	printf("Verifying feasibility: FAILURE\n");	
	printf("\n");
	printf("The verbosity option +v can be used to obtain more information about the\n");
	printf("the option parsing process.\n");
	printf("\n");
	printf("To display the complete documentation about the application, run\n");
	printf("\n");
	printf("./determ_feasib -H\n");
	printf("or\n");
	printf("./determ_feasib -h\n");
	printf("\n");
	printf("See Section 4 below for examples of instructions when the program is run\n");
	printf("with command line instructions.\n");
	printf("\n");
	printf("2 Running the program through Docker instructions\n");
	printf("*************************************************\n");
	printf("\n");
	printf("An advantage of running the executable determ_feasib inside a Docker\n");
	printf("container is the possibility to avoid selection and installation of suitable\n");
	printf("C++ compiler, C++ make build tool and required C++ libraries. It suffices\n");
	printf("that the Docker engine be installed on the host where the computation will\n");
	printf("be run. For this, see https://docs.docker.com/engine/install/.\n");
	printf("\n");
	printf("Once the Docker engine is installed, follow these steps:\n");
	printf("\n");
	printf("a- Open a terminal and retrieve from the GitHub container registry the\n");
	printf("Docker image ghcr.io/larseeri/determ_feasib_image:latest as follows:\n");
	printf("\n");
	printf("docker pull ghcr.io/larseeri/determ_feasib_image:latest\n");
	printf("\n");
	printf("(From this image, Docker containers running the executable 'determ_feasib'\n");
	printf("will be generated.)\n");
	printf("b- For simplicity, define the alias 'determ_feasib_image' through\n");
	printf("\n");
	printf("docker tag ghcr.io/larseeri/determ_feasib_image:latest determ_feasib_image\n");
	printf("\n");
	printf("Remark:\n");
	printf("If the Docker image determ_feasib_image were instead available as the tar\n");
	printf("ball determ_feasib_image.tar, then it could be loaded through\n");
	printf("\n");
	printf("docker load --input determ_feasib_image.tar\n");
	printf("\n");	
	printf("c- If not already done, either (i) use Git to clone locally the remote\n");
	printf("repository https://github.com/larseeri/determ-MCFNDP-gener, or (ii)\n");
	printf("generate, download locally to an appropriate folder and uncompress a .zip\n");
	printf("archive containing the remote repository file tree. The /check_feasib/docker\n");
	printf("folder contains (i) the primitive files named 'Dockerfile' and 'makefile'\n");
	printf("based on which the Docker image was originally generated, (ii) a subfolder\n");
	printf("/inout used for exchanging files between the program running inside a Docker\n");	
	printf("container and the directory structure of the host, (iii) some auxiliary\n");
	printf("files, and (iv) the subfolder /examples containing a number of configuration\n");
	printf("files and their resulting outputs (examined in Section 5 below).\n");
	printf("d- In a terminal, go to the /check_feasib/docker folder just created.\n");	
	printf("\n");
	printf("Then, instructions similar to those appearing in Section 1.2 can be run from\n");
	printf("the terminal on AMD and Intel CPUs (other CPUs are addressed further below).\n");
	printf("\n");
	printf("In general, Docker instructions can be formulated from those appearing in\n");
	printf("Section 1.2 by (i) adding the prefix\n");
	printf("\n");
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image\n");
	printf("\n");
	printf("and by (ii) replacing the executable location ./determ_feasib with /determ_feasib\n");
	printf("(i.e. removing the initial dot), since the executable determ_feasib is located\n");
	printf("at the root of the Docker container being generated and since locations\n");
	printf("inside a Docker container must be accessed through absolute paths.\n");
	printf("\n");
	printf("Hence, the following command line instruction:\n");
	printf("\n");
	printf("./determ_feasib +v -F formatOfNetwork -I pathOfNetworkFile\n");
	printf("\n");
	printf("corresponds to the following Docker instruction:\n");
	printf("\n");
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F formatOfNetwork -I /inout/networkFile\n");
	printf("\n");
	printf("Important:\n");
	printf("When running the program through Docker instructions, all input and output\n");
	printf("files must be located inside the docker/inout subfolder. Within the Docker\n");
	printf("instructions and the configuration files, this subfolder must be denoted\n");
	printf("/inout since it is located at the root of the Docker container. Hence, a\n");
	printf("network file located at docker/inout/networkFile.std will be\n");
	printf("specified in the Docker instruction or inside the configuration file with\n");
	printf("-I /inout/networkFile.std.\n");
	printf("\n");
	printf("Important:\n");
	printf("The supplied Docker image conforms to the amd64 CPU instructions standard\n");
	printf("and will run natively on AMD and Intel CPUs. On CPUs conforming instead to\n");
	printf("the arm64 standard, e.g., on recent Apple computers, the supplied Docker\n");
	printf("image can be run through emulation. Essentially, this may be achieved as\n");
	printf("follows:\n");
	printf("a- Running the following preliminary instruction once. (For details, see\n");
	printf("https://hub.docker.com/r/tonistiigi/binfmt.)\n");
	printf("\n");
	printf("docker run --privileged --rm tonistiigi/binfmt --install all\n");
	printf("\n");
	printf("b- Adding the option --platform linux/amd64 to the Docker instruction line\n");
	printf("prefix as follows:\n");
	printf("\n");
	printf("docker run --platform linux/amd64 --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image\n");
	printf("\n");
	printf("The following characteristics are specific to the operation of the program\n");
	printf("through Docker instructions:\n");
	printf("- An empty folder absolutePathToDockerFolder/inout must already exist on\n");
	printf("the host for the Docker instructions to work correctly.\n");
	printf("- Option --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout\n");
	printf("connects subfolder docker/inout on the host to folder /inout\n");
	printf("of the Docker container. Hence, the connection between\n");
	printf("absolutePathToDockerFolder/inout on the host and folder /inout in the\n");
	printf("container is used to supply input files to and retrieve output files from\n");
	printf("the container.\n");
	printf("- Option --rm ensures suppression of the container implicitly generated by\n");
	printf("docker run. Note that the container created by docker run for running the\n");
	printf("executable determ_feasib stops running immediately after the latter finishes\n");
	printf("and that it cannot be reused afterwards, for instance through a docker exec\n");
	printf("instruction. Immediate suppression with --rm avoids uselessly cluttering\n");
	printf("Docker's container repository.\n");
	printf("\n");
	printf("The following Docker instructions may be useful:\n");
	printf("\n");
	printf("docker image ls (list all currently existing docker images)\n");
	printf("\n");
	printf("docker image rm --force imageName (forcefully remove imageName)\n");
	printf("\n");
	printf("docker container ls (list all running containers)\n");
	printf("\n");
	printf("docker container ls --all (list all containers, running or not)\n");
	printf("\n");
	printf("See Section 4 below for examples of configuration files and instructions\n");
	printf("when the program is run through Docker.\n");
	printf("\n");
	printf("3 Format of the deterministic MCFND instance file\n");
	printf("*************************************************\n");
	printf("\n");
	printf("By default, the name of the file describing the base deterministic MCFND\n");
	printf("network is './network.std' (Linux instructions),\n");
	printf("'./inout/network.std' (Docker instructions) and the default input format\n");
	printf("is STD. This name may be changed with option I and the input format can\n");
	printf("be indicated accordingly with option F.\n");
	printf("\n");
	printf("Important:\n");
	printf("It is critical that the content of the file describing the base\n");
	printf("deterministic MCFND instance and the indicated format of its content match\n");
	printf("together.\n");
	printf("\n");
	printf("DOW and STD formats are commonly used to store deterministic MCFND instances\n");
	printf("in text files. These formats are described in files DOW-format-desc.txt and\n");
	printf("STD-format-desc.txt. The application can read from these two formats. One of\n");
	printf("these three formats may be selected with option F.\n");
	printf("a- DOW presupposes that (i) for each arc, capacities and variable\n");
	printf("costs are identical for all commodities and (ii) each commodity has a\n");
	printf("single source node (at which volume > 0) and a single sink node (at which\n");
	printf("volume < 0).\n");
	printf("b- STD imposes neither (i), nor (ii).\n");
	printf("\n");
	printf("Example\n");
	printf("*******\n");
	printf("\n");
	printf("Consider this hypothetical Linux command line instruction:\n");
	printf("\n");
	printf("./determ_feasib -I instB.std -F G\n");
	printf("\n");
	printf("It is interpreted as follows:\n");
	printf("-I instB.std: deterministic network is to be read from file instB.std\n");
	printf("-F G: format of the latter is STD\n");
	printf("\n");
	printf("4 Detailed examples\n");
	printf("*******************\n");
	printf("\n");
	printf("The /check_feasib/program/examples subfolder contains examples of\n");
	printf("instruction lines running the executable of the program.\n");
	printf("\n");
	printf("Similarly, the /check_feasib/docker/examples subfolder holds examples of\n");
	printf("instruction lines occurring when the executable of the program is run\n");
	printf("through Docker instructions. These examples are identified by the addition\n");
	printf("of the prefix 'docker_'.\n");	
	printf("\n");
	printf("4.1 One source and one sink for each commodity\n");
	printf("**********************************************\n");
	printf("\n");
	printf("Folder /examples/oneSrcOneSnk/\n");
	printf("\n");
	printf("4.1.1 DOW format\n");
	printf("****************\n");
	printf("\n");
	printf("In the deterministic network, there is one source and one sink for each\n");
	printf("commodity and no differentiation of variable costs and capacities according\n");
	printf("to commodities. The network is received in the DOW format.\n");
	printf("\n");
	printf("4.1.1.1 Operation of the feasibility tester\n");
	printf("********************************************\n");
	printf("\n");
	printf("Linux command line instruction:\n");
	printf("\n");
	printf("./determ_feasib +v -F D -I determ_oneSrcOneSnk_12oct2026-17h09.dow\n");
	printf("\n");
	printf("Docker instruction:\n");
	printf("\n");
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F D -I /inout/determ_oneSrcOneSnk_12oct2026-17h09.dow\n");
	printf("\n");
	printf("Location of deterministic network file (input):\n");
	printf("/examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.dow\n");
	printf("\n");	
	printf("Displayed output:\n");
	printf("\n");
	printf("Verifying feasibility: SUCCESS\n");
	printf("\n");	
	printf("4.1.1.2 Origin of the deterministic network\n");
	printf("*******************************************\n");
	printf("\n");
	printf("The deterministic network was originally produced by the determistic\n");
	printf("generator as follows.\n");
	printf("\n");
	printf("Linux command line instruction:\n");
	printf("\n");
	printf("./determ_gen +v +F determ_config_oneSrcOneSnk_12oct2026-17h09.par\n");
	printf("\n");
	printf("Configuration file:\n");
	printf("determ_config_oneSrcOneSnk_12oct2026-17h09.par\n");
	printf("\n");
	printf("Outputs in DOW, STD, LP, MPS, GRA (basic graph), TEX (code of graphical\n");
	printf("representation), pdf (compiled image) formats:\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.dow\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.std\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.mps\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.lp\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.gra\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.tex\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.pdf\n");
	printf("\n");
	printf("4.1.2 STD format\n");
	printf("****************\n");
	printf("\n");
	printf("Similar to 4.1.1, except that the network is received in the STD format\n");
	printf("\n");
	printf("4.1.2.1 Operation of the feasibility tester\n");
	printf("********************************************\n");
	printf("\n");
	printf("Linux command line instruction:\n");
	printf("\n");
	printf("./determ_feasib +v -F G -I determ_oneSrcOneSnk_12oct2026-17h09.std\n");
	printf("\n");
	printf("Docker instruction:\n");
	printf("\n");
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F G -I /inout/determ_oneSrcOneSnk_12oct2026-17h09.std\n");
	printf("\n");
	printf("Location of deterministic network file (input):\n");
	printf("/examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.std\n");
	printf("n");
	printf("Displayed output:\n");
	printf("\n");
	printf("Verifying feasibility: SUCCESS\n");
	printf("\n");		
	printf("4.1.2.2 Origin the base deterministic network\n");
	printf("*********************************************\n");
	printf("\n");
	printf("Similar to 4.1.1.2.\n");
	printf("\n");
	printf("4.2 Multiple but equal sources and multiple but equal sinks\n");
	printf("***********************************************************\n");
	printf("\n");
	printf("Folder /examples/eqSrcsEqSnks/\n");
	printf("\n");
	printf("In the base network, there are multiple sources and multiple sinks for each\n");
	printf("commodity. However, sources and sinks are identical across commodities.\n");
	printf("There is differentiation of variable costs and capacities according to\n");
	printf("commodities. The base network must be received in the STD format (as the DOW\n");
	printf("format is unsuitable in this situation).\n");
	printf("\n");
	printf("4.2.1 Operation of the feasibility tester\n");
	printf("*****************************************\n");
	printf("\n");
	printf("Linux command line instruction:\n");
	printf("\n");
	printf("./determ_feasib +v -F G -I determ_eqSrcsEqSnks_12oct2026-17h09.std\n");
	printf("\n");
	printf("Docker instruction:\n");
	printf("\n");
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F G -I /inout/determ_eqSrcsEqSnks_12oct2026-17h09.std\n");
	printf("\n");
	printf("Location of deterministic network file (input):\n");
	printf("/examples/eqSrcsEqSnks/determ_eqSrcsEqSnks_12oct2026-17h09.std\n");
	printf("\n");
	printf("Displayed output:\n");
	printf("\n");
	printf("Verifying feasibility: SUCCESS\n");
	printf("\n");	
	printf("4.2.1.2 Origin of the base deterministic network\n");
	printf("************************************************\n");
	printf("\n");
	printf("The base deterministic network was originally produced by the determistic\n");
	printf("generator as follows.\n");
	printf("\n");
	printf("Linux command line instruction:\n");
	printf("\n");
	printf("./determ_gen +v +F determ_config_eqSrcsEqSnks_12oct2026-17h09.par\n");
	printf("\n");
	printf("Configuration file:\n");
	printf("determ_config_eqSrcsEqSnks_12oct2026-17h09.par\n");
	printf("\n");
	printf("Outputs in STD, LP, MPS, GRA (basic graph), TEX (code of graphical\n");
	printf("representation), pdf (compiled image) formats:\n");
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.std\n");
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.mps\n");
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.lp\n");
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.gra\n");
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.tex\n");
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.pdf\n");
	printf("\n");
    exit(1);
}

// unused
void Configuration::printUsageAndExit(char ExecName[], bool anonymous)
{
    if (!anonymous)
    {
        printf("\nUsage: %s [options]\n", ExecName);
    }

    printf("\nUSAGE FUNCTIONALITY IS UNIMPLEMENTED\n");
    exit(1);
}
