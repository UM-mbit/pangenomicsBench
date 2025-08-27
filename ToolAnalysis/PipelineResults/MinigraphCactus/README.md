Minigraph Cactus uses toil. Toil will report the runtime for each job in the
standard out. If you installed minigraph-cactus in a virtual environment (most
installations) make sure the environment is active. 

Note that seq.txt contains relative paths that should work if you run these
commands from this directory i.e. 
`/ToolAnalysis/PipelineResults/MinigraphCactus`. If this doesn't work, the paths
can be replaced with absolute paths in the data directory. 
`Data/MinigraphCactusData/seqs.txt`
###### Run graph alignment step.
In minigraph this step is divided into first building a low fidelity graph, and 
then aligning to it. The command divides work so the next step can be
parallelized. It's a small fration of the runtime on our system (>1\% of
alignment) The time of all these steps should be added to get the
total alignment time.
`cactus-minigraph ./jsS1 ../../Data/MinigraphCactusData/seqs.txt S1Out.gfa --reference chm13 | tee stage1Out.log`
`cactus-graphmap ./jsS2 ../../Data/MinigraphCactusData/seqs.txt S1Out.gfa S2Out.paf --outputFasta S1Out.gfa.fa --reference chm13`
`cactus-graphmap-split ./jsS3 ../../Data/MinigraphCactusData/seqs.txt S1Out.gfa ./S2Out.paf --outDir chroms --reference chm13`
###### Run graph induction step
`cactus-align ./jsS4 chroms/chromfile.txt chrom-alignments --batch --pangenome --reference chm13 --outVG`
###### Run smoothing step
`cactus-graphmap-join ./jsS5 --vg chrom-alignments/*.vg --hal chrom-alignments/*.hal --outDir ./S5Out --outName S5-pg --reference chm13 --vcf --giraffe clip`

###### Layout
To run odgi layout, first a few steps are required to prepare the graph. We
only include the timing of the last step which does the actual layout
1. convert gfa 1.1 to gfa 1.0.
   `vg convert -W -H -f  S5-pg.gfa  > vgConverted.gfa`
2. make odgi graph (12s)                                                             
   `odgi build -g vgConverted.gfa -o vgConvert.og`
3. sort the graph
   `odgi sort -O -i vgConvert.og -o vgConvert.opt.og`
4. run odgi layout. This is the step we time.
  `/usr/bin/time -o layoutTime.txt odgi layout -i vgConvert.opt.og -o out.lay -t 56 
