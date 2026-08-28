#ifndef MAINTENANCE_MANAGER_H
#define MAINTENANCE_MANAGER_H
#include "MaintenanceWorker.h"
#include <string>
#include <unordered_map>
using namespace std;

class MaintenanceManager
{
    private:
        unordered_map<string, MaintenanceWorker> workers;
        int workerCounter;

    public:
        MaintenanceManager();
        void addWorker(string name, string skill, string location);
         
        MaintenanceWorker* findWorker(string workerId);

        void removeWorker(string workerId);
};
#endif