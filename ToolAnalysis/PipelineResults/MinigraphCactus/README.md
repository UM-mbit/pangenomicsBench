Minigraph Cactus uses toil. Toil will report the runtime for each job in the
standard out.
###### Run graph alignment step.
In minigraph this step is divided into first building a low fidelity graph, and 
then aligning to it. The command divides work so the next step can be
parallelized. It's a small fration of the runtime on our system (>1\% of
alignment) The time of all these steps should be added to get the
total alignment time.
`../../Code/cactus/cactus_env/bin/cactus-minigraph ./jsS1 ../../Data/seqs.txt S1Out.gfa --reference chm13 | tee stage1Out.log`
`../../Code/cactus/cactus_env/bin/cactus-graphmap ./jsS2 ../../Data/seqs.txt S1Out.gfa S2Out.paf --outputFasta S1Out.gfa.fa --reference chm13`
`../../Code/cactus/cactus_env/bin/cactus-graphmap-split ./jsS3 ../../Data/seqs.txt S1Out.gfa ./S2Out.paf --outDir chroms --reference chm13`
###### Run graph induction step
`../../Code/cactus/cactus_env/bin/cactus-align ./jsS4 chroms/chromfile.txt chrom-alignments --batch --pangenome --reference chm13 --outVG`
##./../Code#### Run smoothing step
`../../Code/cactus/cactus_env/bin/cactus-graphmap-join ./jsS5 --vg chrom-alignments/*.vg --hal chrom-alignments/*.hal --outDir ./S5Out --outName S5-pg --reference chm13 --vcf --giraffe clip`
