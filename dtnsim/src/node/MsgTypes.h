#ifndef SRC_NODE_MSGTYPES_H_
#define SRC_NODE_MSGTYPES_H_

// Messages types and their priority are
// equal, unique and idefined in this file.
// Higher scheduling priority (smaller
// numeric value) are executed first.

#define TRAFFIC_TIMER 0

#define BUNDLE 1

#define FORWARDING_MSG 2

#define CONTACT_END_TIMER 4
#define CONTACT_START_TIMER 5

#define FAULT_END_TIMER 21
#define FAULT_START_TIMER 20

#endif /* SRC_NODE_MSGTYPES_H_ */
