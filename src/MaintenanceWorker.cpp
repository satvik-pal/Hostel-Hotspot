#include "../include/MaintenanceWorker.h"

MaintenanceWorker::MaintenanceWorker(string id, string n, string s,
                                     string loc, bool avail, int work)
{
    workerId = id;
    name = n;
    skill = s;
    location = loc;
    available = avail;
    workload = work;
}

string MaintenanceWorker::getWorkerId()
{
    return workerId;
}
string MaintenanceWorker::getName()
{
    return name;
}
string MaintenanceWorker::getSkill()
{
    return skill;
}
string MaintenanceWorker::getLocation()
{
    return location;
}
bool MaintenanceWorker::getAvailability()
{
    return available;
}
int MaintenanceWorker::getWorkload()
{
    return workload;
}

void MaintenanceWorker::assignComplaint()
{
    if (workload < 10)
    {
        workload++;
    }
    if(workload >=10)
    {
        available = false;
    }
}

void MaintenanceWorker::completeComplaint()
{
    if(workload > 0)
    {
        workload--;
    }
    if (workload < 10)
    {
        available = true;
    }
    else if(workload >=10)
    {
        available = false;
    }
}
void MaintenanceWorker::updateLocation(string new_loc)
{
    location = new_loc;
}

