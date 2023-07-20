#include <src/hierarchicalRegions/node/dtn/contacts/ContactPlan.h>

namespace dtnsimhierarchical {

ContactPlan::~ContactPlan() {

}

ContactPlan::ContactPlan() {

}


void ContactPlan::addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, string sourceRegion, string destinationRegion) {

	Contact contact(id, start, end, sourceEid, destinationEid, dataRate, sourceRegion, destinationRegion);
	contacts_.push_back(contact);
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

vector<Contact> ContactPlan::getContactsBySrc(int Src) {

	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++) {

		if (contacts_.at(i).getSourceEid() == Src) {
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

vector<Contact> ContactPlan::getContactsBySrcRegion(int Src, string region) {

	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++) {

		if (contacts_.at(i).getSourceEid() == Src && contacts_.at(i).getSourceRegion().compare(region) == 0) {
			contacts.push_back(contacts_.at(i));
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

vector<Contact> ContactPlan::getContactsBySrcOrDst(int eid) {

	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++) {

		if (contacts_.at(i).getSourceEid() == eid || contacts_.at(i).getDestinationEid() == eid) {
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

void ContactPlan::addPassageway(int eid) {

	passageways_.push_back(eid);


}

vector<int> ContactPlan::getPassageways() {
	return passageways_;
}

void ContactPlan::printContactPlan() {

	vector<Contact>::iterator it;
	for (it = this->getContacts()->begin(); it != this->getContacts()->end(); ++it) {
		cout << "a contact +" <<
				(*it).getStart() << " +" << (*it).getEnd() << " " <<
				(*it).getSourceEid() << " " << (*it).getDestinationEid() << " ID: " << (*it).getId() << endl;
	}

	vector<int>::iterator pw;
	for (pw = passageways_.begin(); pw != passageways_.end(); ++pw) {
		cout << "a passageway " << *pw << endl;
	}

	cout << endl;
}

void ContactPlan::addContactPlan(ContactPlan &otherContactPlan) {

	vector<Contact>* contacts = otherContactPlan.getContacts();
	for (auto it = contacts->begin(); it != contacts->end(); ++it) {

		int id = it->getId();
		double start = it->getStart();
		double end = it->getEnd();
		int sourceEid = it->getSourceEid();
		int destinationEid = it->getDestinationEid();
		double dataRate = it->getDataRate();
		string sourceRegion = it->getSourceRegion();
		string destinationRegion = it->getDestinationRegion();
		this->addContact(id, start, end, sourceEid, destinationEid, dataRate, sourceRegion, destinationRegion);
	}

	//vector<int> passageways = otherContactPlan.getPassageways();
	//for (auto it = passageways.begin(); it != passageways.end(); ++it) {

	//	this->addPassageway(*it);
	//} // TODO not necessary for routing...

	//for (size_t i = 0; i < contacts_.size(); i++) {
	//	contacts_.at(i).work = NULL;
	//}
}

}
