#include "RoutingHdtn.h"
#include <sys/wait.h>

RoutingHdtn::RoutingHdtn(int eid, SdrModel * sdr, ContactPlan * contactPlan, std::string * path)
: RoutingDeterministic(eid, sdr, contactPlan)
{
	this->hdtnPath = std::string(*path);
	createRouterConfigFile();
}

RoutingHdtn::~RoutingHdtn()
{
}

void RoutingHdtn::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	// connect a listener
	RouterListener listener = RouterListener(HDTN_BOUND_ROUTER_PUBSUB_PATH);
	listener.connect();

	// run HDTN router
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	chdir(this->hdtnPath.c_str());
	std::string execString(
		std::string("hdtn-router --contact-plan-file=") + this->cpFile +
		std::string(" --dest-uri-eid=ipn:") + std::to_string(bundle->getDestinationEid()) + std::string(".1") +
		std::string(" --hdtn-config-file=") + this->configFile +
//			std::string(""));
		std::string(" & router_PID=$! && sleep 1 && kill -2 $router_PID"));
	std::cout << "[RoutingHdtn] Running command: " << std::endl << execString << std::endl;
	system(execString.c_str());
	chdir(cwd);
	// wait to receive message from router
	while (!listener.check());

	// disconnect, kill, and enqueue the bundle
	listener.disconnect();
//	system("kill $router_PID");
	enqueue(bundle, listener.getNextHop());
	std::cout << "[RoutingHdtn] enqueued bundle" << std::endl;
}

void RoutingHdtn::contactStart(Contact * c)
{
	sdr_->transferToContact(c);
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
	std::cout << "[RoutingHdtn] creating configs for node " << this->eid_ << std::endl;
	if (this->eid_ == 1) {
		this->cpFile = std::string("contactPlan_RoutingTest.json");
		this->configFile = std::string(
				this->hdtnPath +
				std::string("test_scripts_linux/Routing_Test/node1/hdtn_node1_cfg.json"));
	} else if (this->eid_ == 2) {
		this->cpFile = std::string("contactPlan_RoutingTest.json");
		this->configFile = std::string(
				this->hdtnPath +
				std::string("test_scripts_linux/Routing_Test/node2/hdtn_node2_cfg.json"));
	} else if (this->eid_ == 10) {
		this->cpFile = std::string("contactPlan.json");
		this->configFile = std::string(
				this->hdtnPath +
				std::string("tests/config_files/hdtn/hdtn_ingress1tcpcl_port4556_egress2tcpcl_port4557flowid1_port4558flowid2.json"));
	}
}

RouterListener::RouterListener(int port)
: port(port)
{
	this->path = std::string(
			std::string("tcp://localhost:") +
			std::to_string(port));
}

RouterListener::~RouterListener()
{
	if (this->connected) {
		disconnect();
	}
}

void RouterListener::connect()
{
	try {
		this->ctx = new zmq::context_t();
		this->sock = new zmq::socket_t(*ctx, zmq::socket_type::sub);
		this->sock->connect(this->path);
		this->sock->setsockopt(ZMQ_SUBSCRIBE, "", strlen(""));
		std::cout << "[listener] connected" << std::endl;
	} catch (const zmq::error_t &ex) {
		std::cerr << ex.what() << std::endl;
	}

	this->connected = true;
	std::cout << "[listener] connected" << std::endl;
}

void RouterListener::disconnect()
{
	this->sock->disconnect(this->path);
	delete this->sock;
	delete this->ctx;
	this->connected = false;
	std::cout << "[listener] disconnected" << std::endl;
}

bool RouterListener::check()
{
	zmq::pollitem_t items[] = {{this->sock->handle(), 0, ZMQ_POLLIN, 0}};
	std::cout << "[listener] polling at " << this->path << std::endl;

	int rc = zmq::poll(&items[0], 1, ZMQ_POLL_TIMEOUT);
	assert(rc >= 0);

	if (rc > 0) {
		std::cout << "[listener] received message from HDTN!" << std::endl;
		if (items[0].revents & ZMQ_POLLIN) {
			std::cout << "[listener] event from router" << std::endl;
			zmq::message_t message;
			if (!sock->recv(message, zmq::recv_flags::none)) {
				std::cout << "[listener] ?????" << std::endl;
				return false;
			}

			if (message.size() < sizeof(CommonHdr)) {
				std::cout << "[listener] unknown message type" << std::endl;
				return false;
			}

			CommonHdr *common = (CommonHdr *)message.data();
			switch (common->type) {
				case HDTN_MSGTYPE_ROUTEUPDATE:
				{
					std::cout << "[listener] received route update" << std::endl;
					RouteUpdateHdr * routeUpdateHdr = (RouteUpdateHdr *)message.data();
					cbhe_eid_t nextHopEid = routeUpdateHdr->nextHopEid;
					cbhe_eid_t finalDestEid = routeUpdateHdr->finalDestEid;
					nextHop = nextHopEid.nodeId;
					finalDest = finalDestEid.nodeId;
					return true;
				}
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









