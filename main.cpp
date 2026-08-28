#include <iostream>
#include "include/MaintenanceWorker.h"
#include "include/MaintenanceManager.h"

using namespace std;

int main()
{
    /* Worker test case
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
*/

    MaintenanceManager manager;
    manager.addWorker("Amit Kumar","Electrical", "Block A");
    manager.addWorker("Satish","Plumber", "Block C");
    /*
    MaintenanceWorker* worker = manager.findWorker("W020");

    if(worker == nullptr)
    {
        cout << "Not found" << endl;
    }
    else
    {
       cout << worker -> getName();
    }
*/
manager.removeWorker("W001");
manager.removeWorker("W001");
    return 0;
}