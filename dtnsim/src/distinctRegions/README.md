# Modified DtnSim

This folder holds the source code for a modified version of DtnSim. Several things have been adjusted/cleaned up:

- Extended APP module in order to define more complex traffic patterns
- Removed things like custody and priorities
- Some directory changes
- Contact plans and contacts
- SDR
- Routing

Most importantly, this code contains a working implementation of dynamic CGR, i.e. CGR with dynamic route management using Yen's algorithm (it doesn't use Lawler's modification, which is easily changed). Routes are computed on-demand by increasing the loop parameter K dynamically. The IRR algorithm has also been updated.

The name "distinct" comes from an initial idea of trying to implement a scalability approach that divides the network into distinct regions (as opposed to overlapping regions). This idea has not been fully implemented, better said it has been discarded. Traces of it can still be found throughout the source code (e.g. the mention of a "backbone" node, the graphic module, the need for an empty "backbone" contact plan, etc.).
