#include <src/node/dtn/ContactPlan.h>

ContactPlan::~ContactPlan()
{

}

ContactPlan::ContactPlan()
{

}

ContactPlan::ContactPlan(ContactPlan &contactPlan)
{
	this->lastEditTime = contactPlan.lastEditTime;
	this->contactsFile_ = contactPlan.contactsFile_;
	this->contacts_ = contactPlan.contacts_;
	this->contactsBySrc_ = contactPlan.contactsBySrc_;
	this->rangesBySrcDst_ = contactPlan.rangesBySrcDst_;

	for (size_t i = 0; i < contacts_.size(); i++)
	{
		contacts_.at(i).work = NULL;
	}
}

void ContactPlan::parseContactPlanFile(string fileName, int nodesNum, int contactsToProcess)
{
	int id = 0;
	double start = 0.0;
	double end = 0.0;
	int sourceEid = 0;
	int destinationEid = 0;
	double dataRateOrRange = 0.0;
	contactsBySrc_.resize(nodesNum + 1);

	for (int i = 0; i <= nodesNum; i++) { contactsBySrc_.at(i).clear(); }


	string fileLine = "#";
	string a;
	string command;
	ifstream file;

	file.open(fileName.c_str());

	if (!file.is_open())
		throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());

	while (getline(file, fileLine))
	{
		if (fileLine.empty())
			continue;

		if (fileLine.at(0) == '#')
			continue;



		// This seems to be a valid command line, parse it
		stringstream stringLine(fileLine);
		stringLine >> a >> command >> start >> end >> sourceEid >> destinationEid >> dataRateOrRange;

		if (a.compare("a") == 0)
		{
			if ((command.compare("contact") == 0))
			{
			    if (contactsToProcess == 0)
			        continue;

			    contactsToProcess--;

				this->addContact(id, start, end, sourceEid, destinationEid, dataRateOrRange, (float) 1.0);
				id++;
			}
			else if ((command.compare("range") == 0))
			{
				this->addRange(id, start, end, sourceEid, destinationEid, dataRateOrRange, (float) 1.0);
			}
			else
			{
				cout << "dtnsim error: unknown contact plan command type: a " << fileLine << endl;
			}
		}
	}

	updateContactRanges();
	ranges_.clear();

	if (cin.bad())
	{
		// IO error
	}
	else if (!cin.eof())
	{
		// format error (not possible with getline but possible with operator>>)
	}
	else
	{
		// format error (not possible with getline but possible with operator>>)
		// or end of file (can't make the difference)
	}

	file.close();

	this->setContactsFile(fileName);
}

void ContactPlan::addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence)
{
	Contact contact(id, start, end, sourceEid, destinationEid, dataRate, confidence, -1);
	contacts_.push_back(contact);
	contactsBySrc_.at(sourceEid).push_back(id);

	lastEditTime = simTime();
}

void ContactPlan::addRange(int id, double start, double end, int sourceEid, int destinationEid, double range, float confidence)
{
    Contact rangeContact(id, start, end, sourceEid, destinationEid, 0, confidence, range);
    ranges_.push_back(rangeContact);

	lastEditTime = simTime();
}

void ContactPlan::updateContactRanges() {
    for (vector<Contact>::iterator rangeContact = ranges_.begin(); rangeContact != ranges_.end(); rangeContact++) {
        int sourceEid = rangeContact->getSourceEid();
        int destinationEid = rangeContact->getDestinationEid();
        double start = rangeContact->getStart();
        double end = rangeContact->getEnd();

        vector<int> contactsBySrc = getContactsBySrc(sourceEid);
        for (vector<int>::iterator contactId = contactsBySrc.begin(); contactId != contactsBySrc.end(); contactId++) {
            Contact* contact = getContactById(*contactId);
            if (contact->getDestinationEid() == destinationEid &&
                    contact->getStart() >= start &&
                    contact->getEnd() <= end) {

                contact->setRange(rangeContact->getRange());
            }
        }
    }
}

double ContactPlan::getRangeBySrcDst(int Src, int Dst)
{
	cout << "WARNING: ContactPlan::getRangeBySrcDst() deprecated. Use Contact::getRange() instead." << endl;
	return -1;
}

Contact *ContactPlan::getContactById(int id)
{
	return &contacts_.at(id);
}

vector<Contact> * ContactPlan::getContacts()
{
	return &contacts_;
}

vector<int> ContactPlan::getContactsBySrc(int Src)
{
	return contactsBySrc_.at(Src);
}

vector<Contact> ContactPlan::getContactsBySrcDst(int Src, int Dst)
{
	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++)
	{
		if ((contacts_.at(i).getSourceEid() == Src) && (contacts_.at(i).getDestinationEid() == Dst))
		{
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

simtime_t ContactPlan::getLastEditTime()
{
	return lastEditTime;
}

void ContactPlan::setContactsFile(string contactsFile)
{
	contactsFile_ = contactsFile;
}

const string& ContactPlan::getContactsFile() const
{
	return contactsFile_;
}

void ContactPlan::printContactPlan()
{
	vector<Contact>::iterator it;
	for (it = this->getContacts()->begin(); it != this->getContacts()->end(); ++it)
		cout << "a contact +" << (*it).getStart() << " +" << (*it).getEnd() << " " << (*it).getSourceEid() << " " << (*it).getDestinationEid() << " " << (*it).getResidualVolume() << "/" << (*it).getVolume() << endl;

	cout << endl;
}

void ContactPlan::deleteContactById(int contactId)
{
    cout << "Warning: ContactPlan.cc:deleteContactById() has been changed: contacts are not " <<
            "removed anymore, a flag is set to tell whether they have been deleted. " <<
            "Make sure the algorithms using deleted contacts are properly modified." << endl;
    exit(1);

	contacts_.at(contactId).setDeleted(true);
}
