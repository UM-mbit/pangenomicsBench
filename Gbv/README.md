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
