#ifndef SRC_HIERARCHICALREGIONS_NODE_DTN_CONTACT_CONTACTPLAN_H_
#define SRC_HIERARCHICALREGIONS_NODE_DTN_CONTACT_CONTACTPLAN_H_

#include <src/hierarchicalRegions/node/dtn/contacts/Contact.h>
#include <omnetpp.h>
#include <vector>
#include <fstream>
#include <string>

using namespace std;
using namespace omnetpp;

namespace dtnsimhierarchical {

class ContactPlan {

public:

	ContactPlan();
	virtual ~ContactPlan();

	// Contact plan population function
	void addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, string sourceRegion, string destinationRegion);
	void addContactPlan(ContactPlan &otherContactPlan);

	// Contact plan exploration functions
	Contact* getContactById(int id);
	vector<Contact>* getContacts();;
	vector<Contact> getContactsBySrc(int Src);
	vector<Contact> getContactsBySrcRegion(int Src, string region);
	vector<Contact> getContactsByDst(int Dst);
	vector<Contact> getContactsBySrcDst(int Src, int Dst);
	vector<Contact> getContactsBySrcOrDst(int eid);

	void addPassageway(int eid);
	vector<int> getPassageways();

	// debug function
	void printContactPlan();


private:

	vector<Contact> contacts_;
	vector<int> passageways_;

};
}

#endif /* SRC_HIERARCHICALREGIONS_NODE_DTN_CONTACT_CONTACTPLAN_H_ */
