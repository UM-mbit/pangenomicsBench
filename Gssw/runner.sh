#!/usr/bin/bash

APP="Code/vg/bin/vg map -d Datasets/Reference/PangenomeRef/HPRC/Vcf/grch38 -t 1 \
    -F Datasets/Reads/ShortReads/SRR7733443Reads/SimReads/simShortReads1e3.fastq"

COMMAND+="${APP} > stdout.txt 2> >(tee stderr.txt >&2)"

#COMMAND="gdb --args ${APP}"
#break ld_node_no_edges

echo "Running command: ${COMMAND}"
eval ${COMMAND}

