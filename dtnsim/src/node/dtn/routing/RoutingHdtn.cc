#include "RoutingHdtn.h"
#include "src/hdtn/libcgr.h"
#include <signal.h>
#include <cmath>

#define ZMQ_POLL_TIMEOUT 50

RoutingHdtn::RoutingHdtn(int eid, SdrModel * sdr, ContactPlan * contactPlan, string * path, string * cpJson, bool useHdtnRouter)
: RoutingDeterministic(eid, sdr, contactPlan)
{
	this->hdtnSourceRoot = string(*path);
	this->cpFile = string(*cpJson);
	if (useHdtnRouter) {
		this->route_fn = &RoutingHdtn::routeHdtn;
		createRouterConfigFile();
	} else {
		this->route_fn = &RoutingHdtn::routeLibcgr;
	}
}

RoutingHdtn::~RoutingHdtn()
{
}

int RoutingHdtn::routeHdtn(BundlePkt * bundle) {
	// connect a listener
//	RouterListener listener = RouterListener(HDTN_BOUND_ROUTER_PUBSUB_PATH + (this->eid_ - 1));
	RouterListener listener = RouterListener(HDTN_BOUND_ROUTER_PUBSUB_PATH);
	listener.connect();

	// run HDTN router
	string hdtnExec = this->hdtnSourceRoot + "/build/module/router/hdtn-router";
	string execString(
		hdtnExec +
		string(" --contact-plan-file=") + this->cpFile +
		string(" --dest-uri-eid=ipn:") + to_string(bundle->getDestinationEid()) + string(".1") +
		string(" --hdtn-config-file=") + this->configFile +
		string(" &"));

	cout << "[RoutingHdtn] Running command: " << endl << execString << endl;
	system(execString.c_str());

	// wait to receive message from router
	while (!listener.check());

	// done with listener
	listener.disconnect();

	// kill any HDTN process spawned
	char pidline[16];
	FILE *cmd = popen("pidof hdtn-router", "r");
	fgets(pidline, 16, cmd);
	pid_t pid = strtoul(pidline, NULL, 10);
	kill(pid, SIGTERM);
	pclose(cmd);

	return listener.getNextHop();
}

int RoutingHdtn::routeLibcgr(BundlePkt * bundle) {
	string jsonEventFileName = this->hdtnSourceRoot + "/module/router/src/" + this->cpFile;
    vector<cgr::Contact> contactPlan = cgr::cp_load(jsonEventFileName);
    cgr::Contact rootContact = cgr::Contact(this->eid_, this->eid_, 0, cgr::MAX_SIZE, 100, 1.0, 0);
	rootContact.arrival_time = (int)ceil(simTime().dbl());
	cgr::Route bestRoute = cgr::dijkstra(&rootContact, bundle->getDestinationEid(), contactPlan);
	return bestRoute.next_node;
}

void RoutingHdtn::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	int nextHop;
	int dest = bundle->getDestinationEid();

	map<int, int>::iterator entry = routeTable.find(dest);
	if (entry == routeTable.end()) {
		nextHop = (this->*route_fn)(bundle);
		routeTable[dest] = nextHop;
	} else {
		nextHop = entry->second;
	}

	// transmit or enqueue
	bool success = attemptTransmission(bundle, nextHop);
	if (success) {
		// an active contact was found so try to transmit on it
		cout << "[RoutingHdtn] placed bundle on outduct to active contact" << endl;
	} else {
		// no active contact found. store and enqueue the bundle
		enqueue(bundle, nextHop);
		cout << "[RoutingHdtn] enqueued bundle to storage" << endl;
	}
}

void RoutingHdtn::contactStart(Contact * c)
{
	sdr_->transferToContact(c);
}

bool RoutingHdtn::attemptTransmission(BundlePkt * bundle, int neighborNodeNbr)
{
	omnetpp::simtime_t time = simTime();
	bundle->setNextHopEid(neighborNodeNbr);
	for (Contact &c : contactPlan_->getContactsBySrcDst(eid_, neighborNodeNbr)) {
		if (c.isActive(time.dbl())) {
			// put bundle directly on active contact
			sdr_->transferToContact(&c, bundle);
			return true;
		}
	}
	return false;
}

void RoutingHdtn::enqueue(BundlePkt * bundle, int neighborNodeNbr)
{
	bundle->setNextHopEid(neighborNodeNbr);
	sdr_->enqueueBundleToNode(bundle, neighborNodeNbr);

}

void RoutingHdtn::createRouterConfigFile()
{
	cout << "[RoutingHdtn] creating configs for node " << this->eid_ << endl;
	char cwd[1024];
	string path;

	getcwd(cwd, sizeof(cwd));
	path = string(cwd) + "/" + "template.json";
	ifstream temp(path);
	if (!temp) {
		cerr << "can't open template" << endl;
	}

	chdir("hdtnFiles");
	getcwd(cwd, sizeof(cwd));
	setenv("HDTN_NODE_LIST_DIR", cwd, 1);

	path = "node" + to_string(this->eid_);
	mkdir(path.c_str(), 0700);
	chdir(path.c_str());

	ofstream file("cfg.json");
	if (!file) {
		cerr << "can't open cfg" << endl;
	}

	file << temp.rdbuf();
	file << "    \"myNodeId\": " << this->eid_ << "," << endl;
	file << "    \"zmqRouterAddress\": \"localhost\"," << endl;
	file << "    \"zmqBoundRouterPubSubPortPath\": " << HDTN_BOUND_ROUTER_PUBSUB_PATH << endl;
	file << "}" << endl;
	temp.close();
	file.close();

	chdir("../../");

	this->configFile = string(cwd) + "/" + path + "/cfg.json";
}

RouterListener::RouterListener(int port)
: port(port)
{
	this->path = string("tcp://localhost:") + to_string(port);
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
		cout << "[listener] connected" << endl;
	} catch (const zmq::error_t &ex) {
		cerr << ex.what() << endl;
	}

	this->connected = true;
	cout << "[listener] connected" << endl;
}

void RouterListener::disconnect()
{
	this->sock->disconnect(this->path);
	delete this->sock;
	delete this->ctx;
	this->connected = false;
	cout << "[listener] disconnected" << endl;
}

bool RouterListener::check()
{
	zmq::pollitem_t items[] = {{this->sock->handle(), 0, ZMQ_POLLIN, 0}};
//	cout << "[listener] polling at " << this->path << endl;

	int rc = zmq::poll(&items[0], 1, ZMQ_POLL_TIMEOUT);
	assert(rc >= 0);

	if (rc > 0) {
		cout << "[listener] received message from HDTN!" << endl;
		if (items[0].revents & ZMQ_POLLIN) {
			cout << "[listener] event from router" << endl;
			zmq::message_t message;
			if (!sock->recv(message, zmq::recv_flags::none)) {
				cout << "[listener] ?????" << endl;
				return false;
			}

			if (message.size() < sizeof(CommonHdr)) {
				cout << "[listener] unknown message type" << endl;
				return false;
			}

			CommonHdr *common = (CommonHdr *)message.data();
			switch (common->type) {
				case HDTN_MSGTYPE_ROUTEUPDATE:
				{
					cout << "[listener] received route update" << endl;
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









