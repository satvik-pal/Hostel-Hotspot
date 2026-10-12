#ifndef MAINTENANCE_MANAGER_H
#define MAINTENANCE_MANAGER_H

#include "MaintenanceWorker.h"
#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>

class MaintenanceManager
{
    private:
        std::unordered_map<std::string, MaintenanceWorker> workers;      // workerId    -> worker
        std::unordered_map<std::string, std::string>       assignments;  // complaintId -> workerId
        int workerCounter;

        std::string generateWorkerId();
        static bool isBetterCandidate(const MaintenanceWorker& candidate, const MaintenanceWorker& currentBest);

    public:
        MaintenanceManager();

        // Worker management

        std::string addWorker(const std::string& name,
                              const std::string& skill,
                              const std::string& location);
        MaintenanceWorker* findWorker(const std::string& workerId);
        bool removeWorker(const std::string& workerId);
        std::size_t getWorkerCount() const;

        // Allocation

        std::optional<std::string> allocateComplaint(const std::string& complaintId, const std::string& requiredSkill);
        bool completeComplaint(const std::string& complaintId);
        std::optional<std::string> getAssignedWorker(const std::string& complaintId) const;
};

#endif