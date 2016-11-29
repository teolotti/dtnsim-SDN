#include "ContactPlan.h"

ContactPlan::~ContactPlan()
{

}

ContactPlan::ContactPlan()
{

}

void ContactPlan::addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence)
{
	Contact contact(id, start, end, sourceEid, destinationEid, dataRate, confidence);

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
