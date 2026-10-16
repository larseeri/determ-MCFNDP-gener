October 13th 2026
https://github.com/larseeri/determ-MCFNDP-gener
check_feasib/TUTOREXAMPLE_FEASIB.md


## Feasibility tester for instances of the linear, deterministic, multi-commodity, capacitated, fixed charge network design problem

## Turorial example 


Using the feasibility tester involves steps A, B below.

### A- If not already done, install the program or create a Docker container from the supplied Docker image.

See Sections 1-2 of the online user guide USERGUIDE_FEASIB.md for a detailed discussion.


### B- Starting the verification process

See Sections 1.1-2 of the online user guide USERGUIDE_FEASIB.md for complete details.

For example, to launch a verification with the Linux command line of the instance /check_feasib/examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.dow stored in DOW format, the following instruction will be issued from the folder /check_feasib:

```
./determ_feasib +v -F D -I ./examples/oneSrcOneSnk/determ_oneSrcOneSnk_12oct2026-17h09.dow

```

The program will display this message:

Verifying feasibility: SUCCESS

Remarks:

- Due to the very small number of options (F, I, H or h), the latter can only be set on the command line and not in a configuration file.

- The script /check_feasib/check-feasibility_in-library-folders_18sept2026-21h15.sh can be adapted to automatize verifications of feasibility for a large number of generated instances. The script sends all results to the file diags-comput.txt rather than to standard output. To pinpoint non-realizable instances, the token "FAILURE" is searched for in file diags-comput.txt.  
