#ifndef MAINTENANCE_WORKER_H
#define MAINTENANCE_WORKER_H

#include <string>
using namespace std;

class MaintenanceWorker
{
    public:
        static constexpr int MAX_WORKLOAD = 10;

    private:
        std::string workerId;
        std::string name;
        std::string skill;
        std::string location;
        bool onDuty;        // set by admin (leave, shift, sick)
        int workload;       // complaints currently assigned

    public:
        MaintenanceWorker(const std::string& workerId,
                          const std::string& name,
                          const std::string& skill,
                          const std::string& location,
                          bool onDuty,
                          int workload);

        std::string getWorkerId() const;
        std::string getName() const;
        std::string getSkill() const;
        std::string getLocation() const;
        int         getWorkload() const;

        bool isOnDuty() const;
        bool hasCapacity() const;
        bool isAvailable() const;

        void setOnDuty(bool status);
        bool assignComplaint();
        bool completeComplaint();
        void updateLocation(const std::string& newLocation);
};

#endif