#ifndef SRC_NODE_DTN_ROUTINGHDTN_H_
#define SRC_NODE_DTN_ROUTINGHDTN_H_

#include <zmq.hpp>
#include <src/node/dtn/routing/RoutingDeterministic.h>
#include <src/node/dtn/SdrModel.h>

#define HDTN_ROUTER_ADDRESS "localhost"
#define HDTN_BOUND_ROUTER_PUBSUB_PATH 10210
#define HDTN_MSGTYPE_ROUTEUPDATE (0xFC07) //Route Update Event from Router process
#define ZMQ_POLL_TIMEOUT 250

struct cbhe_eid_t {
    uint64_t nodeId;
    uint64_t serviceId;
};

struct CommonHdr {
    uint16_t type;
    uint16_t flags;
};

struct RouteUpdateHdr {
    CommonHdr base;
    uint8_t unused3;
    uint8_t unused4;
    cbhe_eid_t nextHopEid;
    cbhe_eid_t finalDestEid;
    uint64_t route[20]; //optimal route
};

class RoutingHdtn : public RoutingDeterministic
{
public:
  RoutingHdtn(int eid, SdrModel *sdr, ContactPlan *contactPlan, std::string *path);
  virtual ~RoutingHdtn();
  virtual void routeAndQueueBundle(BundlePkt* bundle, double simTime);
  virtual void contactStart(Contact *c);
private:
  //time_t startUtcTime_;
  std::string hdtnPath;
  std::string cpFile;
  std::string configFile;
  virtual void enqueue(BundlePkt *bundle, int neighborNodeNbr);
  virtual void createRouterConfigFile();
  //virtual time_t getUtcSimulationTime(double simTime);
};

class RouterListener
{
public:
  RouterListener(int port);
  ~RouterListener();
  void connect();
  void disconnect();
  bool check();
  int getNextHop();
  int getFinalDest();
private:
  int port;
  std::string path;
  zmq::socket_t *sock;
  zmq::context_t *ctx;
  bool connected;
  int nextHop;
  int finalDest;
};

#endif /* SRC_NODE_DTN_ROUTINGHDTN_H_ */
