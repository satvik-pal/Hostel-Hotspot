#ifndef MAINTENANCE_MANAGER_H
#define MAINTENANCE_MANAGER_H

#include "Dijkstra.h"
#include "DynamicArray.h"
#include "HashMap.h"
#include "MaintenanceWorker.h"
#include <optional>
#include <string>

// Result of a distance-aware allocation: who was chosen and how they get there.
struct AllocationResult
{
    std::string workerId;
    RouteResult route;          // route.reachable is false if a location is unknown or unreachable
};

class MaintenanceManager
{
    private:
        HashMap<MaintenanceWorker> workers;       // workerId    -> worker
        HashMap<std::string>       assignments;   // complaintId -> workerId
        int workerCounter;

        std::string generateWorkerId();
        static bool isBetterCandidate(const MaintenanceWorker& candidate,
                                      const MaintenanceWorker& currentBest);
        static bool isBetterCandidate(const MaintenanceWorker& candidate, int candidateDistance,
                                      const MaintenanceWorker& currentBest, int bestDistance);

    public:
        MaintenanceManager();

        // Worker management
        std::string addWorker(const std::string& name,
                              const std::string& skill,
                              const std::string& location);
        MaintenanceWorker* findWorker(const std::string& workerId);
        bool removeWorker(const std::string& workerId);
        int getWorkerCount() const;

        // Allocation (workload only)
        std::optional<std::string> allocateComplaint(const std::string& complaintId,
                                                     const std::string& requiredSkill);
        bool completeComplaint(const std::string& complaintId);
        std::optional<std::string> getAssignedWorker(const std::string& complaintId) const;

        // Allocation (workload first, then distance) + route to the complaint.
        // Graph must also provide: int findVertex(const std::string& name) const;  (-1 if unknown)
        template <typename Graph>
        std::optional<AllocationResult> allocateWithRoute(const std::string& complaintId,
                                                          const std::string& requiredSkill,
                                                          const std::string& complaintLocation,
                                                          const Graph& graph);
};

// Template member functions must be defined in the header.
template <typename Graph>
std::optional<AllocationResult> MaintenanceManager::allocateWithRoute(const std::string& complaintId,
                                                                      const std::string& requiredSkill,
                                                                      const std::string& complaintLocation,
                                                                      const Graph& graph)
{
    if (assignments.contains(complaintId))
    {
        return std::nullopt;                    // already assigned
    }

    // One Dijkstra run from the complaint gives the distance to every location.
    // The hostel graph is two-way, so complaint -> worker equals worker -> complaint.
    int complaintVertex = graph.findVertex(complaintLocation);
    DynamicArray<int> distanceTo(graph.getVertexCount(), UNREACHABLE_DISTANCE);
    if (complaintVertex != -1)
    {
        distanceTo = computeDistancesFrom(graph, complaintVertex);
    }

    MaintenanceWorker* best = nullptr;
    int bestDistance = UNREACHABLE_DISTANCE;

    workers.forEach([&](const std::string& workerId, MaintenanceWorker& worker)
    {
        (void)workerId;

        if (worker.getSkill() != requiredSkill || !worker.isAvailable())
        {
            return;
        }

        int workerVertex = graph.findVertex(worker.getLocation());
        int distance = (workerVertex == -1) ? UNREACHABLE_DISTANCE : distanceTo[workerVertex];

        if (best == nullptr || isBetterCandidate(worker, distance, *best, bestDistance))
        {
            best = &worker;
            bestDistance = distance;
        }
    });

    if (best == nullptr)
    {
        return std::nullopt;                    // nobody suitable
    }

    best->assignComplaint();
    assignments.insert(complaintId, best->getWorkerId());

    AllocationResult result;
    result.workerId = best->getWorkerId();

    int workerVertex = graph.findVertex(best->getLocation());
    if (workerVertex != -1 && complaintVertex != -1)
    {
        result.route = findShortestRoute(graph, workerVertex, complaintVertex);
    }

    return result;
}

#endif