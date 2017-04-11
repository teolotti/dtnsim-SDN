#ifndef CONTACTPLAN_H_
#define CONTACTPLAN_H_

#include <omnetpp.h>
#include <vector>
#include <fstream>

#include "Contact.h"

using namespace std;
using namespace omnetpp;

class ContactPlan {

public:

    ContactPlan();
    virtual ~ContactPlan();

    void parseContactPlanFile(string fileName);
    void setContactsFile(string contactsFile);
    const string& getContactsFile() const;

    void addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence);

    Contact *getContactById(int id);
    vector<Contact> * getContacts();
    vector<Contact> getContactsBySrc(int Src);
    vector<Contact> getContactsByDst(int Dst);
    vector<Contact> getContactsBySrcDst(int Src, int Dst);
    Contact getContactByTuple(int src, int dst, double start, double end);

    // fill structs to make fast searchs of contacts
    void finishContactPlan();

    simtime_t getLastEditTime();

private:

    vector<Contact> contacts_;
    simtime_t lastEditTime;

    // std structs to make fast searchs of contacts
    map<int, vector<Contact *> > contactsBySrc_;
    map<int, vector<Contact *> > contactsByDst_;
    map<int, Contact *> contactsById_;

    string contactsFile_;

};

#endif /* CONTACTPLAN_H_ */
