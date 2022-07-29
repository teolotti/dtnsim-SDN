#include "RoutingHdtn.h"

RoutingHdtn::RoutingHdtn(int eid, SdrModel * sdr, ContactPlan * contactPlan, string * path, string * cpJson)
: RoutingDeterministic(eid, sdr, contactPlan)
{
	this->hdtnSourceRoot = string(*path);
	this->cpFile = string(*cpJson);
	createRouterConfigFile();
}

RoutingHdtn::~RoutingHdtn()
{
}

void RoutingHdtn::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	// connect a listener
//	RouterListener listener = RouterListener(HDTN_BOUND_ROUTER_PUBSUB_PATH + (this->eid_ - 1));
	RouterListener listener = RouterListener(HDTN_BOUND_ROUTER_PUBSUB_PATH);
	listener.connect();

	string hdtnExec = this->hdtnSourceRoot + "/build/module/router/hdtn-router";
	// run HDTN router
//	char cwd[1024];
//	getcwd(cwd, sizeof(cwd));
//	chdir(this->hdtnSourceRoot.c_str());
	string execString(
//		this->hdtnSourceRoot + string("/build/module/router/hdtn-router") +
		hdtnExec +
		string(" --contact-plan-file=") + this->cpFile +
		string(" --dest-uri-eid=ipn:") + to_string(bundle->getDestinationEid()) + string(".1") +
		string(" --hdtn-config-file=") + this->configFile +
//			string(""));
		string(" & router_PID=$! && sleep 1 && kill -2 $router_PID"));
	cout << "[RoutingHdtn] Running command: " << endl << execString << endl;
	system(execString.c_str());
//	chdir(cwd);
	// wait to receive message from router
	while (!listener.check());

	// disconnect, kill, and enqueue the bundle
	listener.disconnect();
//	system("kill $router_PID");
	enqueue(bundle, listener.getNextHop());
	cout << "[RoutingHdtn] enqueued bundle" << endl;
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
	string command = "mkdir " + path;
	system(command.c_str());
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
	this->path = string(
			string("tcp://localhost:") +
			to_string(port));
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
	cout << "[listener] polling at " << this->path << endl;

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









