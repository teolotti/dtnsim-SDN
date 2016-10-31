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

    void addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate);
    Contact *getContact(int id);

private:

    vector<Contact> contacts_;

};

#endif /* CONTACTPLAN_H_ */
