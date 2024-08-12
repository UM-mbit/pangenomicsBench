#!/usr/bin/bash
data_dir="data"
odgi_bin="deps/odgi/bin/odgi"

set -e

echo "downloading and preprocessing the DRB1-3123 pangenome"
mkdir -p $data_dir
curl -o $data_dir/DRB1-3123.gfa https://raw.githubusercontent.com/pangenome/odgi/master/test/DRB1-3123_unsorted.gfa
$odgi_bin build -g $data_dir/DRB1-3123.gfa -o $data_dir/DRB1-3123.og
