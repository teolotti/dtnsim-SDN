#ifndef SRC_NODE_MSGTYPES_H_
#define SRC_NODE_MSGTYPES_H_

// Messages types and their priority are
// equal, unique and defined in this file.
// Higher scheduling priority (smaller
// numeric value) are executed first.

#define CONTACT_END_TIMER 0
#define CONTACT_START_TIMER 1

#define TRAFFIC_TIMER 2

#define BUNDLE 3
#define CUSTODY_REPORT 4

#define FORWARDING_MSG_END 5
#define FORWARDING_MSG_START 6

#define FAULT_END_TIMER 21
#define FAULT_START_TIMER 20

#endif /* SRC_NODE_MSGTYPES_H_ */
