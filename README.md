(Oct 13th 2026)
https://github.com/larseeri/determ-MCFNDP-gener


## Generator of instances of the linear, deterministic, multi-commodity, capacitated, fixed charge network design problem

## README

This companion repository of Larsen et al. (2023) contains the GenDeterm generator of instances of the linear, deterministic, multi-commodity, capacitated, fixed charge network design problem (MCFNDP). 

### Content

The repository contains the following:

##### Generation

/USERGUIDE_DETERM.md: User guide detailing installation, operation, functionalities and usage of options of the generator.

/TUTOREXAMPLE_DETERM.md: Tutorial example about the use of the generator.

/program: Folder structure and files required to build and run the generator from the Linux command line.

/docker: Folder structure and files required to run the generator through a Docker container built from the supplied Docker image.

/AUTHORS.txt: Authors of the generator.

/LICENSE.txt: Statement of the license covering the generator.

##### Library

/library: Folder structure containing a generic library of deterministic MCFNDP instances generated with the GenDeterm generator.

/library/DESCLIBRARY_DETERM.txt: Detailed description of the library.

##### Testing feasibility

/check_feasib/USERGUIDE_FEASIB.md: User guide detailing installation, operation, functionalities and usage of options of the feasibility tester.

/check_feasib/TUTOREXAMPLE_FEASIB.md: Tutorial example about the use of the feasibility tester.

/check_feasib/program: Folder structure of the sub-application responsible for testing feasibility of generated deterministic MCFNDP instances.

/check_feasib/docker: Folder structure and files required to run the testing of feasibility through a Docker container built from the supplied Docker image.

### Learning

To become proficient with the use of the GenDeterm generator, we suggest to start with steps 1-2 and to iterate on steps 3-5:

1 Read Sections 1-2 of Larsen et al. (2023) attentively.\
2 Skim the user guide USERGUIDE_DETERM.md entirely to build a cognitive structure (know where to seek information, start locating the information related to the task at hand). The user guide details installation, operation, functionalities and available options of the generator.\
3 Study the tutorial example TUTOREXAMPLE_DETERM.md in detail.\
4 Study USERGUIDE_DETERM.md in detail.\
5 Launch generation experiments.

### Bibliography

Larsen, E., Bisaillon, S., Cordeau, J.F. and Frejinger, E. Pseudo-random Instance Generators in C++ for Deterministic and Stochastic Multi-commodity Network Design Problems. ArXiv e-prints arXiv:2312.10717, 2023.

