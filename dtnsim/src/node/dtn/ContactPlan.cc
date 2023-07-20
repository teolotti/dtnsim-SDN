#include <src/node/dtn/ContactPlan.h>

ContactPlan::~ContactPlan()
{

}

ContactPlan::ContactPlan()
{

}

ContactPlan::ContactPlan(const ContactPlan &contactPlan) {

	this->lastEditTime = contactPlan.lastEditTime;
	this->contactsFile_ = contactPlan.contactsFile_;
	this->contacts_ = contactPlan.contacts_;
	this->ranges_ = contactPlan.ranges_;

	for (size_t i = 0; i < contacts_.size(); i++) {
		contacts_.at(i).work = NULL;
	}

	for (size_t i = 0; i < ranges_.size(); i++) {
		ranges_.at(i).work = NULL;
	}
}

void ContactPlan::parseContactPlanFile(string fileName, int startId) {

	parseFile(fileName, startId);
}

void ContactPlan::parseFile(string fileName, int startId) {

	int id = startId;
	double start = 0.0;
	double end = 0.0;
	int sourceEid = 0;
	int destinationEid = 0;
	double dataRateOrRange = 0.0;

	string fileLine = "#";
	string a;
	string command;
	ifstream file;

	file.open(fileName.c_str());

	if (!file.is_open()) {
		throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());
	}

	while (getline(file, fileLine)) {

		if (fileLine.at(0) == 'C') { // skip first line
			continue;
		}

		stringstream stringLine(fileLine);
		vector<string> row;
		string word;
		while (getline(stringLine, word, ',')) {

			row.push_back(word);
		}

		this->addContact(row);
	}

	if (cin.bad()) {
		// IO error
	} else if (!cin.eof()) {
		// format error (not possible with getline but possible with operator>>)
	} else {
		// format error (not possible with getline but possible with operator>>)
		// or end of file (can't make the difference)
	}

	file.close();

	this->setContactsFile(fileName);
}

void ContactPlan::addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence) {

	Contact contact(id, start, end, sourceEid, destinationEid, dataRate, confidence, 0);

	contacts_.push_back(contact);

	lastEditTime = simTime();
}

void ContactPlan::addContact(vector<string> row) {

	int contactId = stoi(row.at(0));
	double start = stod(row.at(1));
	double end = stod(row.at(3));
	int srcEid = stoi(row.at(5));
	srcEid = srcEid + 1;
	int dstEid = stoi(row.at(7));
	dstEid = dstEid + 1;
	double range = stod(row.at(9));
	double rate = stod(row.at(10));

	Contact contact(contactId, start, end, srcEid, dstEid, rate, 1, 0);
	contacts_.push_back(contact);
	lastEditTime = simTime();

	Contact contact1(contactId, start, end, srcEid, dstEid, 0, 1, range);
	Contact contact2(contactId, start, end, dstEid, srcEid, 0, 1, range);
	ranges_.push_back(contact1);
	ranges_.push_back(contact2);

	lastEditTime = simTime(); // TODO
}

void ContactPlan::addRange(int id, double start, double end, int sourceEid, int destinationEid, double range, float confidence) {

	// Ranges can be declared in a single direction, but they are bidirectional
	// In the worst case they are repeated
	Contact contact1(id, start, end, sourceEid, destinationEid, 0, confidence, range);
	Contact contact2(id, start, end, destinationEid, sourceEid, 0, confidence, range);

	ranges_.push_back(contact1);
	ranges_.push_back(contact2);

	lastEditTime = simTime();
}

double ContactPlan::getRangeBySrcDst(int Src, int Dst) {

	double rangeFirstContact = -1;

	for (size_t i = 0; i < ranges_.size(); i++) {

		if ((ranges_.at(i).getSourceEid() == Src) && (ranges_.at(i).getDestinationEid() == Dst)) {
			rangeFirstContact = ranges_.at(i).getRange();
		}
	}

	return rangeFirstContact;
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

vector<Contact> * ContactPlan::getContacts() {
	return &contacts_;
}

vector<Contact> * ContactPlan::getRanges() {
	return &ranges_;
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

simtime_t ContactPlan::getLastEditTime() {
	return lastEditTime;
}

void ContactPlan::setContactsFile(string contactsFile) {
	contactsFile_ = contactsFile;
}

string ContactPlan::getContactsFile() {
	return contactsFile_;
}

void ContactPlan::printContactPlan() {

	vector<Contact>::iterator it;

	for (it = this->getContacts()->begin(); it != this->getContacts()->end(); ++it) {
		cout << "a contact +" <<
				(*it).getStart() << " +" << (*it).getEnd() << " " <<
				(*it).getSourceEid() << " " << (*it).getDestinationEid() << " " <<
				(*it).getResidualVolume() << "/" << (*it).getVolume() << endl;
	}

	for (it = this->getRanges()->begin(); it != this->getRanges()->end(); ++it) {
		cout << "a range +" <<
				(*it).getStart() << " +" << (*it).getEnd() << " " <<
				(*it).getSourceEid() << " " << (*it).getDestinationEid() << " " <<
				(*it).getRange() << endl;
	}

	cout << endl;
}

vector<Contact>::iterator ContactPlan::deleteContactById(int contactId) {

	vector<Contact>::iterator itReturn;

	vector<Contact>::iterator it;
	for (it = this->getContacts()->begin(); it != this->getContacts()->end(); ++it) {

		if (it->getId() == contactId) {
			itReturn = contacts_.erase(it);
			break;
		}
	}

	return itReturn;
}

int ContactPlan::getLastContactId() {

	if (contacts_.empty()) {
		return 0;
	}

	return contacts_.back().getId();
}

void ContactPlan::addContactPlan(ContactPlan &contactPlan) {

	vector<Contact> * contacts = contactPlan.getContacts();
	for (auto it = contacts->begin(); it != contacts->end(); ++it) {

		int id = it->getId();
		double start = it->getStart();
		double end = it->getEnd();
		int sourceEid = it->getSourceEid();
		int destinationEid = it->getDestinationEid();
		double dataRate = it->getDataRate();
		double confidence = it->getConfidence();

		this->addContact(id, start, end, sourceEid, destinationEid, dataRate, confidence);
	}

	vector<Contact> * ranges = contactPlan.getRanges();
	for (auto it = ranges->begin(); it != ranges->end(); ++it) {

		int id = it->getId();
		double start = it->getStart();
		double end = it->getEnd();
		int sourceEid = it->getSourceEid();
		int destinationEid = it->getDestinationEid();
		double range = it->getRange();
		double confidence = it->getConfidence();

		this->addRange(id, start, end, sourceEid, destinationEid, range, confidence);
	}

	for (size_t i = 0; i < contacts_.size(); i++) {
		contacts_.at(i).work = NULL;
	}

	for (size_t i = 0; i < ranges_.size(); i++) {
		ranges_.at(i).work = NULL;
	}
}

void ContactPlan::eraseContactPlan() {

	contacts_.clear();
	ranges_.clear();

	for (size_t i = 0; i < contacts_.size(); i++) {
		contacts_.at(i).work = NULL;
	}

	for (size_t i = 0; i < ranges_.size(); i++) {
		ranges_.at(i).work = NULL;
	}
}
