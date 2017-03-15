#include "ContactPlan.h"

ContactPlan::~ContactPlan()
{

}

ContactPlan::ContactPlan()
{

}

// todo fix in simulator: ion considers data rate in contact plan in bytes /second units
// now the simulator considers bits / second
void ContactPlan::addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence)
{
	// todo check older contact plans
	// data rate is in bytes / second
	Contact contact(id, start, end, sourceEid, destinationEid, dataRate*8, confidence);

	contacts_.push_back(contact);

	lastEditTime = simTime();
}

void ContactPlan::finishContactPlan()
{
	for (size_t i = 0; i < contacts_.size(); i++)
	{
		Contact *contactPtr = &contacts_.at(i);

		contactsBySrc_[contactPtr->getSourceEid()].push_back(contactPtr);
		contactsByDst_[contactPtr->getDestinationEid()].push_back(contactPtr);
		contactsById_[contactPtr->getId()] = contactPtr;
	}
}

Contact *ContactPlan::getContactById(int id)
{
	Contact *contactPtr = NULL;

	map<int, Contact *>::iterator it = contactsById_.find(id);
	if (it != contactsById_.end())
	{
		contactPtr  = it->second;
	}

	return contactPtr;
}

vector<Contact> * ContactPlan::getContacts()
{
	return &contacts_;
}

vector<Contact> ContactPlan::getContactsBySrc(int Src)
{
	vector<Contact> contacts;
	map<int, vector<Contact *> >::iterator it = contactsBySrc_.find(Src);

	if (it != contactsBySrc_.end())
	{
		vector<Contact *> contactsPtr = it->second;
		for (size_t i = 0; i < contactsPtr.size(); i++)
		{
			contacts.push_back(*contactsPtr.at(i));
		}
	}

	return contacts;
}

vector<Contact> ContactPlan::getContactsByDst(int Dst)
{
	vector<Contact> contacts;
	map<int, vector<Contact *> >::iterator it = contactsByDst_.find(Dst);

	if (it != contactsByDst_.end())
	{
		vector<Contact *> contactsPtr = it->second;
		for (size_t i = 0; i < contactsPtr.size(); i++)
		{
			contacts.push_back(*contactsPtr.at(i));
		}
	}

	return contacts;
}

vector<Contact> ContactPlan::getContactsBySrcDst(int Src, int Dst)
{
	vector<Contact> contacts;
	vector<Contact *> contactsSrcDst;
	vector<Contact *> contactsSrc;

	map<int, vector<Contact *> >::iterator it = contactsBySrc_.find(Src);

	if (it != contactsBySrc_.end())
	{
		contactsSrc = it->second;
	}

	for (size_t i = 0; i < contactsSrc.size(); i++)
	{
		if (contactsSrc.at(i)->getDestinationEid() == Dst)
		{
			contactsSrcDst.push_back(contactsSrc.at(i));
		}
	}

	for (size_t i = 0; i < contactsSrcDst.size(); i++)
	{
		contacts.push_back(*contactsSrcDst.at(i));
	}

	return contacts;
}

simtime_t ContactPlan::getLastEditTime()
{
	return lastEditTime;
}

Contact ContactPlan::getContactByTuple(int src, int dst, double start, double end)
{
    Contact contactByTuple(0,0,0,0,0,0,0);
    vector<Contact> contacts = getContactsBySrcDst(src, dst);
	for(size_t i = 0; i<contacts.size(); i++)
	{
		if((contacts.at(i).getSourceEid() != src) || (contacts.at(i).getDestinationEid() != dst))
		{
			cout<<"Error in method getContactsBySrcDst"<<endl;
		}

		if((contacts.at(i).getStart() == start) && (contacts.at(i).getEnd() == end))
		{
			contactByTuple = contacts.at(i);
			break;
		}
	}

	return contactByTuple;
}

void ContactPlan::setContactsFile(string contactsFile)
{
	contactsFile_ = contactsFile;
}

const string& ContactPlan::getContactsFile() const
{
	return contactsFile_;
}
