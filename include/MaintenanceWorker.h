#ifndef MAINTENANCE_WORKER_H
#define MAINTENANCE_WORKER_H
#include <string>
using namespace std;

class MaintenanceWorker
{
    private:
        string workerId;
        string name;
        string skill;
        string location;
        bool available;
        int workload;

    public:
        MaintenanceWorker(string wid , string n, string s, string l, bool a, int wl);

        string getWorkerId();
        string getName();
        string getSkill();
        string getLocation();
        bool   getAvailability();
        int    getWorkload();

        void assignComplaint();
        void completeComplaint();
        void updateLocation(string new_loc);
};

#endif