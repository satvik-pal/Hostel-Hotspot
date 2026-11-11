// HostelPulse - Maintenance module demo.
// The full automated test suite is in tests/test_maintenance.cpp.

#include <iostream>
#include <optional>
#include <string>
#include "MaintenanceManager.h"

using namespace std;

void showAllocation(const string& complaintId, const string& skill, const optional<string>& workerId)
{
    cout << "  " << complaintId << " (" << skill << ") -> ";
    if (workerId)
    {
        cout << "assigned to " << *workerId << endl;
    }
    else
    {
        cout << "no suitable worker available" << endl;
    }
}

void showWorker(MaintenanceManager& manager, const string& workerId)
{
    MaintenanceWorker* worker = manager.findWorker(workerId);
    if (worker == nullptr)
    {
        cout << "  " << workerId << ": not found" << endl;
        return;
    }

    cout << "  " << worker->getWorkerId() << "  " << worker->getName()
         << "  [" << worker->getSkill() << ", " << worker->getLocation() << "]"
         << "  workload " << worker->getWorkload() << "/" << MaintenanceWorker::MAX_WORKLOAD
         << (worker->isOnDuty() ? "" : "  (off duty)") << endl;
}

int main()
{
    MaintenanceManager manager;

    cout << "=== HostelPulse: Maintenance Module Demo ===" << endl;

    cout << endl << "Registering workers:" << endl;
    string amit   = manager.addWorker("Amit Kumar", "Electrical", "Reception");
    string ravi   = manager.addWorker("Ravi Singh", "Electrical", "Room 102");
    string satish = manager.addWorker("Satish",     "Plumbing",   "Mess");
    showWorker(manager, amit);
    showWorker(manager, ravi);
    showWorker(manager, satish);

    cout << endl << "Allocating complaints (lowest workload first, then lower ID):" << endl;
    showAllocation("C001", "Electrical", manager.allocateComplaint("C001", "Electrical"));
    showAllocation("C002", "Electrical", manager.allocateComplaint("C002", "Electrical"));
    showAllocation("C003", "Plumbing",   manager.allocateComplaint("C003", "Plumbing"));
    showAllocation("C004", "Carpentry",  manager.allocateComplaint("C004", "Carpentry"));

    cout << endl << "Satish goes off duty; a new plumbing complaint arrives:" << endl;
    manager.findWorker(satish)->setOnDuty(false);
    showAllocation("C005", "Plumbing", manager.allocateComplaint("C005", "Plumbing"));

    cout << endl << "Completing C001:" << endl;
    cout << "  " << (manager.completeComplaint("C001") ? "C001 completed" : "C001 could not be completed") << endl;

    cout << endl << "Current workers:" << endl;
    showWorker(manager, amit);
    showWorker(manager, ravi);
    showWorker(manager, satish);

    return 0;
}