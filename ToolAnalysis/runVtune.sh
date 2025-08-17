echo "========================================================================="
echo "========================RUNNING GIRAFFE=================================="
echo "========================================================================="
vtune -collect hotspots -data-limit=10000 -r PipelineResults/Giraffe/GiraffeProfile ./Code/vg/bin/vg giraffe -t 56 -Z ./Data/chr20.giraffe.gbz -f ./Data/HG002_HiSeq30x_Chr20Randomized.fasta > PipelineResults/Giraffe/out.log

echo "========================================================================="
echo "========================RUNNING GRAPHALIGNER============================="
echo "========================================================================="
vtune -collect hotspots -data-limit=10000 -r PipelineResults/GraphAligner/GraphAlignerProfile ./Code/GraphAligner/bin/GraphAligner -g ./Data/chr20.gfa -t 56 -x vg -f ./Data/allLongReadsChr20Randomized.fasta -a PipelineResults/GraphAligner/out.gaf

echo "========================================================================="
echo "========================RUNNING VG MAP =================================="
echo "========================================================================="
vtune -collect hotspots -data-limit=10000 -r PipelineResults/VgMap/VgMapProfile ./Code/vg/bin/vg map -t 56 -d ./Data/chr20 -F ./Data/HG002_HiSeq30x_Chr20.fasta --gaf > PipelineResults/VgMapout.gaf

echo "========================================================================="
echo "========================RUNNING MINIGRAPH LR============================="
echo "========================================================================="
vtune -collect hotspots -data-limit=10000 -r PipelineResults/MinigraphLr/MinigraphProfileLongReads ./Code/minigraph/minigraph -o out -t 56 -cxlr ./Data/chr20.gfa ./Data/allLongReadsChr20Randomized.fasta 

#TODO add in the code to run the hotspots analysis for Minigrpah-Cr
echo "========================================================================="
echo "========================RUNNING MINIGRAPH CR============================="
echo "========================================================================="
vtune -collect hotspots -data-limit=10000 -r PipelineResults/MinigraphCr/MinigraphProfilePanChr20 ./minigraph/minigraph -o out -t 56 -cxasm ./Data/chr20.gfa ./Data/chm13Chrom20.fasta ./Data/chr20.pan.fasta
