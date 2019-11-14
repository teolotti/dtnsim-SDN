#ifndef CONTACTPLAN_H_
#define CONTACTPLAN_H_

#include <src/node/dtn/Contact.h>
#include <omnetpp.h>
#include <vector>
#include <fstream>

using namespace std;
using namespace omnetpp;

class ContactPlan {

public:

	ContactPlan();
	virtual ~ContactPlan();
	ContactPlan(ContactPlan &contactPlan);

	// Contact plan population functions
	void addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence);
	void addRange(int id, double start, double end, int sourceEid, int destinationEid, double range, float confidence);

	// Contact plan exploration functions
	Contact *getContactById(int id);
	vector<Contact> * getContacts();
	vector<Contact> * getRanges();
	vector<Contact> getContactsBySrc(int Src);
	vector<Contact> getContactsByDst(int Dst);
	vector<Contact> getContactsBySrcDst(int Src, int Dst);
	double getRangeBySrcDst(int Src, int Dst);
	void parseContactPlanFile(string fileName);
	void setContactsFile(string contactsFile);
	const string& getContactsFile() const;
	simtime_t getLastEditTime();

	// delete contact function
	vector<Contact>::iterator deleteContactById(int contactId);

	// debug function
	void printContactPlan();

private:

	vector<Contact> contacts_;
	vector<Contact> ranges_;
	simtime_t lastEditTime;
	string contactsFile_;

};

#endif /* CONTACTPLAN_H_ */
