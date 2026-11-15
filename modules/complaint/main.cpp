// HostelPulse - Complaint Management console menu.
// All input and output happens here; the library classes only return results.

#include <iostream>
#include <optional>
#include <string>

#include "ComplaintManager.h"
#include "PriorityRules.h"

using namespace std;

namespace
{
    void displayComplaint(const Complaint& complaint)
    {
        cout << "\n------------------------------------\n"
             << "Complaint ID : " << complaint.getId() << '\n'
             << "Student ID   : " << complaint.getStudentId() << '\n'
             << "Category     : " << complaint.getCategory() << '\n'
             << "Description  : " << complaint.getDescription() << '\n'
             << "Location     : " << complaint.getLocation() << '\n'
             << "Priority     : " << priorityToString(complaint.getPriority()) << '\n'
             << "Status       : " << statusToString(complaint.getStatus()) << '\n';
        if (!complaint.getAssignedWorkerId().empty())
        {
            cout << "Worker       : " << complaint.getAssignedWorkerId() << '\n';
        }
        cout << "------------------------------------\n";
    }

    string readLine(const string& prompt)
    {
        string text;
        cout << prompt;
        getline(cin, text);
        return text;
    }

    void addComplaint(ComplaintManager& manager)
    {
        string studentId = readLine("Enter Student/User ID: ");

        cout << "Categories:";
        for (int i = 0; i < PriorityRules::CATEGORY_COUNT; i++)
        {
            cout << ' ' << PriorityRules::CATEGORIES[i] << (i + 1 < PriorityRules::CATEGORY_COUNT ? "," : "\n");
        }
        string category = readLine("Enter category (exactly as listed): ");
        string description = readLine("Enter complaint description: ");
        string location = readLine("Enter location (e.g. Sai Hostel 204): ");

        optional<string> id = manager.addComplaint(studentId, category, description, location);
        if (id)
        {
            cout << "\nComplaint added successfully!\n"
                 << "Generated Complaint ID: " << *id << '\n'
                 << "Assigned Priority: "
                 << priorityToString(manager.findComplaint(*id)->getPriority()) << '\n';
        }
        else
        {
            cout << "Invalid input: check the fields and category.\n";
        }
    }
}

int main()
{
    ComplaintManager manager;
    int choice = -1;

    do
    {
        cout << "\n======= HOSTELPULSE =======\n"
             << "Complaint Management\n"
             << "1. Add complaint\n"
             << "2. Search complaint by ID\n"
             << "3. Display all complaints\n"
             << "4. Show next pending complaint\n"
             << "5. Mark complaint assigned\n"
             << "6. Mark complaint resolved\n"
             << "0. Exit\n"
             << "Enter your choice: ";

        if (!(cin >> choice))
        {
            if (cin.eof())
            {
                break;
            }
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a valid number.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice)
        {
            case 1:
                addComplaint(manager);
                break;

            case 2:
            {
                const Complaint* complaint = manager.findComplaint(readLine("Enter Complaint ID to search: "));
                if (complaint != nullptr)
                {
                    displayComplaint(*complaint);
                }
                else
                {
                    cout << "Complaint not found.\n";
                }
                break;
            }

            case 3:
                if (manager.getComplaintCount() == 0)
                {
                    cout << "No complaints have been added yet.\n";
                }
                else
                {
                    cout << "\n===== ALL COMPLAINTS =====\n";
                    manager.forEachComplaint(displayComplaint);
                }
                break;

            case 4:
            {
                optional<string> next = manager.nextPending();
                if (next)
                {
                    cout << "Next complaint to handle:";
                    displayComplaint(*manager.findComplaint(*next));
                }
                else
                {
                    cout << "No pending complaints.\n";
                }
                break;
            }

            case 5:
            {
                string id = readLine("Enter Complaint ID: ");
                string worker = readLine("Enter Worker ID: ");
                cout << (manager.markAssigned(id, worker)
                             ? "Complaint marked as Assigned.\n"
                             : "Not allowed: unknown complaint, empty worker ID, or not Pending.\n");
                break;
            }

            case 6:
            {
                string id = readLine("Enter Complaint ID: ");
                cout << (manager.markResolved(id)
                             ? "Complaint marked as Resolved.\n"
                             : "Not allowed: unknown complaint or not Assigned.\n");
                break;
            }

            case 0:
                cout << "Exiting HostelPulse.\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
