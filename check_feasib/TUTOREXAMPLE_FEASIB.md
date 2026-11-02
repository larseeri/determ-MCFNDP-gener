(November 1st 2026)


## Feasibility tester for instances of the linear, deterministic, multi-commodity, capacitated, fixed charge network design problem

## Tutorial example 


Using the feasibility tester involves steps A, B below.

### A- If not already done, install the program or create a Docker container from the supplied Docker image.

See Sections 1-2 of the online user guide USERGUIDE_FEASIB.md for a detailed discussion.


### B- Starting the verification process

See Sections 1.1-2 of the online user guide USERGUIDE_FEASIB.md for complete details.

For example, to launch a verification with the Linux command line of the instance /check_feasib/program/examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.dow stored in DOW format, the following instruction will be issued from the folder /check_feasib/program:

```
./determ_feasib +v -F D -I ./examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.dow

```

The program will display this message:

Verifying feasibility: SUCCESS

Remarks:

- Due to the very small number of options (F, I), the latter can only be set on the command line and not in a configuration file. +H displays the user guide.

- The script /check_feasib/program/check-feasibility_in-library-folders_18sept2026-21h15.sh can be adapted to automatize verifications of feasibility for a large number of generated instances. Notice that the indicated paths must be adapted to the existing folder structure and that the relevant branch(es) must be activated. The script sends all results to the file diags-comput.txt rather than to standard output. To pinpoint non-realizable instances, the token "FAILURE" is searched for in file diags-comput.txt.

- In its current form, the script  /check_feasib/program/check-feasibility_in-library-folders_18sept2026-21h15.sh  
is set up to verify the feasibility of every instance in the library. Notice, however, that /library supplies configuration files (rather than full instances) within two .zip archives: hubsAndSats_determ_config-files.zip and pointToPoint_determ_config-files.zip. These should be opened and the corresponding MCFNDP instances would have to be generated from the configuration files using the program contained in the /program folder before the script could be launched on instances of the library. See the tutorial example TUTOREXAMPLE_DETERM.md for additional details.