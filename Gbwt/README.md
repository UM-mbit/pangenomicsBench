# Understanding Outputs
Outputs are of the format:
```
<nodeId>: (<startRange>,<endRange>)
```

One for each call to the kernel. The range is the range attribute of a
SearchState object defined in gbwt/algorithms.h.
This corresponds to the range of edges that can be followed from the node
