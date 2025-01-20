# Building Gssw
1. Ensure that the environment variable `VTUNE_HOME` points to the vtune
   directory (e.g. `export VTUNE_HOME=/opt/intel/oneapi/vtune/latest`). This is
   tested with the compiler shipped in the GraphAligner conda environment
   (g++-11)
2. Create python 3.8 environment. Python of later version could cause the
   program to fail to build.
   `conda create -n vg python=3.8`
   activate the environment
3. Build Gssw library
   `cd deps/Gssw && make -j && cd ../..`
4. Build the kernel
   `make -j`

# Running the Kernel
Run the kernel with the command
`./bin/gssw <path to data> <num iterations, or blank>`
Example with `$KERNEL_DATA` pointing to the kernel data directory
`./bin/gssw $KERNEL_DATA/Gssw/McFirst1e4Inputs 30`:

# Generating Inputs
These runs were computed by running a modified version of vg map.
`https://github.com/kaplannp/vg/tree/dumper`
They use Illumina reads from the HPRC v1 assemblies for HG002.
For a graph, they use the HPRC v1 chrom20 assembly constructed via Minigraph
Cactus.
After running vg futher filtering commands were used to get a smaller subset:
`jq '.[:1000]' graph.json > graph1.json.copy` extracts the first 1000 elements
`head -n 1000 reads.txt > reads.txt.copy` gives the first 1000 reads
We use
`jq -s . graph.json > tmp && mv tmp graph.json`
to correct the formating errors

The graph in file contains a json serialized version of the gssw graph
structure. The reads contain one read per line
The format of the json graph is this:
```                                                                                   
graph                                                                                 
  + nodes                                                                             
    + alignment (null to start)                                                       
    + count_next the number of nodes reachable from this node                         
    + next list of the nodes reachable from this node                                 
    + count_prev number of nodes with edges to this node                              
    + prev list of the nodes with edges to this node                                  
    + ptr id of this node                                                             
    + num is the numerical conversion of sequence (a=0, g=1, c=2, t=3) Not            
      sure exactly which mapping, but that's the idea                                 
    + len is length of sequence                                                       
    + id I think should map to the cigar strings                                      
```                                                                                   

# Understanding Outputs
Each run of the kernel generates one line in the output file. The output is not
json. Each line has the format
```
<BestScore>@<FirstNodeOffset>:<GraphNode>[<CigarStringThroughGraphNodeSequence>]<GraphNode>[<CigarStringThroughGraphNodeSequence]...
```
