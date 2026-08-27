#include <iostream>
#include "include/MaintenanceWorker.h"

using namespace std;

int main()
{
    MaintenanceWorker worker(
        "W001",
        "Amit Kumar",
        "Electrical",
        "Block A",
        true,
        0
    );
    cout << worker.getWorkerId() << endl;
    cout << worker.getName() << endl;
    cout << worker.getSkill() << endl;
    cout << worker.getLocation() << endl;
    cout << worker.getAvailability() << endl;
    cout << worker.getWorkload() << endl;

    worker.completeComplaint();
    cout <<"Workload after completing" << worker.getWorkload() << endl;

    if(worker.getAvailability())
    {
        cout << "Available" << endl;
    }
    else
    {
        cout << "Not available" << endl;
    }

    return 0;
}