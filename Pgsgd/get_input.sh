#!/usr/bin/bash
data_dir="data"
odgi_bin="deps/odgi/bin/odgi"

set -e

mkdir -p $data_dir

echo "downloading and preprocessing the DRB1-3123 pangenome"
curl -o $data_dir/DRB1-3123.gfa https://raw.githubusercontent.com/pangenome/odgi/master/test/DRB1-3123_unsorted.gfa
$odgi_bin build -g $data_dir/DRB1-3123.gfa -o $data_dir/DRB1-3123.og

# echo "downloading and preprocessing the chr20 pangenome"
# curl -o $data_dir/chr20.gfa.gz https://s3-us-west-2.amazonaws.com/human-pangenomics/pangenomes/freeze/freeze1/pggb/chroms/chr20.hprc-v1.0-pggb.gfa.gz
# gunzip $data_dir/chr20.gfa.gz
# $odgi_bin build -g $data_dir/chr20.gfa -o $data_dir/chr20.og
