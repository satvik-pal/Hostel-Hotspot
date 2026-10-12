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
    workers.insert({workerId, worker});

    return workerId;
}

MaintenanceWorker* MaintenanceManager::findWorker(const std::string& workerId)
{
    auto it = workers.find(workerId);

    if (it != workers.end())
    {
        return &it->second;
    }
    return nullptr;
}

bool MaintenanceManager::removeWorker(const std::string& workerId)
{
    auto it = workers.find(workerId);

    if (it == workers.end())
    {
        return false;                       // no such worker
    }
    if (it->second.getWorkload() > 0)
    {
        return false;                       // still has open complaints
    }

    workers.erase(it);
    return true;
}

std::size_t MaintenanceManager::getWorkerCount() const
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
    if (assignments.count(complaintId) > 0)
    {
        return std::nullopt;                // already assigned
    }

    MaintenanceWorker* best = nullptr;

    for (auto& [workerId, worker] : workers)
    {
        if (worker.getSkill() != requiredSkill)
        {
            continue;
        }
        if (!worker.isAvailable())
        {
            continue;
        }
        if (best == nullptr || isBetterCandidate(worker, *best))
        {
            best = &worker;
        }
    }

    if (best == nullptr)
    {
        return std::nullopt;                // nobody suitable
    }

    best->assignComplaint();
    assignments[complaintId] = best->getWorkerId();

    return best->getWorkerId();
}

bool MaintenanceManager::completeComplaint(const std::string& complaintId)
{
    auto assignmentIt = assignments.find(complaintId);

    if (assignmentIt == assignments.end())
    {
        return false;                       // never assigned
    }

    MaintenanceWorker* worker = findWorker(assignmentIt->second);
    if (worker != nullptr)
    {
        worker->completeComplaint();
    }

    assignments.erase(assignmentIt);
    return true;
}

std::optional<std::string> MaintenanceManager::getAssignedWorker(const std::string& complaintId) const
{
    auto it = assignments.find(complaintId);

    if (it == assignments.end())
    {
        return std::nullopt;
    }
    return it->second;
}