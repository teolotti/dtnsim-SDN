//
// Generated file, do not edit! Created by nedtool 5.0 from dtnsim.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "dtnsim_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: no doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: no doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(Bundle);

Bundle::Bundle(const char *name, int kind) : ::omnetpp::cPacket(name,kind)
{
    this->sourceEid = 0;
    this->destinationEid = 0;
    this->returnToSender = false;
    this->critical = false;
    this->creationTimestamp = 0;
    this->ttl = 0;
    this->senderEid = 0;
    this->nextHopEid = 0;
    this->xmitCopiesCount = 0;
    this->dlvConfidence = 0;
}

Bundle::Bundle(const Bundle& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Bundle::~Bundle()
{
}

Bundle& Bundle::operator=(const Bundle& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Bundle::copy(const Bundle& other)
{
    this->sourceEid = other.sourceEid;
    this->destinationEid = other.destinationEid;
    this->returnToSender = other.returnToSender;
    this->critical = other.critical;
    this->creationTimestamp = other.creationTimestamp;
    this->ttl = other.ttl;
    this->senderEid = other.senderEid;
    this->nextHopEid = other.nextHopEid;
    this->xmitCopiesCount = other.xmitCopiesCount;
    this->dlvConfidence = other.dlvConfidence;
    this->originalRoute = other.originalRoute;
    this->takenRoute = other.takenRoute;
}

void Bundle::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->sourceEid);
    doParsimPacking(b,this->destinationEid);
    doParsimPacking(b,this->returnToSender);
    doParsimPacking(b,this->critical);
    doParsimPacking(b,this->creationTimestamp);
    doParsimPacking(b,this->ttl);
    doParsimPacking(b,this->senderEid);
    doParsimPacking(b,this->nextHopEid);
    doParsimPacking(b,this->xmitCopiesCount);
    doParsimPacking(b,this->dlvConfidence);
    doParsimPacking(b,this->originalRoute);
    doParsimPacking(b,this->takenRoute);
}

void Bundle::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->sourceEid);
    doParsimUnpacking(b,this->destinationEid);
    doParsimUnpacking(b,this->returnToSender);
    doParsimUnpacking(b,this->critical);
    doParsimUnpacking(b,this->creationTimestamp);
    doParsimUnpacking(b,this->ttl);
    doParsimUnpacking(b,this->senderEid);
    doParsimUnpacking(b,this->nextHopEid);
    doParsimUnpacking(b,this->xmitCopiesCount);
    doParsimUnpacking(b,this->dlvConfidence);
    doParsimUnpacking(b,this->originalRoute);
    doParsimUnpacking(b,this->takenRoute);
}

int Bundle::getSourceEid() const
{
    return this->sourceEid;
}

void Bundle::setSourceEid(int sourceEid)
{
    this->sourceEid = sourceEid;
}

int Bundle::getDestinationEid() const
{
    return this->destinationEid;
}

void Bundle::setDestinationEid(int destinationEid)
{
    this->destinationEid = destinationEid;
}

bool Bundle::getReturnToSender() const
{
    return this->returnToSender;
}

void Bundle::setReturnToSender(bool returnToSender)
{
    this->returnToSender = returnToSender;
}

bool Bundle::getCritical() const
{
    return this->critical;
}

void Bundle::setCritical(bool critical)
{
    this->critical = critical;
}

::omnetpp::simtime_t Bundle::getCreationTimestamp() const
{
    return this->creationTimestamp;
}

void Bundle::setCreationTimestamp(::omnetpp::simtime_t creationTimestamp)
{
    this->creationTimestamp = creationTimestamp;
}

::omnetpp::simtime_t Bundle::getTtl() const
{
    return this->ttl;
}

void Bundle::setTtl(::omnetpp::simtime_t ttl)
{
    this->ttl = ttl;
}

int Bundle::getSenderEid() const
{
    return this->senderEid;
}

void Bundle::setSenderEid(int senderEid)
{
    this->senderEid = senderEid;
}

int Bundle::getNextHopEid() const
{
    return this->nextHopEid;
}

void Bundle::setNextHopEid(int nextHopEid)
{
    this->nextHopEid = nextHopEid;
}

int Bundle::getXmitCopiesCount() const
{
    return this->xmitCopiesCount;
}

void Bundle::setXmitCopiesCount(int xmitCopiesCount)
{
    this->xmitCopiesCount = xmitCopiesCount;
}

double Bundle::getDlvConfidence() const
{
    return this->dlvConfidence;
}

void Bundle::setDlvConfidence(double dlvConfidence)
{
    this->dlvConfidence = dlvConfidence;
}

List& Bundle::getOriginalRoute()
{
    return this->originalRoute;
}

void Bundle::setOriginalRoute(const List& originalRoute)
{
    this->originalRoute = originalRoute;
}

List& Bundle::getTakenRoute()
{
    return this->takenRoute;
}

void Bundle::setTakenRoute(const List& takenRoute)
{
    this->takenRoute = takenRoute;
}

class BundleDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    BundleDescriptor();
    virtual ~BundleDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(BundleDescriptor);

BundleDescriptor::BundleDescriptor() : omnetpp::cClassDescriptor("Bundle", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

BundleDescriptor::~BundleDescriptor()
{
    delete[] propertynames;
}

bool BundleDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Bundle *>(obj)!=nullptr;
}

const char **BundleDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *BundleDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int BundleDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount() : 12;
}

unsigned int BundleDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<12) ? fieldTypeFlags[field] : 0;
}

const char *BundleDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sourceEid",
        "destinationEid",
        "returnToSender",
        "critical",
        "creationTimestamp",
        "ttl",
        "senderEid",
        "nextHopEid",
        "xmitCopiesCount",
        "dlvConfidence",
        "originalRoute",
        "takenRoute",
    };
    return (field>=0 && field<12) ? fieldNames[field] : nullptr;
}

int BundleDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceEid")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationEid")==0) return base+1;
    if (fieldName[0]=='r' && strcmp(fieldName, "returnToSender")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "critical")==0) return base+3;
    if (fieldName[0]=='c' && strcmp(fieldName, "creationTimestamp")==0) return base+4;
    if (fieldName[0]=='t' && strcmp(fieldName, "ttl")==0) return base+5;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderEid")==0) return base+6;
    if (fieldName[0]=='n' && strcmp(fieldName, "nextHopEid")==0) return base+7;
    if (fieldName[0]=='x' && strcmp(fieldName, "xmitCopiesCount")==0) return base+8;
    if (fieldName[0]=='d' && strcmp(fieldName, "dlvConfidence")==0) return base+9;
    if (fieldName[0]=='o' && strcmp(fieldName, "originalRoute")==0) return base+10;
    if (fieldName[0]=='t' && strcmp(fieldName, "takenRoute")==0) return base+11;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *BundleDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "bool",
        "bool",
        "simtime_t",
        "simtime_t",
        "int",
        "int",
        "int",
        "double",
        "List",
        "List",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : nullptr;
}

const char **BundleDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *BundleDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int BundleDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    Bundle *pp = (Bundle *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string BundleDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Bundle *pp = (Bundle *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSourceEid());
        case 1: return long2string(pp->getDestinationEid());
        case 2: return bool2string(pp->getReturnToSender());
        case 3: return bool2string(pp->getCritical());
        case 4: return simtime2string(pp->getCreationTimestamp());
        case 5: return simtime2string(pp->getTtl());
        case 6: return long2string(pp->getSenderEid());
        case 7: return long2string(pp->getNextHopEid());
        case 8: return long2string(pp->getXmitCopiesCount());
        case 9: return double2string(pp->getDlvConfidence());
        case 10: {std::stringstream out; out << pp->getOriginalRoute(); return out.str();}
        case 11: {std::stringstream out; out << pp->getTakenRoute(); return out.str();}
        default: return "";
    }
}

bool BundleDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    Bundle *pp = (Bundle *)object; (void)pp;
    switch (field) {
        case 0: pp->setSourceEid(string2long(value)); return true;
        case 1: pp->setDestinationEid(string2long(value)); return true;
        case 2: pp->setReturnToSender(string2bool(value)); return true;
        case 3: pp->setCritical(string2bool(value)); return true;
        case 4: pp->setCreationTimestamp(string2simtime(value)); return true;
        case 5: pp->setTtl(string2simtime(value)); return true;
        case 6: pp->setSenderEid(string2long(value)); return true;
        case 7: pp->setNextHopEid(string2long(value)); return true;
        case 8: pp->setXmitCopiesCount(string2long(value)); return true;
        case 9: pp->setDlvConfidence(string2double(value)); return true;
        default: return false;
    }
}

const char *BundleDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 10: return omnetpp::opp_typename(typeid(List));
        case 11: return omnetpp::opp_typename(typeid(List));
        default: return nullptr;
    };
}

void *BundleDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Bundle *pp = (Bundle *)object; (void)pp;
    switch (field) {
        case 10: return (void *)(&pp->getOriginalRoute()); break;
        case 11: return (void *)(&pp->getTakenRoute()); break;
        default: return nullptr;
    }
}

Register_Class(TrafficGeneratorMsg);

TrafficGeneratorMsg::TrafficGeneratorMsg(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->bundlesNumber = 0;
    this->destinationEid = 0;
    this->size = 0;
    this->ttl = 0;
    this->interval = 0;
}

TrafficGeneratorMsg::TrafficGeneratorMsg(const TrafficGeneratorMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

TrafficGeneratorMsg::~TrafficGeneratorMsg()
{
}

TrafficGeneratorMsg& TrafficGeneratorMsg::operator=(const TrafficGeneratorMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void TrafficGeneratorMsg::copy(const TrafficGeneratorMsg& other)
{
    this->bundlesNumber = other.bundlesNumber;
    this->destinationEid = other.destinationEid;
    this->size = other.size;
    this->ttl = other.ttl;
    this->interval = other.interval;
}

void TrafficGeneratorMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->bundlesNumber);
    doParsimPacking(b,this->destinationEid);
    doParsimPacking(b,this->size);
    doParsimPacking(b,this->ttl);
    doParsimPacking(b,this->interval);
}

void TrafficGeneratorMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->bundlesNumber);
    doParsimUnpacking(b,this->destinationEid);
    doParsimUnpacking(b,this->size);
    doParsimUnpacking(b,this->ttl);
    doParsimUnpacking(b,this->interval);
}

int TrafficGeneratorMsg::getBundlesNumber() const
{
    return this->bundlesNumber;
}

void TrafficGeneratorMsg::setBundlesNumber(int bundlesNumber)
{
    this->bundlesNumber = bundlesNumber;
}

int TrafficGeneratorMsg::getDestinationEid() const
{
    return this->destinationEid;
}

void TrafficGeneratorMsg::setDestinationEid(int destinationEid)
{
    this->destinationEid = destinationEid;
}

int TrafficGeneratorMsg::getSize() const
{
    return this->size;
}

void TrafficGeneratorMsg::setSize(int size)
{
    this->size = size;
}

int TrafficGeneratorMsg::getTtl() const
{
    return this->ttl;
}

void TrafficGeneratorMsg::setTtl(int ttl)
{
    this->ttl = ttl;
}

int TrafficGeneratorMsg::getInterval() const
{
    return this->interval;
}

void TrafficGeneratorMsg::setInterval(int interval)
{
    this->interval = interval;
}

class TrafficGeneratorMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    TrafficGeneratorMsgDescriptor();
    virtual ~TrafficGeneratorMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(TrafficGeneratorMsgDescriptor);

TrafficGeneratorMsgDescriptor::TrafficGeneratorMsgDescriptor() : omnetpp::cClassDescriptor("TrafficGeneratorMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

TrafficGeneratorMsgDescriptor::~TrafficGeneratorMsgDescriptor()
{
    delete[] propertynames;
}

bool TrafficGeneratorMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TrafficGeneratorMsg *>(obj)!=nullptr;
}

const char **TrafficGeneratorMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *TrafficGeneratorMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int TrafficGeneratorMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount() : 5;
}

unsigned int TrafficGeneratorMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *TrafficGeneratorMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "bundlesNumber",
        "destinationEid",
        "size",
        "ttl",
        "interval",
    };
    return (field>=0 && field<5) ? fieldNames[field] : nullptr;
}

int TrafficGeneratorMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='b' && strcmp(fieldName, "bundlesNumber")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationEid")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "size")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "ttl")==0) return base+3;
    if (fieldName[0]=='i' && strcmp(fieldName, "interval")==0) return base+4;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *TrafficGeneratorMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : nullptr;
}

const char **TrafficGeneratorMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TrafficGeneratorMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TrafficGeneratorMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string TrafficGeneratorMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getBundlesNumber());
        case 1: return long2string(pp->getDestinationEid());
        case 2: return long2string(pp->getSize());
        case 3: return long2string(pp->getTtl());
        case 4: return long2string(pp->getInterval());
        default: return "";
    }
}

bool TrafficGeneratorMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setBundlesNumber(string2long(value)); return true;
        case 1: pp->setDestinationEid(string2long(value)); return true;
        case 2: pp->setSize(string2long(value)); return true;
        case 3: pp->setTtl(string2long(value)); return true;
        case 4: pp->setInterval(string2long(value)); return true;
        default: return false;
    }
}

const char *TrafficGeneratorMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *TrafficGeneratorMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(ContactMsg);

ContactMsg::ContactMsg(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->id = 0;
    this->dataRate = 0;
    this->start = 0;
    this->end = 0;
    this->duration = 0;
    this->sourceEid = 0;
    this->destinationEid = 0;
}

ContactMsg::ContactMsg(const ContactMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ContactMsg::~ContactMsg()
{
}

ContactMsg& ContactMsg::operator=(const ContactMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ContactMsg::copy(const ContactMsg& other)
{
    this->id = other.id;
    this->dataRate = other.dataRate;
    this->start = other.start;
    this->end = other.end;
    this->duration = other.duration;
    this->sourceEid = other.sourceEid;
    this->destinationEid = other.destinationEid;
}

void ContactMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->id);
    doParsimPacking(b,this->dataRate);
    doParsimPacking(b,this->start);
    doParsimPacking(b,this->end);
    doParsimPacking(b,this->duration);
    doParsimPacking(b,this->sourceEid);
    doParsimPacking(b,this->destinationEid);
}

void ContactMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->id);
    doParsimUnpacking(b,this->dataRate);
    doParsimUnpacking(b,this->start);
    doParsimUnpacking(b,this->end);
    doParsimUnpacking(b,this->duration);
    doParsimUnpacking(b,this->sourceEid);
    doParsimUnpacking(b,this->destinationEid);
}

int ContactMsg::getId() const
{
    return this->id;
}

void ContactMsg::setId(int id)
{
    this->id = id;
}

double ContactMsg::getDataRate() const
{
    return this->dataRate;
}

void ContactMsg::setDataRate(double dataRate)
{
    this->dataRate = dataRate;
}

::omnetpp::simtime_t ContactMsg::getStart() const
{
    return this->start;
}

void ContactMsg::setStart(::omnetpp::simtime_t start)
{
    this->start = start;
}

::omnetpp::simtime_t ContactMsg::getEnd() const
{
    return this->end;
}

void ContactMsg::setEnd(::omnetpp::simtime_t end)
{
    this->end = end;
}

::omnetpp::simtime_t ContactMsg::getDuration() const
{
    return this->duration;
}

void ContactMsg::setDuration(::omnetpp::simtime_t duration)
{
    this->duration = duration;
}

int ContactMsg::getSourceEid() const
{
    return this->sourceEid;
}

void ContactMsg::setSourceEid(int sourceEid)
{
    this->sourceEid = sourceEid;
}

int ContactMsg::getDestinationEid() const
{
    return this->destinationEid;
}

void ContactMsg::setDestinationEid(int destinationEid)
{
    this->destinationEid = destinationEid;
}

class ContactMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ContactMsgDescriptor();
    virtual ~ContactMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ContactMsgDescriptor);

ContactMsgDescriptor::ContactMsgDescriptor() : omnetpp::cClassDescriptor("ContactMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

ContactMsgDescriptor::~ContactMsgDescriptor()
{
    delete[] propertynames;
}

bool ContactMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ContactMsg *>(obj)!=nullptr;
}

const char **ContactMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ContactMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ContactMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount() : 7;
}

unsigned int ContactMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *ContactMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "id",
        "dataRate",
        "start",
        "end",
        "duration",
        "sourceEid",
        "destinationEid",
    };
    return (field>=0 && field<7) ? fieldNames[field] : nullptr;
}

int ContactMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "id")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "dataRate")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "start")==0) return base+2;
    if (fieldName[0]=='e' && strcmp(fieldName, "end")==0) return base+3;
    if (fieldName[0]=='d' && strcmp(fieldName, "duration")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceEid")==0) return base+5;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationEid")==0) return base+6;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ContactMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "simtime_t",
        "simtime_t",
        "simtime_t",
        "int",
        "int",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : nullptr;
}

const char **ContactMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ContactMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ContactMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ContactMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getId());
        case 1: return double2string(pp->getDataRate());
        case 2: return simtime2string(pp->getStart());
        case 3: return simtime2string(pp->getEnd());
        case 4: return simtime2string(pp->getDuration());
        case 5: return long2string(pp->getSourceEid());
        case 6: return long2string(pp->getDestinationEid());
        default: return "";
    }
}

bool ContactMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setId(string2long(value)); return true;
        case 1: pp->setDataRate(string2double(value)); return true;
        case 2: pp->setStart(string2simtime(value)); return true;
        case 3: pp->setEnd(string2simtime(value)); return true;
        case 4: pp->setDuration(string2simtime(value)); return true;
        case 5: pp->setSourceEid(string2long(value)); return true;
        case 6: pp->setDestinationEid(string2long(value)); return true;
        default: return false;
    }
}

const char *ContactMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *ContactMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(FreeChannelMsg);

FreeChannelMsg::FreeChannelMsg(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->neighborEid = 0;
    this->contactId = 0;
}

FreeChannelMsg::FreeChannelMsg(const FreeChannelMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

FreeChannelMsg::~FreeChannelMsg()
{
}

FreeChannelMsg& FreeChannelMsg::operator=(const FreeChannelMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void FreeChannelMsg::copy(const FreeChannelMsg& other)
{
    this->neighborEid = other.neighborEid;
    this->contactId = other.contactId;
}

void FreeChannelMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->neighborEid);
    doParsimPacking(b,this->contactId);
}

void FreeChannelMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->neighborEid);
    doParsimUnpacking(b,this->contactId);
}

int FreeChannelMsg::getNeighborEid() const
{
    return this->neighborEid;
}

void FreeChannelMsg::setNeighborEid(int neighborEid)
{
    this->neighborEid = neighborEid;
}

int FreeChannelMsg::getContactId() const
{
    return this->contactId;
}

void FreeChannelMsg::setContactId(int contactId)
{
    this->contactId = contactId;
}

class FreeChannelMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    FreeChannelMsgDescriptor();
    virtual ~FreeChannelMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(FreeChannelMsgDescriptor);

FreeChannelMsgDescriptor::FreeChannelMsgDescriptor() : omnetpp::cClassDescriptor("FreeChannelMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

FreeChannelMsgDescriptor::~FreeChannelMsgDescriptor()
{
    delete[] propertynames;
}

bool FreeChannelMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<FreeChannelMsg *>(obj)!=nullptr;
}

const char **FreeChannelMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *FreeChannelMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int FreeChannelMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int FreeChannelMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *FreeChannelMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "neighborEid",
        "contactId",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int FreeChannelMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "neighborEid")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "contactId")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *FreeChannelMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **FreeChannelMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *FreeChannelMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int FreeChannelMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    FreeChannelMsg *pp = (FreeChannelMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string FreeChannelMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    FreeChannelMsg *pp = (FreeChannelMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getNeighborEid());
        case 1: return long2string(pp->getContactId());
        default: return "";
    }
}

bool FreeChannelMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    FreeChannelMsg *pp = (FreeChannelMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setNeighborEid(string2long(value)); return true;
        case 1: pp->setContactId(string2long(value)); return true;
        default: return false;
    }
}

const char *FreeChannelMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *FreeChannelMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    FreeChannelMsg *pp = (FreeChannelMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(IonTrafficGeneratorMsg);

IonTrafficGeneratorMsg::IonTrafficGeneratorMsg(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
}

IonTrafficGeneratorMsg::IonTrafficGeneratorMsg(const IonTrafficGeneratorMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

IonTrafficGeneratorMsg::~IonTrafficGeneratorMsg()
{
}

IonTrafficGeneratorMsg& IonTrafficGeneratorMsg::operator=(const IonTrafficGeneratorMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void IonTrafficGeneratorMsg::copy(const IonTrafficGeneratorMsg& other)
{
}

void IonTrafficGeneratorMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
}

void IonTrafficGeneratorMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
}

class IonTrafficGeneratorMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    IonTrafficGeneratorMsgDescriptor();
    virtual ~IonTrafficGeneratorMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(IonTrafficGeneratorMsgDescriptor);

IonTrafficGeneratorMsgDescriptor::IonTrafficGeneratorMsgDescriptor() : omnetpp::cClassDescriptor("IonTrafficGeneratorMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

IonTrafficGeneratorMsgDescriptor::~IonTrafficGeneratorMsgDescriptor()
{
    delete[] propertynames;
}

bool IonTrafficGeneratorMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<IonTrafficGeneratorMsg *>(obj)!=nullptr;
}

const char **IonTrafficGeneratorMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *IonTrafficGeneratorMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int IonTrafficGeneratorMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int IonTrafficGeneratorMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *IonTrafficGeneratorMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int IonTrafficGeneratorMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *IonTrafficGeneratorMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **IonTrafficGeneratorMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *IonTrafficGeneratorMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int IonTrafficGeneratorMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    IonTrafficGeneratorMsg *pp = (IonTrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string IonTrafficGeneratorMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    IonTrafficGeneratorMsg *pp = (IonTrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool IonTrafficGeneratorMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    IonTrafficGeneratorMsg *pp = (IonTrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *IonTrafficGeneratorMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *IonTrafficGeneratorMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    IonTrafficGeneratorMsg *pp = (IonTrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


