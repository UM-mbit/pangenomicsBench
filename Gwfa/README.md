Please note that the raw inputs as generated from running the kernel do not
include the graph. You must move the graph there manually and title it
"graph.gfa"

# Understanding Output 
Each kernel output gets one line of the format:
```
<gfa_edrst_t.s> <gfa_edrst_t.wlen> { <nodeId> <nodeId> ... }
```

I include the first two terms so we can compare with true outputs and ensure we
didn't change anything, but I do not understand
their exact function. If I had to guess, wlen is some kind of character count,
but to really understand these terms you will need to dig through the minigraph
source. I found them difficult to understand.

The bracket term is the more important result anyways, it represents a path 
through the graph.
