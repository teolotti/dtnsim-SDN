#ifndef CONTACTPLAN_H_
#define CONTACTPLAN_H_

#include <src/node/dtn/Contact.h>
#include <omnetpp.h>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

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
	vector<int> getContactsBySrc(int Src);
	vector<Contact> getContactsBySrcDst(int Src, int Dst);
	double getRangeBySrcDst(int Src, int Dst);
	void parseContactPlanFile(string fileName, int nodesNum, int contactsToProcess);
	void setContactsFile(string contactsFile);
	const string& getContactsFile() const;
	simtime_t getLastEditTime();

	// delete contact function
	void deleteContactById(int contactId);

	// debug function
	void printContactPlan();

private:
	void updateContactRanges();
	void sortContactsByArrivalTime();

	vector<Contact> contacts_;
	vector<vector<int> > contactsBySrc_;
	vector<Contact> ranges_;
	map<int, map<int, double> > rangesBySrcDst_;
	simtime_t lastEditTime;
	string contactsFile_;

};

#endif /* CONTACTPLAN_H_ */
