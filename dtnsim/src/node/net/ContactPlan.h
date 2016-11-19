#ifndef CONTACTPLAN_H_
#define CONTACTPLAN_H_

#include <omnetpp.h>
#include <vector>
#include "Contact.h"

using namespace std;
using namespace omnetpp;

class ContactPlan {

public:

    ContactPlan();
    virtual ~ContactPlan();

    void addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence);
    Contact *getContactById(int id);
    vector<Contact> * getContacts();
    vector<Contact> getContactsBySrc(int Src);
    vector<Contact> getContactsByDst(int Dst);
    vector<Contact> getContactsBySrcDst(int Src, int Dst);

    simtime_t getLastEditTime();

private:

    vector<Contact> contacts_;
    simtime_t lastEditTime;

};

#endif /* CONTACTPLAN_H_ */
