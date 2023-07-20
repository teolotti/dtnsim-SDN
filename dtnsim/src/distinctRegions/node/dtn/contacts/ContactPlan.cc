#include <src/distinctRegions/node/dtn/contacts/ContactPlan.h>

namespace dtnsimdistinct {

ContactPlan::~ContactPlan() {

}

ContactPlan::ContactPlan() {

}


void ContactPlan::addContact(int id, double start, double end, int sourceEid, int destinationEid, double range, double dataRate) {

	Contact contact(id, start, end, sourceEid, destinationEid, range, dataRate);
	contacts_.push_back(contact);
}

void ContactPlan::addContact(vector<string> row) {

	int contactId = stoi(row.at(0));
	double start = stod(row.at(1));
	double end = stod(row.at(3));
	int srcEid = stoi(row.at(5));
	int dstEid = stoi(row.at(7));
	double range = stod(row.at(9));
	double rate = stod(row.at(10));


	/* scenario3
	if ((1 <= srcEid) && (srcEid <= 14)) {
		// earth to moon 30mbps
		rate = 3750000.0;

	} else if (((29 <= srcEid) && (srcEid <= 32) ) || (srcEid == 0)) {
		// relay to anything 100 mbps
		rate = 12500000.0;

	} else {

		if (((29 <= dstEid) && (dstEid <= 32) ) || (dstEid == 0)) {
			// moon to relay 15 mbps
			rate = 1875000.0;

		} else {
			rate = 12500000.0;
		}
	} */


	/* scenario 2
	if ((0 <= srcEid) && (srcEid <= 2)) {
		// earth to moon 30mbps
		rate = 3750000.0;

	} else if ((7 <= srcEid) && (srcEid <= 11)) {
		// relay to anything 100 mbps
		rate = 12500000.0;

	} else {

		if ((7 <= dstEid) && (dstEid <= 11)) {
			// moon to relay 15 mbps
			rate = 1875000.0;

		} else {
			rate = 12500000.0;
		}
	}
	*/


	/* scenario 1
	if (srcEid == 0) {
		// GS -> moon = 30 Mbps
		rate = 3750000.0;
	} else if (dstEid == 0) {
		// moon -> GS = 100 Mbps
		rate = 12500000.0;
	} else if (srcEid == 1) {
		// moon -> relay = 15 Mbps
		rate = 1875000.0;
	} else {
		// relay -> rest
		rate = 12500000.0;
	} */

	Contact contact(contactId, start, end, srcEid, dstEid, range, rate);
	contacts_.push_back(contact);
}

void ContactPlan::printContactPlan() {

	for (size_t i = 0; i < contacts_.size(); i++) {

		Contact contact = contacts_.at(i);
		cout << "ID: " << contact.getId()
				<< ", " << contact.getSourceEid() << " to " << contact.getDestinationEid()
				<< ", window: " << contact.getStart() << " to " << contact.getEnd()
				<< ", rate: " << contact.getDataRate() << endl;
	}

}

vector<Contact> * ContactPlan::getContacts() {
	return &contacts_;
}

Contact *ContactPlan::getContactById(int id) {

	Contact *contactPtr = NULL;

	for (size_t i = 0; i < contacts_.size(); i++) {

		if (contacts_.at(i).getId() == id) {
			contactPtr = &contacts_.at(i);
			break;
		}
	}

	return contactPtr;
}

vector<Contact*> ContactPlan::getContactsBySrc(int Src) {

	vector<Contact*> contacts;

	for (size_t i = 0; i < contacts_.size(); i++) {

		if (contacts_.at(i).getSourceEid() == Src) {
			contacts.push_back(&contacts_.at(i));
		}
	}

	return contacts;
}


vector<Contact> ContactPlan::getContactsByDst(int Dst) {

	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++) {

		if (contacts_.at(i).getDestinationEid() == Dst) {
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}


vector<Contact> ContactPlan::getContactsBySrcDst(int Src, int Dst) {

	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++) {

		if ((contacts_.at(i).getSourceEid() == Src) && (contacts_.at(i).getDestinationEid() == Dst)) {
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

void ContactPlan::clearAllRouteSearchWorkingArea() {
	for (auto & contact : contacts_) {
		contact.clearRouteSearchWorkingArea();
	}
}

void ContactPlan::clearAllRouteSearchWorkingArea(int rootContactId) {
	for (auto & contact : contacts_) {
		if (contact.getId() != rootContactId) {
			contact.clearRouteSearchWorkingArea();
		}
	}
}

void ContactPlan::clearAllRouteManagementWorkingArea() {
	for (auto & contact : contacts_) {
		contact.clearRouteManagementWorkingArea();
	}
}

void ContactPlan::clearAllForwardingWorkingArea() {
	for (auto & contact : contacts_) {
		contact.clearForwardingWorkingArea();
	}
}

}
