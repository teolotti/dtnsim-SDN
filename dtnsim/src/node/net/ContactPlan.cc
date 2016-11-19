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

Contact *ContactPlan::getContactById(int id)
{
	for (size_t i = 0; i < contacts_.size(); i++)
	{
		if (contacts_.at(i).getId() == id)
		{
			return &contacts_.at(i);
		}
	}

	return NULL;
}

vector<Contact> * ContactPlan::getContacts()
{
	return &contacts_;
}

vector<Contact> ContactPlan::getContactsBySrc(int Src)
{
	vector<Contact> contacts;
	for (size_t i = 0; i < contacts_.size(); i++)
	{
		if (contacts_.at(i).getSourceEid() == Src)
		{
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

vector<Contact> ContactPlan::getContactsByDst(int Dst)
{
	vector<Contact> contacts;
	for (size_t i = 0; i < contacts_.size(); i++)
	{
		if (contacts_.at(i).getDestinationEid() == Dst)
		{
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
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
