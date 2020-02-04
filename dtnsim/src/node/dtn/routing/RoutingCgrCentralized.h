#ifndef SRC_NODE_DTN_ROUTINGCGRCENTRALIZED_H_
#define SRC_NODE_DTN_ROUTINGCGRCENTRALIZED_H_

#include <src/node/dtn/routing/CgrRoute.h>
#include <src/node/dtn/routing/RoutingDeterministic.h>

class RoutingCgrCentralized : public RoutingDeterministic
{
public:
    RoutingCgrCentralized(int eid, int neighborsNum, SdrModel *sdr, ContactPlan *localContactPlan, string routingType);
    virtual ~RoutingCgrCentralized();
    void initializeRouteTable();

    int getDijkstraCalls();
    vector<int> getRouteLengthVector();

private:
    void cgrForward(BundlePkt * bundle);
    void routeAndQueueBundle(BundlePkt * bundle, double simTime);
    void cgrEnqueue(BundlePkt * bundle, CgrRoute * bestRoute);
    static bool compareRoutes(CgrRoute i, CgrRoute j);
    void findNextBestRoute(vector<int> suppressedContactIds, int terminusNode, CgrRoute * route);

    // stats
    int dijkstraCalls_;
    int dijkstraLoops_;
    vector<int> routeLengthVector_;

    int neighborsNum_;
    string routingType_;
    vector<vector<CgrRoute>> routeTable_;
    double simTime_;

    typedef struct {
        Contact * predecessor;      // Predecessor Contact
        double arrivalTime;         // Dijkstra exploration: best arrival time so far
        bool visited;               // Dijkstra exploration: visited
        bool suppressed;            // Dijkstra exploration: suppressed

        bool operator()(Contact const *a, Contact const *b) {
            return ((Work *) a->work)->arrivalTime > ((Work *) b->work)->arrivalTime;
        }
    } Work;
};

#endif
