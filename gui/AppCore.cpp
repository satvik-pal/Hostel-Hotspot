#include "AppCore.h"

#include <optional>

namespace
{
    void addToCount(HashMap<int>& counts, const std::string& key)
    {
        int* value = counts.find(key);
        if (value != nullptr)
        {
            (*value)++;
        }
        else
        {
            counts.insert(key, 1);
        }
    }

    DynamicArray<HotspotEntry> sortedByCount(HashMap<int>& counts)
    {
        DynamicArray<HotspotEntry> entries;
        counts.forEach([&](const std::string& key, int& value)
        {
            HotspotEntry e;
            e.name = key;
            e.count = value;
            entries.push(e);
        });

        for (int i = 0; i < entries.size(); i++)
        {
            int best = i;
            for (int j = i + 1; j < entries.size(); j++)
            {
                if (entries[j].count > entries[best].count)
                {
                    best = j;
                }
            }
            HotspotEntry temp = entries[i];
            entries[i] = entries[best];
            entries[best] = temp;
        }
        return entries;
    }

    ActionResult fail(const std::string& message)
    {
        ActionResult r;
        r.success = false;
        r.message = message;
        return r;
    }

    ActionResult ok(const std::string& message)
    {
        ActionResult r;
        r.success = true;
        r.message = message;
        return r;
    }
}

bool setupSystem(HostelSystem& sys)
{
    if (!Campus::build(sys.graph, sys.rooms))
    {
        return false;
    }

    // Demo-only accounts. Change these passwords before a live demonstration.
    sys.auth.createAccount("admin", "admin123", Role::Admin);

    sys.students.addStudent(Student("S101", "Demo Student", "9000000000", "Sai Hostel", "204"));
    sys.auth.createAccount("S101", "student123", Role::Student);

    const std::string office = Campus::MAINTENANCE_OFFICE;
    sys.maintenance.addWorker("Amit Kumar", "Electrical", office);
    sys.maintenance.addWorker("Ravi Singh", "Electrical", Campus::entranceName("Bhagat Singh Hostel"));
    sys.maintenance.addWorker("Satish Yadav", "Plumbing", Campus::MESS);
    sys.maintenance.addWorker("Mohan Lal", "Carpentry", office);
    sys.maintenance.addWorker("Sunita Devi", "Cleaning", Campus::entranceName("Sai Hostel"));
    sys.maintenance.addWorker("Deepak Rawat", "AC Repair", office);
    sys.maintenance.addWorker("Karan Mehta", "Internet", Campus::MAIN_GATE);

    return true;
}

ActionResult registerStudent(HostelSystem& sys,
                             const std::string& id, const std::string& name,
                             const std::string& phone, const std::string& hostel,
                             const std::string& room, const std::string& password)
{
    if (id.empty() || name.empty())
    {
        return fail("Student ID and name are required.");
    }
    if (sys.auth.getRole(id))
    {
        return fail("That ID is already taken.");
    }
    if (sys.rooms.findRoom(hostel + " " + room) == nullptr)
    {
        return fail("Room " + room + " does not exist in " + hostel + ".");
    }
    if (static_cast<int>(password.length()) < AuthManager::MIN_PASSWORD_LENGTH)
    {
        return fail("Password must have at least " +
                    std::to_string(AuthManager::MIN_PASSWORD_LENGTH) + " characters.");
    }
    if (!sys.students.addStudent(Student(id, name, phone, hostel, room)))
    {
        return fail("Could not register student.");
    }
    if (!sys.auth.createAccount(id, password, Role::Student))
    {
        sys.students.removeStudent(id);
        return fail("Account creation failed; registration rolled back.");
    }
    return ok("Registered successfully. You can now log in.");
}

ActionResult raiseComplaint(HostelSystem& sys, const std::string& studentId,
                            const std::string& category, const std::string& description,
                            const std::string& location)
{
    if (sys.graph.findVertex(location) == -1)
    {
        return fail("Unknown campus location: " + location);
    }
    std::optional<std::string> id = sys.complaints.addComplaint(studentId, category, description, location);
    if (!id)
    {
        return fail("Invalid input. Check the category and description.");
    }
    const Complaint* c = sys.complaints.findComplaint(*id);
    return ok("Complaint " + *id + " registered with " +
              priorityToString(c->getPriority()) + " priority.");
}

std::string routeToString(const HostelGraph& graph, const RouteResult& route)
{
    if (!route.reachable)
    {
        return "route not available";
    }
    std::string text = std::to_string(route.distance) + " m: ";
    for (int i = 0; i < route.path.size(); i++)
    {
        if (i > 0)
        {
            text += " -> ";
        }
        text += graph.getName(route.path[i]);
    }
    return text;
}

ActionResult assignComplaint(HostelSystem& sys, const std::string& complaintId)
{
    const Complaint* c = sys.complaints.findComplaint(complaintId);
    if (c == nullptr)
    {
        return fail("Complaint " + complaintId + " not found.");
    }
    if (c->getStatus() != ComplaintStatus::Pending)
    {
        return fail("Complaint " + complaintId + " is already " + statusToString(c->getStatus()) + ".");
    }

    const std::string category = c->getCategory();
    std::optional<AllocationResult> result =
        sys.maintenance.allocateWithRoute(complaintId, category, c->getLocation(), sys.graph);

    if (!result)
    {
        return fail("No available " + category + " worker for " + complaintId + ". It stays Pending.");
    }

    sys.complaints.markAssigned(complaintId, result->workerId);

    std::string message = complaintId + " assigned to " + result->workerId;
    const MaintenanceWorker* worker = sys.maintenance.findWorker(result->workerId);
    if (worker != nullptr)
    {
        message += " (" + worker->getName() + ", at " + worker->getLocation() + ")";
    }
    message += ". Route " + routeToString(sys.graph, result->route);
    return ok(message);
}

ActionResult assignNextPending(HostelSystem& sys)
{
    std::optional<std::string> next = sys.complaints.nextPending();
    if (!next)
    {
        return fail("No pending complaints.");
    }
    return assignComplaint(sys, *next);
}

ActionResult resolveComplaint(HostelSystem& sys, const std::string& complaintId)
{
    const Complaint* c = sys.complaints.findComplaint(complaintId);
    if (c == nullptr)
    {
        return fail("Complaint " + complaintId + " not found.");
    }
    if (c->getStatus() != ComplaintStatus::Assigned)
    {
        return fail("Only Assigned complaints can be resolved (" + complaintId + " is " +
                    statusToString(c->getStatus()) + ").");
    }
    sys.maintenance.completeComplaint(complaintId);
    sys.complaints.markResolved(complaintId);
    return ok(complaintId + " marked Resolved. Worker workload updated.");
}

ActionResult addWorker(HostelSystem& sys, const std::string& name,
                       const std::string& skill, const std::string& location)
{
    if (name.empty())
    {
        return fail("Worker name is required.");
    }
    if (!PriorityRules::isValidCategory(skill))
    {
        return fail("Invalid skill.");
    }
    if (sys.graph.findVertex(location) == -1)
    {
        return fail("Unknown campus location: " + location);
    }
    std::string id = sys.maintenance.addWorker(name, skill, location);
    return ok("Worker " + name + " added with ID " + id + ".");
}

ActionResult toggleWorkerDuty(HostelSystem& sys, const std::string& workerId)
{
    MaintenanceWorker* w = sys.maintenance.findWorker(workerId);
    if (w == nullptr)
    {
        return fail("Worker " + workerId + " not found.");
    }
    w->setOnDuty(!w->isOnDuty());
    return ok(workerId + " is now " + (w->isOnDuty() ? "on duty." : "off duty."));
}

HotspotReport buildHotspotReport(const HostelSystem& sys)
{
    HashMap<int> byLocation;
    HashMap<int> byHostel;
    HashMap<int> byCategory;

    sys.complaints.forEachComplaint([&](const Complaint& c)
    {
        addToCount(byLocation, c.getLocation());
        addToCount(byCategory, c.getCategory());
        const Room* room = sys.rooms.findRoom(c.getLocation());
        addToCount(byHostel, room != nullptr ? room->hostel : "Common areas");
    });

    HotspotReport report;
    report.total = sys.complaints.getComplaintCount();
    report.locations = sortedByCount(byLocation);
    report.hostels = sortedByCount(byHostel);
    report.categories = sortedByCount(byCategory);
    return report;
}

