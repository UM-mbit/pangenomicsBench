# Building GraphAligner
1. Ensure that the environment variable `VTUNE_HOME` points to the vtune
   directory (e.g. `export VTUNE_HOME=/opt/intel/oneapi/vtune/latest`). This is
   tested with the compiler shipped in the GraphAligner conda environment
   (g++-11)
2. create the GraphAligner conda environment
   `cd deps/GraphAligner && conda env create -f CondaEnvironment_linux.yml && cd ../..`
3. activate the conda environment
   `conda activate GraphAligner`
4. build the kernel
   `make -j`

# Running GraphAligner
Run the kernel with the command
`./bin/gbv <path to data> <num iterations, or blank>`
Example with `$KERNEL_DATA` pointing to the kernel data directory
`./bin/gbv $KERNEL_DATA/Gbv/Mc1e3Reads 30`:

# Generating Inputs  
Run the command
`GraphAligner -g <path to chr20 vg graph> -t 1 -x vg -f <path to fasta> -a
<output in gaf format. doesn't matter>`
An example
`bin/GraphAligner -g test/data/chr20.vg -t 1 -x vg -f chr20.fasta -a out.gaf`
Now we must do a jq -s . clusters.json > tmp && mv tmp clusters.json`

# Understanding Outputs
Outputs are in json format. One json object per output. The objects have the
format:
```json
{
  "index": <theIterationNumber>,
  "traces": [
    {
      "graphChars": [ #the characters in the sequence in the graph
        <asciiEncodingOfBasePair>,
        <asciiEncodingOfBasePair>,
        ...
      ]
      "nodeId": [ #the node the character is from
        <nodeId>,
        <nodeId>,
        ...
      ]
      "nodeOffset": [ #offset in the node. With nodeId gives loc of char
        <nodeOffset>,
        <nodeOffset>,
        ...
      ]
      "score": <score>, #alignment score
      "seqChars": [ #sequence character being matched
        <asciiEncodingOfBasePair>,
        <asciiEncodingOfBasePair>,
        ...
      ]
    } 
  ]
}
```
