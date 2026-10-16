(October 16th 2026)


## Feasibility tester for instances of the linear, deterministic, multi-commodity, capacitated, fixed charge network design problem

## User guide


This application verifies if a specified instance of the linear, deterministic, multi-commodity, capacitated, fixed charge network design problem (MCFNDP) is feasible. It does so by (i) opening all existing arcs in the network, (ii) setting the fixed cost over each arc to 0, (iii) setting the variable cost over each arc-commodity pair to 1, (iv) leaving unchanged demanded and supplied commodity volumes at every node as well as overall capacities and commodity-specific capacities, and then (v) verifying if a minimum cost solution is available under these conditions.

This user guide comprises the following sections:\
1 Building and running the program directly from the Linux command line.\
2 Running the program through Docker instructions.\
3 Format of the deterministic MCFND instance file.\
4 Detailed examples.

### 1 Building and running the program directly from the Linux command line

#### 1.1 Building the program from the Linux command line

Under Linux, follow these steps to build the program:

a- Install the CPLEX solver and/or the HIGHS solver (if not already done) and note the installation folder(s). According to our verifications, CPLEX and HIGHS work equally well in our application. HIGHS is freely available under the MIT license at https://highs.dev/. See Huangfu, Q., Hall, J.A.J. Parallelizing the dual revised simplex method. Math. Prog. Comp. 10, 119-142 (2018).

b- If not already done, either (i) use Git to clone locally the remote repository https://github.com/larseeri/determ-MCFNDP-gener, or (ii) generate, download locally to an appropriate folder and uncompress a .zip archive containing the remote repository file tree.

c- In a Linux terminal, go to the /check_feasib/program folder just created.

d- Edit the file named "makefile" to set the variables indicating the locations relevant to the CPLEX solver and/or the HIGHS solver, as noted in step a-. These locations are chiefly CPLEX_HOME and HIGHS_HOME. Save the modifications.

e- Make sure that the gcc C++ compiler and the make C++ build tool are available.

f- In the terminal, run ONLY ONE of these commands to build the program equipped either with the CPLEX solver or with the HIGHS solver:

```
make CPLEX_SOLVER=1
```
```
make HIGHS_SOLVER=1
```

An executable file named "determ_feasib" will be created inside the /check_feasib/program folder. Building with versions 11.5.0, 12.2.0, 13.2.0, 14.2.0 and 15.1.0 of the gcc compiler has been verified.

Remarks:
- To build once again with HIGHS if the program has already been built with CPLEX, run the following:

```
make clean CPLEX_SOLVER=1
make HIGHS_SOLVER=1
```

- To build once again with CPLEX if the program has already been built with HIGHS, run the following:

```
make clean HIGHS_SOLVER=1
make CPLEX_SOLVER=1
````

#### 1.2 Running the program with Linux command line instructions

The general command line for running the program from the /check_feasib/program folder just created is as follows:

```
./determ_feasib [+v] -F formatOfInstance -I pathOfInstanceFile
```

where the two available main options are

```
F <char>; format of file containing the deterministic MCFND network; D: DOW, G: STD; default: G
```
```
I <string>; path of file where the deterministic MCFND network is to be read; default value is './network.std'; (see further below about Docker instructions)
```

The result is displayed on standard output as follows:

Verifying feasibility: SUCCESS

or

Verifying feasibility: FAILURE

The verbosity option +v can be used to obtain more information about the the option parsing process.

To display the complete documentation about the application, run

```
./determ_feasib -H
```
or
```
./determ_feasib -h
```

See Section 4 below for examples of instructions when the program is run with command line instructions.

### 2 Running the program through Docker instructions

An advantage of running the executable determ_feasib inside a Docker container is the possibility to avoid selection and installation of suitable C++ compiler, C++ make build tool and required C++ libraries. It suffices that the Docker engine be installed on the host where the computation will be run. For this, see https://docs.docker.com/engine/install/.

Once the Docker engine is installed, follow these steps:

a- Open a terminal and retrieve from the GitHub container registry the Docker image ghcr.io/larseeri/determ_feasib_image:latest as follows:

```
docker pull ghcr.io/larseeri/determ_feasib_image:latest
```

(From this image, Docker containers running the executable 'determ_feasib' will be generated.)

b- For simplicity, define the alias 'determ_feasib_image' through

```
docker tag ghcr.io/larseeri/determ_feasib_image:latest determ_feasib_image
```

Remark:\
If the Docker image determ_feasib_image were instead available as the tar ball determ_feasib_image.tar, then it could be loaded through

```
docker load --input determ_feasib_image.tar
```

c- If not already done, either (i) use Git to clone locally the remote repository https://github.com/larseeri/determ-MCFNDP, or (ii) generate, download locally to an appropriate folder and uncompress a .zip archive containing the remote repository file tree. The /check_feasib/docker folder contains (i) the primitive files named 'Dockerfile' and 'makefile' based on which the Docker image was originally generated, (ii) a subfolder /inout used for exchanging files between the program running inside a Docker container and the directory structure of the host, (iii) some auxiliary files, and (iv) the subfolder /examples containing a number of configuration files and their resulting outputs (examined in Section 5 below).

d- In a terminal, go to the /check_feasib/docker folder just created.

Then, instructions similar to those appearing in Section 1.2 can be run from the terminal on AMD and Intel CPUs (other CPUs are addressed further below).

In general, Docker instructions can be formulated from those appearing in Section 1.2 by (i) adding the prefix 

```
docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image
```

and by (ii) replacing the executable location ./determ_feasib with /determ_feasib (i.e. removing the initial dot), since the executable determ_feasib is located at the root of the Docker container being generated and since locations inside a Docker container must be accessed through absolute paths.

Hence, the following command line instruction:

```
./determ_feasib +v -F formatOfNetwork -I pathOfNetworkFile
```

corresponds to the following Docker instruction:

```
docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F formatOfNetwork -I /inout/networkFile
```

Important:\
When running the program through Docker instructions, all input and output files must be located inside the docker/inout subfolder. Within the Docker instructions and the configuration files, this subfolder must be denoted /inout since it is located at the root of the Docker container. Hence, a network file located at docker/inout/networkFile.std will be specified in the Docker instruction or inside the configuration file with -I /inout/networkFile.std.

Important:\
The supplied Docker image conforms to the amd64 CPU instructions standard and will run natively on AMD and Intel CPUs. On CPUs conforming instead to the arm64 standard, e.g., on recent Apple computers, the supplied Docker image can be run through emulation. Essentially, this may be achieved as follows:

a- Running the following preliminary instruction once. (For details, see https://hub.docker.com/r/tonistiigi/binfmt.)

```
docker run --privileged --rm tonistiigi/binfmt --install all 
```

b- Adding the option --platform linux/amd64 to the Docker instruction line prefix as follows:

```
docker run --platform linux/amd64 --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image
```

The following characteristics are specific to the operation of the program through Docker instructions:
- An empty folder absolutePathToDockerFolder/inout must already exist on the host for the Docker instructions to work correctly.
- Option --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout connects subfolder docker/inout on the host to folder /inout of the Docker container. Hence, the connection between absolutePathToDockerFolder/inout on the host and folder /inout in the container is used to supply input files to and retrieve output files from the container.
- Option --rm ensures suppression of the container implicitly generated by docker run. Note that the container created by docker run for running the executable determ_feasib stops running immediately after the latter finishes and that it cannot be reused afterwards, for instance through a docker exec instruction. Immediate suppression with --rm avoids uselessly cluttering Docker's container repository.

The following Docker instructions may be useful:

```
docker image ls (list all currently existing docker images)
```
```
docker image rm --force imageName (forcefully remove imageName)
```
```
docker container ls (list all running containers)
```
```
docker container ls --all (list all containers, running or not)
```

See Section 4 below for examples of configuration files and instructions when the program is run through Docker.

### 3 Format of the deterministic MCFND instance file

By default, the name of the file describing the base deterministic MCFND network is './network.std' (Linux instructions), './inout/network.std' (Docker instructions) and the default input format is STD. This name may be changed with option I and the input format can be indicated accordingly with option F.

Important:
It is critical that the content of the file describing the base deterministic MCFND instance and the indicated format of its content match together.

DOW and STD formats are commonly used to store deterministic MCFND instances in text files. These formats are described in files DOW-format-desc.txt and STD-format-desc.txt. The application can read from these two formats. One of these three formats may be selected with option F.\
a- DOW presupposes that (i) for each arc, capacities and variable costs are identical for all commodities and (ii) each commodity has a single source node (at which volume > 0) and a single sink node (at which volume < 0).\
b- STD imposes neither (i), nor (ii).

Example:\
Consider this hypothetical Linux command line instruction:

```
./determ_feasib -I instB.std -F G 
```

It is interpreted as follows:\
-I instB.std: deterministic network is to be read from file instB.std\
-F G: format of the latter is STD

### 4 Detailed examples

The /check_feasib/program/examples subfolder contains examples of instruction lines running the executable of the program.

Similarly, the /check_feasib/docker/examples subfolder holds examples of instruction lines occurring when the executable of the program is run through Docker instructions. These examples are identified by the addition of the prefix 'docker_'.

#### 4.1 One source and one sink for each commodity

Folder /examples/oneSrcOneSnk/

##### 4.1.1 DOW format

In the deterministic network, there is one source and one sink for each commodity and no differentiation of variable costs and capacities according to commodities. The network is received in the DOW format.

###### 4.1.1.1 Operation of the feasibility tester

Linux command line instruction:

```
./determ_feasib +v -F D -I determ_oneSrcOneSnk_12oct2026-17h09.dow
```

Docker instruction:

```
docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F D -I /inout/determ_oneSrcOneSnk_12oct2026-17h09.dow
```

Location of deterministic network file (input):\
/examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.dow

Displayed output:

Verifying feasibility: SUCCESS

###### 4.1.1.2 Origin of the deterministic network

The deterministic network was originally produced by the determistic generator as follows.

Linux command line instruction:

```
./determ_gen +v +F determ_config_oneSrcOneSnk_12oct2026-17h09.par
```

Configuration file:\
determ_config_oneSrcOneSnk_12oct2026-17h09.par

Outputs in DOW, STD, LP, MPS, GRA (basic graph), TEX (code of graphical representation), pdf (compiled image) formats:\
determ_oneSrcOneSnk_12oct2026-17h09.dow\
determ_oneSrcOneSnk_12oct2026-17h09.std\
determ_oneSrcOneSnk_12oct2026-17h09.mps\
determ_oneSrcOneSnk_12oct2026-17h09.lp\
determ_oneSrcOneSnk_12oct2026-17h09.gra\
determ_oneSrcOneSnk_12oct2026-17h09.tex\
determ_oneSrcOneSnk_12oct2026-17h09.pdf

##### 4.1.2 STD format

Similar to 4.1.1, except that the network is received in the STD format

###### 4.1.2.1 Operation of the feasibility tester

Linux command line instruction:

```
./determ_feasib +v -F G -I determ_oneSrcOneSnk_12oct2026-17h09.std
```

Docker instruction:

```
docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F G -I /inout/determ_oneSrcOneSnk_12oct2026-17h09.std
```

Location of deterministic network file (input):\
/examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.std

Displayed output:

Verifying feasibility: SUCCESS

###### 4.1.2.2 Origin the base deterministic network

Similar to 4.1.1.2.

#### 4.2 Multiple but equal sources and multiple but equal sinks

Folder /examples/eqSrcsEqSnks/

In the base network, there are multiple sources and multiple sinks for each commodity. However, sources and sinks are identical across commodities. There is differentiation of variable costs and capacities according to commodities. The base network must be received in the STD format (as the DOW format is unsuitable in this situation).

##### 4.2.1 Operation of the feasibility tester

Linux command line instruction:

```
./determ_feasib +v -F G -I determ_eqSrcsEqSnks_12oct2026-17h09.std
```

Docker instruction:

```
docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_feasib_image /determ_feasib +v -F G -I /inout/determ_eqSrcsEqSnks_12oct2026-17h09.std
```

Location of deterministic network file (input):\
/examples/eqSrcsEqSnks/determ_eqSrcsEqSnks_12oct2026-17h09.std

Displayed output:

Verifying feasibility: SUCCESS

###### 4.2.1.2 Origin of the base deterministic network

The base deterministic network was originally produced by the determistic generator as follows.

Linux command line instruction:

```
./determ_gen +v +F determ_config_eqSrcsEqSnks_12oct2026-17h09.par
```

Configuration file:\
determ_config_eqSrcsEqSnks_12oct2026-17h09.par

Outputs in STD, LP, MPS, GRA (basic graph), TEX (code of graphical representation), pdf (compiled image) formats:\
determ_eqSrcsEqSnks_12oct2026-17h09.std\
determ_eqSrcsEqSnks_12oct2026-17h09.mps\
determ_eqSrcsEqSnks_12oct2026-17h09.lp\
determ_eqSrcsEqSnks_12oct2026-17h09.gra\
determ_eqSrcsEqSnks_12oct2026-17h09.tex\
determ_eqSrcsEqSnks_12oct2026-17h09.pdf
