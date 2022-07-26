#include <zmq.h>
#include "RoutingHdtn.h"

RoutingHdtn::RoutingHdtn(int eid, SdrModel * sdr, ContactPlan * contactPlan)
: RoutingDeterministic(eid, sdr, contactPlan)
{
	createRouterConfigFile();
}

RoutingHdtn::~RoutingHdtn()
{
}

void RoutingHdtn::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	RouterListener r = RouterListener(HDTN_BOUND_ROUTER_PUBSUB_PATH);

	std::string execString(
			"hdtn-router --contact-plan-file=" + this->cpFile +
			" --dest-uri-eid=ipn:" + bundle->destinationEid + ".1" +
			" --hdtn-config-file=" + this->configFile +
			" & router_PID=$!");

	system(execString);
	while (!r.check()); // wait to receive message from router
	system("kill -2 $router_PID");
	enqueue(bundle, r.getNextHop());
}

void RoutingHdtn::contactStart(Contact * c)
{
	sdr_->transferFromNode(Contact * c);
}

void RoutingHdtn::enqueue(BundlePkt * bundle, int neighborNodeNbr)
{
	bundle->setNextHopEid(neighborNodeNbr);
	sdr_->enqueueBundleToNode(bundle, neighborNodeNbr);
}

void RoutingHdtn::createRouterConfigFile()
{
	// for now makes these constant
	// later they will be parameterized or generated
	this->cpFile = std::string("~/hdtn/module/scheduler/src/contactPlan.json");
	this->configFile = std::string("~/hdtn/tests/config_files/hdtn/hdtn_ingress1tcpcl_port4556_egress2tcpcl_port4557flowid1_port4558flowid2.json");
}

RouterListener::RouterListener(int port)
: port(port)
{
}

RouterListener::~RouterListener()
: port(HDTN_BOUND_ROUTER_PUBSUB_PATH)
{
}

bool RouterListener::check()
{
	zmq::context_t ctx;
	zmq::socket_t sock(ctx, zmq::socket_type::sub);
	std::string path(std::string(
			"tcp://localhost") +
			std::lexical_cast<std::string>(this->port));
	sock.connect(path);
	zmq::pollitem_t items[] = {{sock.handle(), 0, ZMQ_POLLIN, 0}};

	int rc = zmq::poll(&items[0], 1, ZMQ_POLL_TIMEOUT);

	if (rc > 0) {
		if (items[0].revents & ZMQ_POLLIN) {
			zmq::message_t message;
			if (!sock->recv(message, zmq::recv_flags::none)) {
				return false;
			}
			if (message.size() < sizeof(CommonHdr)) {
				return false;
			}

			CommonHdr *common = (CommonHdr *)message.data();
			switch (common->type) {
				case HDTN_MSGTYPE_ROUTEUPDATE:
				hdtn::RouteUpdateHdr * routeUpdateHdr = (hdtn::RouteUpdateHdr *)message.data();
				cbhe_eid_t nextHopEid = routeUpdateHdr->nextHopEid;
				cbhe_eid_t finalDestEid = routeUpdateHdr->finalDestEid;
				nextHop = nextHopEid.nodeId;
				finalDest = finalDestEid.nodeId;
				return true;
			}
		}
	}
	return false;
}

int RouterListener::getNextHop()
{
	return nextHop;
}

int RouterListener::getFinalDest()
{
	return finalDest;
}









