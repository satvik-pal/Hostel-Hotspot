// Hostel Hotspot - integrated application.
// Connects the four modules into one program:
//   Student (Avinash)  -> login, student records
//   Complaint (Saksham) -> complaint registration, priority queue, status
//   Infrastructure (Utsav) -> campus graph and room registry
//   Maintenance (Satvik) -> worker allocation with Dijkstra routing

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>

#include "AuthManager.h"
#include "Campus.h"
#include "ComplaintManager.h"
#include "MaintenanceManager.h"
#include "PriorityRules.h"
#include "StudentManager.h"

using namespace std;

struct HostelSystem
{
    HostelGraph graph;
    RoomRegistry rooms;
    StudentManager students;
    AuthManager auth;
    ComplaintManager complaints;
    MaintenanceManager maintenance;
};

struct HotspotEntry
{
    string name;
    int count = 0;
};

namespace
{
    string readLine(const string& prompt)
    {
        cout << prompt;
        string text;
        if (!getline(cin, text))
        {
            cout << "\nInput closed. Exiting Hostel Hotspot.\n";
            exit(0);
        }
        return text;
    }

    int readChoice(const string& prompt)
    {
        string text = readLine(prompt);
        if (text.empty() || text.size() > 3)
        {
            return -1;
        }
        for (char c : text)
        {
            if (!isdigit(static_cast<unsigned char>(c)))
            {
                return -1;
            }
        }
        return stoi(text);
    }

    string chooseCategory()
    {
        cout << "Categories:\n";
        for (int i = 0; i < PriorityRules::CATEGORY_COUNT; i++)
        {
            cout << "  " << i + 1 << ". " << PriorityRules::CATEGORIES[i] << '\n';
        }
        int choice = readChoice("Choose category: ");
        if (choice < 1 || choice > PriorityRules::CATEGORY_COUNT)
        {
            return "";
        }
        return PriorityRules::CATEGORIES[choice - 1];
    }

    string chooseHostel()
    {
        cout << "Hostels:\n";
        for (int i = 0; i < Campus::HOSTEL_COUNT; i++)
        {
            cout << "  " << i + 1 << ". " << Campus::HOSTEL_NAMES[i] << '\n';
        }
        int choice = readChoice("Choose hostel: ");
        if (choice < 1 || choice > Campus::HOSTEL_COUNT)
        {
            return "";
        }
        return Campus::HOSTEL_NAMES[choice - 1];
    }

    string readCampusLocation(const HostelSystem& sys, const string& prompt)
    {
        cout << "(Examples: Sai Hostel 204, " << Campus::MESS << ", "
             << Campus::MAIN_GATE << ", " << Campus::MAINTENANCE_OFFICE << ")\n";
        string location = readLine(prompt);
        if (sys.graph.findVertex(location) == -1)
        {
            cout << "Unknown campus location.\n";
            return "";
        }
        return location;
    }

    void displayComplaint(const Complaint& c)
    {
        cout << "\n------------------------------------\n"
             << "Complaint ID : " << c.getId() << '\n'
             << "Student ID   : " << c.getStudentId() << '\n'
             << "Category     : " << c.getCategory() << '\n'
             << "Description  : " << c.getDescription() << '\n'
             << "Location     : " << c.getLocation() << '\n'
             << "Priority     : " << priorityToString(c.getPriority()) << '\n'
             << "Status       : " << statusToString(c.getStatus()) << '\n';
        if (!c.getAssignedWorkerId().empty())
        {
            cout << "Worker       : " << c.getAssignedWorkerId() << '\n';
        }
        cout << "------------------------------------\n";
    }

    void displayWorker(const MaintenanceWorker& w)
    {
        cout << "  " << w.getWorkerId() << "  " << w.getName()
             << "  [" << w.getSkill() << ", " << w.getLocation() << "]"
             << "  workload " << w.getWorkload() << "/" << MaintenanceWorker::MAX_WORKLOAD
             << (w.isOnDuty() ? "" : "  (off duty)") << '\n';
    }

    void displayRoute(const HostelGraph& graph, const RouteResult& route)
    {
        if (!route.reachable)
        {
            cout << "Route        : not available\n";
            return;
        }
        cout << "Distance     : " << route.distance << " m\n"
             << "Route        : ";
        for (int i = 0; i < route.path.size(); i++)
        {
            if (i > 0)
            {
                cout << " -> ";
            }
            cout << graph.getName(route.path[i]);
        }
        cout << '\n';
    }

    // ---------------- Setup ----------------

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

        const string office = Campus::MAINTENANCE_OFFICE;
        sys.maintenance.addWorker("Amit Kumar", "Electrical", office);
        sys.maintenance.addWorker("Ravi Singh", "Electrical", Campus::entranceName("Bhagat Singh Hostel"));
        sys.maintenance.addWorker("Satish Yadav", "Plumbing", Campus::MESS);
        sys.maintenance.addWorker("Mohan Lal", "Carpentry", office);
        sys.maintenance.addWorker("Sunita Devi", "Cleaning", Campus::entranceName("Sai Hostel"));
        sys.maintenance.addWorker("Deepak Rawat", "AC Repair", office);
        sys.maintenance.addWorker("Karan Mehta", "Internet", Campus::MAIN_GATE);

        return true;
    }

    // ---------------- Student features ----------------

    void raiseComplaint(HostelSystem& sys, const string& studentId)
    {
        const Student* student = sys.students.findStudent(studentId);
        if (student == nullptr)
        {
            cout << "Student record not found.\n";
            return;
        }

        string category = chooseCategory();
        if (category.empty())
        {
            cout << "Invalid category.\n";
            return;
        }

        string description = readLine("Describe the problem: ");

        string myRoom = student->getHostelName() + " " + student->getRoomNumber();
        cout << "Location:\n  1. My room (" << myRoom << ")\n  2. Another campus location\n";
        int where = readChoice("Choose: ");

        string location;
        if (where == 1)
        {
            location = myRoom;
        }
        else if (where == 2)
        {
            location = readCampusLocation(sys, "Enter location: ");
        }

        if (location.empty() || sys.graph.findVertex(location) == -1)
        {
            cout << "Invalid location. Complaint not registered.\n";
            return;
        }

        optional<string> id = sys.complaints.addComplaint(studentId, category, description, location);
        if (!id)
        {
            cout << "Invalid input. Complaint not registered.\n";
            return;
        }

        cout << "\nComplaint registered.\n"
             << "Complaint ID : " << *id << '\n'
             << "Priority     : " << priorityToString(sys.complaints.findComplaint(*id)->getPriority()) << '\n';
    }

    void showMyComplaints(const HostelSystem& sys, const string& studentId)
    {
        int found = 0;
        sys.complaints.forEachComplaint([&](const Complaint& c)
        {
            if (c.getStudentId() == studentId)
            {
                displayComplaint(c);
                found++;
            }
        });
        if (found == 0)
        {
            cout << "You have not raised any complaints.\n";
        }
    }

    void trackComplaint(const HostelSystem& sys, const string& studentId)
    {
        const Complaint* c = sys.complaints.findComplaint(readLine("Enter Complaint ID: "));
        if (c == nullptr || c->getStudentId() != studentId)
        {
            cout << "No complaint with that ID in your account.\n";
            return;
        }
        displayComplaint(*c);
    }

    void showProfile(const HostelSystem& sys, const string& studentId)
    {
        const Student* s = sys.students.findStudent(studentId);
        if (s == nullptr)
        {
            cout << "Student record not found.\n";
            return;
        }
        cout << "\nStudent ID : " << s->getStudentId() << '\n'
             << "Name       : " << s->getName() << '\n'
             << "Phone      : " << s->getPhone() << '\n'
             << "Hostel     : " << s->getHostelName() << '\n'
             << "Room       : " << s->getRoomNumber() << '\n';
    }

    void studentMenu(HostelSystem& sys, const string& studentId)
    {
        while (true)
        {
            cout << "\n===== STUDENT MENU (" << studentId << ") =====\n"
                 << "1. Raise a complaint\n"
                 << "2. View my complaints\n"
                 << "3. Track a complaint\n"
                 << "4. My profile\n"
                 << "0. Logout\n";
            switch (readChoice("Enter choice: "))
            {
                case 1: raiseComplaint(sys, studentId); break;
                case 2: showMyComplaints(sys, studentId); break;
                case 3: trackComplaint(sys, studentId); break;
                case 4: showProfile(sys, studentId); break;
                case 0: return;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    // ---------------- Admin features ----------------

    void assignComplaint(HostelSystem& sys, const string& complaintId)
    {
        const Complaint* c = sys.complaints.findComplaint(complaintId);
        if (c == nullptr)
        {
            cout << "Complaint not found.\n";
            return;
        }
        if (c->getStatus() != ComplaintStatus::Pending)
        {
            cout << "Complaint is already " << statusToString(c->getStatus()) << ".\n";
            return;
        }

        const string category = c->getCategory();
        optional<AllocationResult> result =
            sys.maintenance.allocateWithRoute(complaintId, category, c->getLocation(), sys.graph);

        if (!result)
        {
            cout << "No available " << category << " worker right now. Complaint stays Pending.\n";
            return;
        }

        sys.complaints.markAssigned(complaintId, result->workerId);

        const MaintenanceWorker* worker = sys.maintenance.findWorker(result->workerId);
        cout << "\nComplaint " << complaintId << " assigned.\n"
             << "Worker       : " << result->workerId;
        if (worker != nullptr)
        {
            cout << " (" << worker->getName() << ", at " << worker->getLocation() << ")";
        }
        cout << '\n';
        displayRoute(sys.graph, result->route);
    }

    void assignNextPending(HostelSystem& sys)
    {
        optional<string> next = sys.complaints.nextPending();
        if (!next)
        {
            cout << "No pending complaints.\n";
            return;
        }
        cout << "Highest-priority pending complaint:";
        displayComplaint(*sys.complaints.findComplaint(*next));
        assignComplaint(sys, *next);
    }

    void resolveComplaint(HostelSystem& sys)
    {
        string id = readLine("Enter Complaint ID: ");
        const Complaint* c = sys.complaints.findComplaint(id);
        if (c == nullptr)
        {
            cout << "Complaint not found.\n";
            return;
        }
        if (c->getStatus() != ComplaintStatus::Assigned)
        {
            cout << "Only Assigned complaints can be resolved (current: "
                 << statusToString(c->getStatus()) << ").\n";
            return;
        }
        sys.maintenance.completeComplaint(id);
        sys.complaints.markResolved(id);
        cout << "Complaint " << id << " marked Resolved. Worker workload updated.\n";
    }

    void addWorker(HostelSystem& sys)
    {
        string name = readLine("Worker name: ");
        if (name.empty())
        {
            cout << "Name is required.\n";
            return;
        }
        string skill = chooseCategory();
        if (skill.empty())
        {
            cout << "Invalid skill.\n";
            return;
        }
        string location = readCampusLocation(sys, "Worker's current location: ");
        if (location.empty())
        {
            return;
        }
        cout << "Worker added with ID " << sys.maintenance.addWorker(name, skill, location) << ".\n";
    }

    void toggleWorkerDuty(HostelSystem& sys)
    {
        MaintenanceWorker* w = sys.maintenance.findWorker(readLine("Enter Worker ID: "));
        if (w == nullptr)
        {
            cout << "Worker not found.\n";
            return;
        }
        w->setOnDuty(!w->isOnDuty());
        cout << w->getWorkerId() << " is now " << (w->isOnDuty() ? "on duty" : "off duty") << ".\n";
    }

    void addToCount(HashMap<int>& counts, const string& key)
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
        counts.forEach([&](const string& key, int& value)
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

    void hotspotReport(const HostelSystem& sys)
    {
        if (sys.complaints.getComplaintCount() == 0)
        {
            cout << "No complaints yet.\n";
            return;
        }

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

        DynamicArray<HotspotEntry> locations = sortedByCount(byLocation);
        DynamicArray<HotspotEntry> hostels = sortedByCount(byHostel);
        DynamicArray<HotspotEntry> categories = sortedByCount(byCategory);

        cout << "\n===== HOTSPOT REPORT =====\n"
             << "Total complaints: " << sys.complaints.getComplaintCount() << "\n\n"
             << "Top locations:\n";
        for (int i = 0; i < locations.size() && i < 5; i++)
        {
            cout << "  " << i + 1 << ". " << locations[i].name << " - " << locations[i].count << '\n';
        }
        cout << "\nBy hostel:\n";
        for (int i = 0; i < hostels.size(); i++)
        {
            cout << "  " << hostels[i].name << " - " << hostels[i].count << '\n';
        }
        cout << "\nBy category:\n";
        for (int i = 0; i < categories.size(); i++)
        {
            cout << "  " << categories[i].name << " - " << categories[i].count << '\n';
        }
    }

    void adminMenu(HostelSystem& sys)
    {
        while (true)
        {
            cout << "\n===== ADMIN MENU =====\n"
                 << "Pending complaints: " << sys.complaints.getPendingCount() << '\n'
                 << "1. View all complaints\n"
                 << "2. Assign highest-priority complaint (automatic)\n"
                 << "3. Assign a specific complaint\n"
                 << "4. Mark complaint resolved\n"
                 << "5. View workers\n"
                 << "6. Add worker\n"
                 << "7. Change worker duty status\n"
                 << "8. View students\n"
                 << "9. Hotspot report\n"
                 << "0. Logout\n";
            switch (readChoice("Enter choice: "))
            {
                case 1:
                    if (sys.complaints.getComplaintCount() == 0)
                    {
                        cout << "No complaints yet.\n";
                    }
                    else
                    {
                        sys.complaints.forEachComplaint(displayComplaint);
                    }
                    break;
                case 2: assignNextPending(sys); break;
                case 3: assignComplaint(sys, readLine("Enter Complaint ID: ")); break;
                case 4: resolveComplaint(sys); break;
                case 5:
                    cout << "\nWorkers (" << sys.maintenance.getWorkerCount() << "):\n";
                    sys.maintenance.forEachWorker(displayWorker);
                    break;
                case 6: addWorker(sys); break;
                case 7: toggleWorkerDuty(sys); break;
                case 8:
                    cout << "\nStudents (" << sys.students.getTotalStudents() << "):\n";
                    sys.students.forEachStudent([](const Student& s)
                    {
                        cout << "  " << s.getStudentId() << "  " << s.getName() << "  ("
                             << s.getHostelName() << " " << s.getRoomNumber() << ")\n";
                    });
                    break;
                case 9: hotspotReport(sys); break;
                case 0: return;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    // ---------------- Entry screens ----------------

    void registerStudent(HostelSystem& sys)
    {
        string id = readLine("Student ID: ");
        string name = readLine("Name: ");
        string phone = readLine("Phone: ");
        if (id.empty() || name.empty())
        {
            cout << "Student ID and name are required.\n";
            return;
        }
        if (sys.auth.getRole(id))
        {
            cout << "That ID is already taken.\n";
            return;
        }

        string hostel = chooseHostel();
        if (hostel.empty())
        {
            cout << "Invalid hostel.\n";
            return;
        }
        string room = readLine("Room number (e.g. 204): ");
        if (sys.rooms.findRoom(hostel + " " + room) == nullptr)
        {
            cout << "Room " << room << " does not exist in " << hostel << ".\n";
            return;
        }

        string password = readLine("Create password (min 6 characters): ");
        if (static_cast<int>(password.length()) < AuthManager::MIN_PASSWORD_LENGTH)
        {
            cout << "Password too short.\n";
            return;
        }

        if (!sys.students.addStudent(Student(id, name, phone, hostel, room)))
        {
            cout << "Could not register student.\n";
            return;
        }
        if (!sys.auth.createAccount(id, password, Role::Student))
        {
            sys.students.removeStudent(id);
            cout << "Account creation failed; registration rolled back.\n";
            return;
        }
        cout << "Registered successfully. You can now log in.\n";
    }

    void login(HostelSystem& sys)
    {
        string id = readLine("User ID: ");
        string password = readLine("Password: ");
        if (!sys.auth.login(id, password))
        {
            cout << "Invalid ID or password.\n";
            return;
        }
        if (*sys.auth.getRole(id) == Role::Admin)
        {
            adminMenu(sys);
        }
        else
        {
            studentMenu(sys, id);
        }
    }
}

int main()
{
    HostelSystem sys;
    if (!setupSystem(sys))
    {
        cout << "Failed to build campus. Exiting.\n";
        return 1;
    }

    cout << "========================================\n"
         << "            HOSTEL HOTSPOT\n"
         << "  Complaint & Maintenance Management\n"
         << "========================================\n"
         << "Campus: " << Campus::HOSTEL_COUNT << " hostels, "
         << sys.rooms.getRoomCount() << " rooms, "
         << sys.maintenance.getWorkerCount() << " workers on record\n"
         << "Demo logins -> admin / admin123   S101 / student123\n";

    while (true)
    {
        cout << "\n1. Login\n2. Register as student\n0. Exit\n";
        switch (readChoice("Enter choice: "))
        {
            case 1: login(sys); break;
            case 2: registerStudent(sys); break;
            case 0:
                cout << "Goodbye.\n";
                return 0;
            default: cout << "Invalid choice.\n";
        }
    }
}
