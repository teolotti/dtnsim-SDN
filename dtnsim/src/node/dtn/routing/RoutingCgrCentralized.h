#ifndef SRC_NODE_DTN_ROUTINGCGRCENTRALIZED_H_
#define SRC_NODE_DTN_ROUTINGCGRCENTRALIZED_H_

#include <list>
#include <queue>
#include <src/node/dtn/routing/CgrRoute.h>
#include <src/node/dtn/routing/RoutingDeterministic.h>


class RoutingCgrCentralized : public RoutingDeterministic
{
public:
    RoutingCgrCentralized(int eid, int neighborsNum, SdrModel *sdr, ContactPlan *localContactPlan,
            bool printDebug, string routingType, int maxRouteLength, int maxRoutesWithSameDst,
            double bfsIntervalTime, int bfsIntervalNum);
    virtual ~RoutingCgrCentralized();
    void initializeRouteTable();

    void fillRouteTableWithBfs();
    void findRoutesBfs(double minEndTimeFilter, double* outRouteMinEndTime);
    void fillRouteTableWithFirstEnded();
    void findRoutesDijkstra(Contact** outMinEndContact);

    // stats
    int getComputedRoutes();
    vector<int> getRouteLengthVector();
    double getTimeToComputeRoutes();
    void clearRouteLengthVector();

private:
    void cgrForward(BundlePkt * bundle);
    void routeAndQueueBundle(BundlePkt * bundle, double simTime);
    void cgrEnqueue(BundlePkt * bundle, CgrRoute * bestRoute);

    void createContactsWork();
    void clearContactsWork();

    // stats
    int computedRoutes_;
    double timeToComputeRoutes_;
    vector<int> routeLengthVector_;

    bool printDebug_;
    int neighborsNum_;
    string routingType_;
    vector<vector<CgrRoute>> routeTable_;
    double simTime_;
    int maxRouteHops_;
    int maxRoutesWithSameDst_;
    double bfsIntervalTime_;
    int bfsIntervalNum_;

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
