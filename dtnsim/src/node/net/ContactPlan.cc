#include "ContactPlan.h"

ContactPlan::~ContactPlan()
{

}

ContactPlan::ContactPlan()
{

}

void ContactPlan::addContact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate)
{
	Contact contact(id, start, end, sourceEid, destinationEid, dataRate);
	contacts_.push_back(contact);
}

Contact *ContactPlan::getContact(int id)
{
	for(size_t i = 0; i<contacts_.size(); i++)
	{
		if(contacts_.at(i).getId() == id)
		{
			return &contacts_.at(i);
		}
	}

	return NULL;
}

