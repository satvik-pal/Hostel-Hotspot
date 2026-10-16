#include <iostream>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <algorithm>

using namespace std;

enum class Priority {
    Low = 1,
    Medium = 2,
    High = 3
};

string priorityToString(Priority priority) {
    switch (priority) {
        case Priority::High: return "High";
        case Priority::Medium: return "Medium";
        default: return "Low";
    }
}

string toLowerCopy(string text) {
    transform(text.begin(), text.end(), text.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return text;
}

// Basic prototype priority rules. These can be improved after team discussion.
Priority assignPriority(const string& category, const string& description) {
    string text = toLowerCopy(category + " " + description);

    if (text.find("fire") != string::npos ||
        text.find("sparking") != string::npos ||
        text.find("exposed wire") != string::npos ||
        text.find("gas leak") != string::npos ||
        text.find("flood") != string::npos) {
        return Priority::High;
    }

    if (text.find("electrical") != string::npos ||
        text.find("plumbing") != string::npos ||
        text.find("water") != string::npos ||
        text.find("internet") != string::npos) {
        return Priority::Medium;
    }

    return Priority::Low;
}

class Complaint {
private:
    string complaintId;
    string studentId;
    string category;
    string description;
    string location;
    Priority priority;
    string status;

public:
    Complaint() : priority(Priority::Low), status("Pending") {}

    Complaint(string id, string student, string complaintCategory,
              string complaintDescription, string complaintLocation,
              Priority complaintPriority)
        : complaintId(id), studentId(student), category(complaintCategory),
          description(complaintDescription), location(complaintLocation),
          priority(complaintPriority), status("Pending") {}

    string getId() const { return complaintId; }

    void display() const {
        cout << "\n------------------------------------\n";
        cout << "Complaint ID : " << complaintId << '\n';
        cout << "Student ID   : " << studentId << '\n';
        cout << "Category     : " << category << '\n';
        cout << "Description  : " << description << '\n';
        cout << "Location     : " << location << '\n';
        cout << "Priority     : " << priorityToString(priority) << '\n';
        cout << "Status       : " << status << '\n';
        cout << "------------------------------------\n";
    }
};

class ComplaintManager {
private:
    unordered_map<string, Complaint> complaints;
    int nextId = 1;

    string generateComplaintId() {
        string number = to_string(nextId++);
        while (number.length() < 3) {
            number = "0" + number;
        }
        return "C" + number;
    }

public:
    void addComplaint() {
        string studentId, category, description, location;

        cout << "Enter Student/User ID: ";
        getline(cin, studentId);
        cout << "Enter complaint category: ";
        getline(cin, category);
        cout << "Enter complaint description: ";
        getline(cin, description);
        cout << "Enter location (block/room): ";
        getline(cin, location);

        string id = generateComplaintId();
        Priority priority = assignPriority(category, description);

        Complaint complaint(id, studentId, category, description, location, priority);
        complaints.emplace(id, complaint);

        cout << "\nComplaint added successfully!\n";
        cout << "Generated Complaint ID: " << id << '\n';
        cout << "Assigned Priority: " << priorityToString(priority) << '\n';
    }

    void findComplaint() const {
        string id;
        cout << "Enter Complaint ID to search: ";
        getline(cin, id);

        auto it = complaints.find(id);
        if (it != complaints.end()) {
            it->second.display();
        } else {
            cout << "Complaint not found.\n";
        }
    }

    void displayAllComplaints() const {
        if (complaints.empty()) {
            cout << "No complaints have been added yet.\n";
            return;
        }

        cout << "\n===== ALL COMPLAINTS =====\n";
        for (const auto& entry : complaints) {
            entry.second.display();
        }
    }
};

int main() {
    ComplaintManager manager;
    int choice;

    do {
        cout << "\n======= HOSTELPULSE =======\n";
        cout << "Complaint Management - Phase 1\n";
        cout << "1. Add complaint\n";
        cout << "2. Search complaint by ID\n";
        cout << "3. Display all complaints\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a valid number.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1:
                manager.addComplaint();
                break;
            case 2:
                manager.findComplaint();
                break;
            case 3:
                manager.displayAllComplaints();
                break;
            case 0:
                cout << "Exiting HostelPulse.\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
