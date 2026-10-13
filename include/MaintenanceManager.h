#ifndef MAINTENANCE_MANAGER_H
#define MAINTENANCE_MANAGER_H

#include "HashMap.h"
#include "MaintenanceWorker.h"
#include <optional>
#include <string>

class MaintenanceManager
{
    private:
        HashMap<MaintenanceWorker> workers;       // workerId    -> worker
        HashMap<std::string>       assignments;   // complaintId -> workerId
        int workerCounter;

        std::string generateWorkerId();
        static bool isBetterCandidate(const MaintenanceWorker& candidate,
                                      const MaintenanceWorker& currentBest);

    public:
        MaintenanceManager();

        // Worker management
        std::string addWorker(const std::string& name,
                              const std::string& skill,
                              const std::string& location);
        MaintenanceWorker* findWorker(const std::string& workerId);
        bool removeWorker(const std::string& workerId);
        int getWorkerCount() const;

        // Allocation
        std::optional<std::string> allocateComplaint(const std::string& complaintId,
                                                     const std::string& requiredSkill);
        bool completeComplaint(const std::string& complaintId);
        std::optional<std::string> getAssignedWorker(const std::string& complaintId) const;
};

#endif