October 13th 2026
https://github.com/larseeri/determ-MCFNDP-gener


## Generator of instances of the linear, deterministic, multi-commodity, capacitated, fixed charge network design problem

## Tutorial example 


To become proficient with the use of the GenDeterm generator, we suggest to start with steps 1-2 and to iterate on steps 3-5:

1 Read Sections 1-2 of Larsen et al. (2023) attentively.\
2 Skim the user guide USERGUIDE_DETERM.md entirely to build a cognitive structure (know where to seek information, start locating the information related to the task at hand). The user guide details installation, operation, functionalities and available options of the generator.\
3 Study this tutorial example in detail.\
4 Study USERGUIDE_DETERM.md in detail.\
5 Launch generation experiments.


Using the generator involves steps A, B, C below.

### A- If not already done, install the program or create a Docker container from the supplied Docker image.

See Section 2 of the online user guide for complete details.


### B- Build or modify a configuration file.

As an example, this configuration file is taken from the deterministic library:

library/hubsAndSats/eqSrcsEqSnks/flat/hs_vols-20-100_csts-20-100-20-100-20-100-20-100_eqSrcsEqSnks_secTop-flat_secTopSpe-2-3-2-3-2-3-2-8-2-8-12-15-45-10-15-2-15-45-10-15-2_noPar-20_adjSpe-0~33-1~00-0~33-1~00_seed-50163844_strm-5343.par

By thoroughly understanding this example and experimenting with alternative and/or additional settings, the user will gain sufficient knowledge to use the full extent of the available functionalities in new experiments.


#### 1 Content of the configuration file

We reordered the content of the original file to make the exposition flow more progressively.

*** Content of the configuration file starts on the next line. ***

// file name:\ hs_vols-20-100_csts-20-100-20-100-20-100-20-100_eqSrcsEqSnks_secTop-flat_secTopSpe-2-3-2-3-2-3-2-8-2-8-12-15-45-10-15-2-15-45-10-15-2_noPar-20_adjSpe-0~33-1~00-0~33-1~00_seed-50163844_strm-5343.par

seed 50163844\
stream 5343

hubsAndSats true

numbCom 12\
minComVol 20\
maxComVol 100\
eqSrcsEqSnks true\
minNbSrc 2\
maxNbSrc 3\
minNbSnk 2\
maxNbSnk 3\
secTopology 3\
lengthX 2\
lengthY 3\
minFixCos 20\
maxFixCos 100\
minVarCos 20\
maxVarCos 100\
minArcCap 15\
maxArcCap 45\
minComCap 10\
maxComCap 15\
nbAddArcPerSampHead 2\
noParalArc 20\
adjCapacsDown 0.330000\
adjFixCostsUp 1.000000

minSatsPerHub 2\
maxSatsPerHub 8\
minSatsRoundSize 2\
maxSatsRoundSize 8\
minSatFixCos 20\
maxSatFixCos 100\
minSatVarCos 20\
maxSatVarCos 100\
minSatArcCap 15\
maxSatArcCap 45\
minSatComCap 10\
maxSatComCap 15\
nbAddSatArcPerSampHead 2\
adjSatCapacsDown 0.330000\
adjSatFixCostsUp 1.000000

shortOutFileName\ hs_vols-20-100_csts-20-100-20-100-20-100-20-100_eqSrcsEqSnks_secTop-flat_secTopSpe-2-3-2-3-2-3-2-8-2-8-12-15-45-10-15-2-15-45-10-15-2_noPar-20_adjSpe-0~33-1~00-0~33-1~00_seed-50163844_strm-5343

*** Content of the configuration file ends at the preceding line. ***


#### 2 Detailed explanations

We explain each statement and comment appearing in the example.

Remarks:

a- To supply an option, a line of a configuration file must begin with a dash '-' or a letter. A line beginning with any other character will be considered a comment. It is advisable to start comment lines exclusively with "//" or with "#". Blank lines are ignored. See Section 4.2 of the online user guide for complete details.

b- The order of the statements and comments in a configuration file is immaterial to the workings of the generator.

c- Any boolean option that does not appear in the configuration file or in the command line is interpreted as having the boolean value false. To set a boolean option to true, it is sufficient to state it without any value in the configuration file or in the command line or to state it with the boolean value true in the confiration file (as done here).

d- The selected option values must conform to the restrictions stated in Section 4.3 of the online user guide. Otherwise, the program will notify the user of any error and stop.

e- In this section, the suffixes appearing in the names of the option values indicate the expected type. For instance, option seed expects an integer. Hence the value is named seed_int. 

**2.1** While not absolutely necessary, it is advisable to state the name of the current configuration file as a comment within the configuration file and to make sure that this name is a unique identifier of the content.

// file name: name-of-configuration-file

**2.2** seed_int and stream_int stand for the two integers that are assigned to the seed and stream of the pseudo-random generator. To ensure reproducibility, they should always be set. When several configurations are included in a set, the pair (seed_int, stream_int) should be selected for each configuration so as to constitute a unique identifier. A practical choice is to make seed_int invariant within the set of configurations whereas stream_int varies between the configurations in an interpretable way. 

```
seed seed_int
```

```
stream stream_int
```

**2.3** Setting hubsAndSats_bool to true (either explicitly or implicitly by omitting hubsAndSats_bool), as is the case here, means that the hubs-and-satellites primary topology is to be implemented. If setting to false explicitly or implicitly (by omitting this option entirely), then the point-to-point primary toplogy is implemented. See Section 1 of the online user guide for details.

```
hubsAndSats hubsAndSats_bool
```

**2.4** Number of commodities is equal to numbCom_int. Here, numbCom_int==12. 

```
numbCom numbCom_int
```

**2.5** Each commodity volume will be generated pseudo-randomly according to a uniform(minComVol_int, maxComVol_int) distribution. Here, minComVol_int == 20, maxComVol_int == 100. (See note 1 below.)

```
minComVol minComVol_int
maxComVol maxComVol_int
```

**2.6** Setting eqSrcsEqSnks_bool to true explicitly or implicitly (by omitting eqSrcsEqSnks_bool), as is the case here, means that commodity volumes will have common source nodes over all commodities and common sink nodes over all commodities; the common sources and common sinks are sampled pseudo-randomly according to uniform(minNbSrc_int, maxNbSrc_int) and uniform(minNbSnk_int, maxNbSnk_int) distributions, respectively. Here, minNbSrc_int==2, maxNbSrc_int==3, minNbSnk_int==2, maxNbSnk_int==3. Setting instead the value of option oneSrcOneSnk to true explicitly or implicitly (by omitting oneSrcOneSnk_bool) would have meant that there would be a single source and a single sink for each commodity. In this case, minNbSrc_int, maxNbSrc_int, minNbSnk_int, maxNbSnk_int would have been neglected. Of course, values of options eqSrcsEqSnks and oneSrcOneSnk cannot both be set to true.

```
eqSrcsEqSnks eqSrcsEqSnks_bool
```

```
minNbSrc minNbSrc_int
maxNbSrc maxNbSrc_int
```

```
minNbSnk minNbSnk_int
maxNbSnk maxNbSnk_int
```

**2.7** Secondary topology will be that specified with secTopology_int (equal to 3 here, that is a grid on a flat surface). See Section 1 of online user guide.

```
secTopology secTopology_int
```

**2.8** The horizontal and vertical numbers of hub nodes (recall that the primary topology is hubs-and-satellites) on the torus will be respectively lengthX_int, lengthY_int (equal to 2 and 3 here). The number of hub nodes will automatically be inferred as 6 = 2 * 3.

```
lengthX lengthX_int
lengthY lengthY_int
```

**2.9** Non-random hub arcs initially created to build a strongly connected graph between hub nodes will be assigned a fixed cost equal to maxFixCos_int. The additional, random hub arcs that will eventually be created will have a fixed cost sampled pseudo-randomly according to uniform(minFixCos_int, maxFixCos_int). Here, minFixCos_int==20, maxFixCos_int==100. (See note 1 below.)

```
minFixCos minFixCos_int
maxFixCos maxFixCos_int
```

**2.10 Non-random hub arcs initially created to build a strongly connected graph between hub nodes will be assigned a variable cost common to all commodities equal to maxVarCos_int. Additional, random hub arcs that will eventually be created will have commodity-specific costs sampled pseudo-randomly according to uniform(minVarCos_int, maxVarCos_int). Here, minVarCos_int==20, maxVarCos_int==100. (See note 1 below.)

```
minVarCos minVarCos_int
maxVarCos maxVarCos_int
```

**2.11 Non-random hub arcs initially created to build a strongly connected graph between hub nodes will be assigned an overall capacity equal to total volume over all commodities. Additional, random hub arcs that will eventually be created will have overall capacity sampled pseudo-randomly according to uniform(minArcCap_int, maxArcCap_int). Here, minArcCap_int==15, maxArcCap_int==45. (See note 1 below.)

```
minArcCap minArcCap_int
maxArcCap maxArcCap_int
```

**2.12 Non-random hub arcs initially created to build a strongly connected graph between hub nodes will be assigned commodity-specific capacities equal to commodity volume. Additional, random hub arcs that will entually be created will have commodity-specific capacities sampled pseudo-randomly according to uniform(minComCap_int, maxComCap_int). Here, minComCap_int==10, maxComCap_int==15. (See note 1 below.)

```
minComCap minComCap_int
maxComCap maxComCap_int
```

**2.13 From each hub node, nbAddArcPerSampHead_int arcs towards other hub nodes will be added randomly. Here, nbAddArcPerSampHead_int==2 (i.e., only the heads of the random arcs are sampled). These arcs will be specified according to 2.9-2.12. Notice that the addition of random arcs whose head and tail would be both sampled is also possible. In this case the appropriate option would be nbAddArcSampTailHead. 

```
nbAddArcPerSampHead nbAddArcPerSampHead_int
```

**2.14 While generating random arcs (between any pair of nodes), up to noParalArc_int attempts (equal to 20 here) will be made to sample an arc that is not paralleling an existing arc. If noParalArc_int attempts are unsuccessful, the current search for a non-parallel arc will be interrupted. 

```
noParalArc noParalArc_int
```

**2.15 (Unused in the current example) Although the current example does not make use of this additional functionality, it would be possible to exert a direct, deterministic control over the properties of the random hub arcs added so as to alter the path followed by the optimal solution over these arcs as well as the complexity of the problem. Thus, the fixed cost of a percentage of the random hub arcs could be set to zero, a percentage of the random hub arcs could be set to full overall capacity, one commodity-specific capacity could be set to zero in a percentage of random hub arcs or one commodity-specific capacity could be set to full in a percentage of random hub arcs. These four adjustments are respectively controlled with the options ratZeroFix, ratTotVolCap, ratZeroComCap and ratMaxComCap.

**2.16 Normalize overall and commodity-specific capacities over all hub arcs (non-random and random) by uniform adjustment equal to adjCapacsDown_double (downward if adjCapacsDown_double >= 1, upward otherwise). Specifying a value of 1 balances the sum of total commodity volumes flowing on all hub arcs with the sum of all hub arc capacities. It might be viewed as a neutral setting. Here, adjCapacsDown_double==0.330000. (See note 2 below.)

```
adjCapacsDown adjCapacsDown_double
```

**2.17 Normalize fixed costs over all hub arcs (non-random and random) by uniform adjustment equal to adjFixCostsUp_double (downward if adjFixCostsUp_double <= 1, upward otherwise). Specifying a value of 1 balances the sum of fixed costs over all hub arcs with the total variable cost that would be incurred by circulating average commodity volume on all hub arcs. Here, adjFixCostsUp_double==1. (See note 2 below.)

```
adjFixCostsUp adjFixCostsUp_double
```

**2.18 (Unused in the current example) Although the current example does not make use of these additional functionalities, it would be possible to uniformize the commodity-specific capacities and/or variable costs over all commodities for each hub arc. This would be controlled with the options equalComCaps and equalVarCosts, respectively.

**2.19 The number of satellite nodes assigned to a hub node will be sampled pseudo-randomly according to uniform(minSatsPerHub_int, maxSatsPerHub_int). Here, minSatsPerHub_int==2 and maxSatsPerHub_int==8
```
minSatsPerHub minSatsPerHub_int
maxSatsPerHub maxSatsPerHub_int
```

**2.20 The number of satellite nodes tied sequentially in a round will be sampled pseudo-randomly according to uniform(minSatsRoundSize_int, maxSatsRoundSize_int). Here, minSatsRoundSize_int==2, maxSatsRoundSize_int==8. 

```
minSatsRoundSize minSatsRoundSize_int
maxSatsRoundSize maxSatsRoundSize_int
```

**2.21 Non-random satellite arcs initially created to build a strongly connected graph will be assigned a fixed cost equal to maxSatFixCos_int. Additional, random satellite arcs that will eventually be created will have a fixed cost sampled pseudo-randomly according to uniform(minSatFixCos_int, maxSatFixCos_int). Here, minSatFixCos_int==20, maxSatFixCos_int==100. (See note 1 below.)

```
minSatFixCos minSatFixCos_int
maxSatFixCos maxSatFixCos_int
```

**2.22 Non-random satellite arcs initially created to build a strongly connected graph will be assigned a variable cost common to all commodities equal to maxSatVarCos_int. Additional, random satellite arcs that will eventually be created will have commodity-specific costs sampled pseudo-randomly according to uniform(minSatVarCos_int, maxSatVarCos_int). Here, minSatVarCos_int==20, maxSatVarCos_int==100. (See note 1 below.)

```
minSatVarCos minSatVarCos_int
maxSatVarCos maxSatVarCos_int
```

**2.23 Non-random satellite arcs initially created to build a strongly connected graph will be assigned an overall capacity equal to total volume over all commoditie. Additional, random satellite arcs that will eventually be created will have overall capacity sampled pseudo-randomly according to uniform(minSatArcCap_int, maxSatArcCap_int). Here, minSatArcCap_int==15, maxSatArcCap_int==45.

```
minSatArcCap minSatArcCap_int
maxSatArcCap maxSatArcCap_int
```

**2.24 Non-random satellite arcs initially created to build a strongly connected graph will be assigned commodity-specific capacities equal to commodity volume. Additional, random satellite arcs that will eventually be created will have commodity-specific capacities sampled pseudo-randomly according to uniform(minSatComCap_int, maxSatComCap_int). Here, minSatComCap_int==10, maxSatComCap_int==15.

```
minSatComCap minSatComCap_int
maxSatComCap maxSatComCap_int
```

**2.25 From each satellite node, nbAddSatArcPerSampHead_int arcs towards any other node nodes will be added randomly (i.e., only the heads of the random arcs are sampled). Here, nbAddSatArcPerSampHead==2. These arcs will be specified according to 2.21-2.24. Notice that the addition of random arcs whose head and tail would be both sampled is also possible. In this case the appropriate option would be nbAddSatArcSampTailHead. 

```
nbAddSatArcPerSampHead 2
```

**2.26 (Unused in the current example) Although the current example does not make use of this additional functionality, it would be possible to exert a direct, deterministic control over the properties of the random satellite arcs added so as to alter the path followed by the optimal solution over these arcs as well as the complexity of the problem. The motivation is to Thus, the fixed cost of a percentage of the random satellite arcs could be set to zero, a percentage of the random satellite arcs could be set to full overall capacity, one commodity-specific capacity could be set to zero in a percentage of random satellite arcs or one commodity-specific capacity could be set to full in a percentage of random satellite arcs. These four adjustments are respectively controlled with the options ratSatZeroFix, ratSatTotVolCap, ratSatZeroComCap and ratSatMaxComCap.

**2.27 Normalize overall and commodity-specific capacities over all satellite arcs (non-random and random) by uniform adjustment equal to adjSatCapacsDown_double (downward if adjSatCapacsDown_double >= 1, upward otherwise). Specifying a value of 1 balances the sum of total commodity volumes flowing on all arcs with the sum of all arc capacities. It might be viewed as a neutral setting. Here, adjSatCapacsDown_double==0.330000. (See note 2 below.)

```
adjSatCapacsDown adjSatCapacsDown_double
```

**2.28 Normalize fixed costs over all satellite arcs (non-random and random) by uniform adjustment equal to adjSatFixCostsUp_double (downward if adjSatFixCostsUp_double <= 1, upward otherwise). Specifying a value of 1 balances the sum of fixed costs over all arcs with the total variable cost that would be incurred by circulating average commodity volume on all arcs. Here, adjSatFixCostsUp_double==1. (See note 2 below.)

```
adjSatFixCostsUp adjSatFixCostsUp_double
```

**2.29 (Unused in the current example) Although the current example does not make use of these additional functionalities, it would be possible to uniformize the commodity-specific capacities and/or variable costs over all commodities for each satellite arc. This would be controlled with the options equalSatComCaps and equalSatVarCosts, respectively.

**2.30 Path of output file, excluding any extension name. The program will save all output files using this path and will add the relevant extensions according to their exact nature. Unless the option noStdOut is specified, the output is automatically saved in STD format and therefore will have the ".std" extension. Here, shortOutFileName_string==hs_vols-20-100_csts-20-100-20-100-20-100-20-100_eqSrcsEqSnks_secTop-flat_secTopSpe-2-3-2-3-2-3-2-8-2-8-12-15-45-10-15-2-15-45-10-15-2_noPar-20_adjSpe-0~33-1~00-0~33-1~00_seed-50163844_strm-5343

```
shortOutFileName shortOutFileName_string
```

**2.31 (Unused in the current example) Provided the generated MCFNDP instance satisfies a number of restrictive conditions, it will also be saved automatically in the simpler DOW format. Saving the output additionally under other formats can be requested: option mpsOut specifies the machine-readable MPS format, option lpOut specifies a human-readable statement of the MCFNDP, option texOut specifies generating and saving a LaTeX code representation. Output files of every format are saved under the file name specified with option shortOutFileName. The particular extension (std, dow, mps, lp, tex) denotes the exact content. See the user guide for complete details.

Note 1: When generating a set of several instances with the intent of creating diversification, it is desirable that the ratio of the maximum value to the minimum value be large.

Note 2: These options are critical instruments for controlling and diversifying the relative tightness of commodity volumes vs capacities and fixed vs variable costs. Section 2.2.2 of the article discusses their role as well as suggestions for adjusting their values. 


### C- Starting the generation process

See Section 2 of the online user guide for a detailed discussion.

For example, to launch the generation process from the Linux command line based on the configuration file described in Section B above, the following instruction would be issued from the folder GenDeterm/program:

```
./determ_gen +v +F ../library/hubsAndSats/eqSrcsEqSnks/flat/hs_vols-20-100_csts-20-100-20-100-20-100-20-100_eqSrcsEqSnks_secTop-flat_secTopSpe-2-3-2-3-2-3-2-8-2-8-12-15-45-10-15-2-15-45-10-15-2_noPar-20_adjSpe-0~33-1~00-0~33-1~00_seed-50163844_strm-5343.par
```

Notice that options can also be set on the command line. Furthermore, the value of an option specified on the command line supersedes that specified in the configuration file.
