#include "MaintenanceWorker.h"

MaintenanceWorker::MaintenanceWorker(const std::string& workerId,
                                     const std::string& name,
                                     const std::string& skill,
                                     const std::string& location,
                                     bool onDuty,
                                     int workload)
    : workerId(workerId),
      name(name),
      skill(skill),
      location(location),
      onDuty(onDuty),
      workload(workload)
{
}

std::string MaintenanceWorker::getWorkerId() const { return workerId; }
std::string MaintenanceWorker::getName() const { return name; }
std::string MaintenanceWorker::getSkill() const { return skill; }
std::string MaintenanceWorker::getLocation() const { return location; }
int MaintenanceWorker::getWorkload() const { return workload; }

bool MaintenanceWorker::isOnDuty() const
{
    return onDuty;
}

bool MaintenanceWorker::hasCapacity() const
{
    return workload < MAX_WORKLOAD;
}

bool MaintenanceWorker::isAvailable() const
{
    return onDuty && hasCapacity();
}

void MaintenanceWorker::setOnDuty(bool status)
{
    onDuty = status;
}

bool MaintenanceWorker::assignComplaint()
{
    if (!isAvailable())
    {
        return false;
    }
    workload++;
    return true;
}

bool MaintenanceWorker::completeComplaint()
{
    if (workload == 0)
    {
        return false;
    }
    workload--;
    return true;
}

void MaintenanceWorker::updateLocation(const std::string& newLocation)
{
    location = newLocation;
}