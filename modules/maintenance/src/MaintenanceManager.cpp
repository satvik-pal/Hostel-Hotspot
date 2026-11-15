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

const MaintenanceWorker* MaintenanceManager::findWorker(const std::string& workerId) const
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

bool MaintenanceManager::isBetterCandidate(const MaintenanceWorker& candidate, int candidateDistance,
                                           const MaintenanceWorker& currentBest, int bestDistance)
{
    if (candidate.getWorkload() != currentBest.getWorkload())
    {
        return candidate.getWorkload() < currentBest.getWorkload();     // 1. lower workload
    }
    if (candidateDistance != bestDistance)
    {
        return candidateDistance < bestDistance;                         // 2. nearer to the complaint
    }
    return candidate.getWorkerId() < currentBest.getWorkerId();         // 3. lower ID
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

// ---------- Persistence support ----------

bool MaintenanceManager::restoreWorker(const MaintenanceWorker& worker)
{
    if (worker.getWorkerId().empty())
    {
        return false;
    }
    if (!workers.insert(worker.getWorkerId(), worker))
    {
        return false;                       // ID already present
    }
    reserveWorkerId(worker.getWorkerId());
    return true;
}

bool MaintenanceManager::restoreAssignment(const std::string& complaintId, const std::string& workerId)
{
    if (complaintId.empty() || workers.find(workerId) == nullptr)
    {
        return false;
    }
    return assignments.insert(complaintId, workerId);
}

void MaintenanceManager::reserveWorkerId(const std::string& workerId)
{
    // IDs look like "W012": read the digits after the first character.
    int number = 0;
    for (int i = 1; i < static_cast<int>(workerId.size()); i++)
    {
        char c = workerId[i];
        if (c < '0' || c > '9')
        {
            return;                         // not a generated ID: nothing to reserve
        }
        number = number * 10 + (c - '0');
    }
    if (number + 1 > workerCounter)
    {
        workerCounter = number + 1;
    }
}
