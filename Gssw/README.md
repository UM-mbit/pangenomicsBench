# Generating Inputs
Run the command
`vg map -d <path to chr20 + header with gcsa, pg (or vg), and gcsa.lcp files> 
-t 1 -F <path to reads.fa>`
An example
`vg map -d graphDir/chr20.hprc-v1.0-pggb -t 1 -F reads.fa`
The GraphDir directory should contain chr20.hprc-v1.0-pggb.pg, 
chr20.hprc-v1.0-pggb.gcsa, and chr20.hprc-v1.0-pggb.gcsa.lcp.

# Understanding Outputs
Each run of the kernel generates one line in the output file. The output is not
json. Each line has the format
```
<TODO>@<TODO>:<GraphNode>[<CigarStringThroughGraphNodeSequence>]<GraphNode>[<CigarStringThroughGraphNodeSequence]...
```

TODO gotta update this when we hear from Dev.

