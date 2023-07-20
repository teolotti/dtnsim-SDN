#ifndef SRC_DISTINCTREGIONS_NODE_DTN_CONTACT_CONTACTPLAN_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_CONTACT_CONTACTPLAN_H_

#include <src/distinctRegions/node/dtn/contacts/Contact.h>
#include <omnetpp.h>
#include <vector>
#include <fstream>
#include <string>

using namespace std;
using namespace omnetpp;

namespace dtnsimdistinct {

class ContactPlan {

public:

	ContactPlan();
	virtual ~ContactPlan();

	// Contact plan population function
	void addContact(int id, double start, double end, int sourceEid, int destinationEid, double range, double dataRate);
	void addContact(vector<string> row);

	// Contact plan exploration functions
	Contact *getContactById(int id);
	vector<Contact> * getContacts();;
	vector<Contact*> getContactsBySrc(int Src);
	vector<Contact> getContactsByDst(int Dst);
	vector<Contact> getContactsBySrcDst(int Src, int Dst);

	// debug function
	void printContactPlan();

	// Working area clearance
	void clearAllRouteSearchWorkingArea();
	void clearAllRouteSearchWorkingArea(int rootContactId);
	void clearAllRouteManagementWorkingArea();
	void clearAllForwardingWorkingArea();

private:

	vector<Contact> contacts_;

};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_CONTACT_CONTACTPLAN_H_ */
