# Commands extracted from PGGB

run_wfmash() {
    wfmash -s 10000 -l 50000 -p 99.95 -n 1 -k 19 -H 0.001 -Y \# -t 28 \
        ../../Data/PggbData/chr20.pan.fasta --lower-triangular \
        --hg-filter-ani-diff 30 --approx-map > chr20.mappings.paf 2> wfmash.log &&
    wfmash -s 10000 -l 50000 -p 99.95 -n 1 -k 19 -H 0.001 -Y \# -t 28 \
        ../../Data/PggbData/chr20.pan.fasta --lower-triangular \
        --hg-filter-ani-diff 30 -i chr20.mappings.paf --invert-filtering > chr20.paf 2>> wfmash.log
}

run_seqwish() {
    seqwish -s ../../Data/PggbData/chr20.pan.fasta -p chr20.paf -k 23 -f 0 -g chr20.seqwish.gfa -B 10M -t 28 -P &> seqwish.log
}

run_smoothxg() {
    smoothxg -t 28 -T 28 -g chr20.seqwish.gfa -r 14 -K --chop-to 100 -I .9995 -R 0 \
        -j 0 -e 0 -l 700,900,1100 -P 1,19,39,3,81,1 -O 0.001 -Y 1400 -d 0 -D 0 -Q Consensus_ -V -o chr20.smooth.gfa &> smoothxg.log 
}

run_odgi_layout() {
    ../../Code/odgi/bin/odgi build -g chr20.smooth.gfa -o chr20.og -t 28 -P &> odgi.log &&
    ../../Code/odgi/bin/odgi layout -i chr20.og -o chr20.og.lay -T out/*.tsv -t 28 -P &>> odgi.log &&
    ../../Code/odgi/bin/odgi draw -i chr20.og -c chr20.og.lay -p chr20.png -C &>> odgi.log
}


TIMEFORMAT="Elapsed time: %R seconds"

echo "running wfmash..."
time run_wfmash

echo "running seqwish..."
time run_seqwish

echo "running smoothxg..."
time run_smoothxg

echo "running odgi layout..."
time run_odgi_layout
