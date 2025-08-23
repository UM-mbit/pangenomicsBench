#!/usr/bin/bash
set -e
THREAD_NOS=(56 28 14 4)

for THREAD_NO in "${THREAD_NOS[@]}"; do
    echo "running seqwish" 
    /usr/bin/time --verbose -o "ThreadScaling/Timing/seqwishTime_${THREAD_NO}Threads.txt" Code/seqwish/bin/seqwish -s Data/chr20.pan.fasta -p Data/chr20.paf -k 23 -f 0 -g "seqishout_chr20_.gfa" -B 10M -t ${THREAD_NO} -P 2>&1 | tee "ThreadScaling/Logs/seqwish${THREAD_NO}.log"
    echo "running pgsgd"
    /usr/bin/time --verbose -o "ThreadScaling/Timing/pgsgdTime_${THREAD_NO}Threads.txt" Code/odgi/bin/odgi layout --gpu -i Data/chr20_smooth.og -o "out.chr2.og.lay" -T out.layout.odgi.tsv -t ${THREAD_NO} -P 2>&1 | tee -a "ThreadScaling/Logs/pgsgd${THREAD_NO}.log"
    echo "running graphAligner"
    /usr/bin/time --verbose -o "ThreadScaling/Timing/graphAlignerTime_${THREAD_NO}Threads.txt" Code/GraphAligner/bin/GraphAligner -g Data/chr20.gfa -t $THREAD_NO -x vg -f Data/allLongReadsChr20Randomized.fasta -a out.gaf > "ThreadSCaling/Logs/graphAligner${THREAD_NO}.log"
    echo "running giraffe"
    /usr/bin/time --verbose -o "ThreadScaling/Timing/giraffeTime_${THREAD_NO}Threads.txt" Code/vg/bin/vg giraffe -t $THREAD_NO -Z Data/chr20.giraffe.gbz -f Data/HG002_HiSeq30x_Chr20Randomized.fasta > "ThreadScaling/Logs/giraffe${THREAD_NO}.log"
    echo "running minigraph CR"
    /usr/bin/time --verbose -o "ThreadScaling/Timing/minigraphTime_${THREAD_NO}Threads.txt" Code/minigraph/minigraph -t $THREAD_NO -cxasm Data/chr20.gfa Data/chm13Chrom20.fasta > "ThreadScaling/Logs/minigraph${THREAD_NO}.log"
    echo "running minigraph long reads"
    /usr/bin/time --verbose -o "ThreadScaling/Timing/minigraphLongReadTime_${THREAD_NO}Threads.txt" Code/minigraph/minigraph -t $THREAD_NO -cxlr Data/chr20.gfa Data/allLongReadsChr20Randomized.fasta > "ThreadScaling/Logs/minigraphLongRead${THREAD_NO}.log"
    echo "running minigraph long reads Large Batch"
    /usr/bin/time --verbose -o "ThreadScaling/Timing/minigraphLongReadTime_${THREAD_NO}Threads.txt" Code/minigraph/minigraph -K 5000M -t $THREAD_NO -cxlr Data/chr20.gfa Data/allLongReadsChr20Randomized.fasta > "ThreadScaling/Logs/minigraphLongReadLargeBatch${THREAD_NO}.log"
    echo "running vg map"
    /usr/bin/time --verbose -o "ThreadScaling/Timing/vgMapTime_${THREAD_NO}Threads.txt" Code/vg/bin/vg map -t $THREAD_NO -d Data/chr20 -F Data/HG002_HiSeq30x_Chr20Randomized.fasta --gaf  > "ThreadScaling/Logs/vg${THREAD_NO}.log"
done
