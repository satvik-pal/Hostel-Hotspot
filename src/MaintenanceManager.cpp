#include "MaintenanceManager.h"

#include <iomanip>
#include <sstream>

MaintenanceManager::MaintenanceManager()
    : workerCounter(1)
{
}

// ---------- Worker management ----------

std::string MaintenanceManager::generateWorkerId()
{
    std::stringstream idStream;
    idStream << "W" << std::setfill('0') << std::setw(3) << workerCounter;
    workerCounter++;
    return idStream.str();
}

std::string MaintenanceManager::addWorker(const std::string& name,
                                          const std::string& skill,
                                          const std::string& location)
{
    std::string workerId = generateWorkerId();

    MaintenanceWorker worker(workerId, name, skill, location, true, 0);
    workers.insert(workerId, worker);

    return workerId;
}

MaintenanceWorker* MaintenanceManager::findWorker(const std::string& workerId)
{
    return workers.find(workerId);
}

bool MaintenanceManager::removeWorker(const std::string& workerId)
{
    MaintenanceWorker* worker = workers.find(workerId);

    if (worker == nullptr)
    {
        return false;                       // no such worker
    }
    if (worker->getWorkload() > 0)
    {
        return false;                       // still has open complaints
    }

    return workers.erase(workerId);
}

int MaintenanceManager::getWorkerCount() const
{
    return workers.size();
}

// ---------- Allocation ----------

bool MaintenanceManager::isBetterCandidate(const MaintenanceWorker& candidate,
                                           const MaintenanceWorker& currentBest)
{
    if (candidate.getWorkload() != currentBest.getWorkload())
    {
        return candidate.getWorkload() < currentBest.getWorkload();
    }
    return candidate.getWorkerId() < currentBest.getWorkerId();   // tie-break
}

std::optional<std::string> MaintenanceManager::allocateComplaint(const std::string& complaintId,
                                                                 const std::string& requiredSkill)
{
    if (assignments.contains(complaintId))
    {
        return std::nullopt;                // already assigned
    }

    MaintenanceWorker* best = nullptr;

    workers.forEach([&](const std::string& workerId, MaintenanceWorker& worker)
    {
        (void)workerId;                     // key not needed here

        if (worker.getSkill() != requiredSkill)
        {
            return;
        }
        if (!worker.isAvailable())
        {
            return;
        }
        if (best == nullptr || isBetterCandidate(worker, *best))
        {
            best = &worker;
        }
    });

    if (best == nullptr)
    {
        return std::nullopt;                // nobody suitable
    }

    best->assignComplaint();
    assignments.insert(complaintId, best->getWorkerId());

    return best->getWorkerId();
}

bool MaintenanceManager::completeComplaint(const std::string& complaintId)
{
    std::string* assignedId = assignments.find(complaintId);

    if (assignedId == nullptr)
    {
        return false;                       // never assigned
    }

    MaintenanceWorker* worker = workers.find(*assignedId);
    if (worker != nullptr)
    {
        worker->completeComplaint();
    }

    assignments.erase(complaintId);         // assignedId is invalid after this line
    return true;
}

std::optional<std::string> MaintenanceManager::getAssignedWorker(const std::string& complaintId) const
{
    const std::string* assignedId = assignments.find(complaintId);

    if (assignedId == nullptr)
    {
        return std::nullopt;
    }
    return *assignedId;
}