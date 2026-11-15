#ifndef COMPLAINT_H
#define COMPLAINT_H

#include <string>

enum class Priority
{
    Low = 1,
    Medium = 2,
    High = 3
};

enum class ComplaintStatus
{
    Pending,        // waiting for a worker
    Assigned,       // a worker has been allocated
    Resolved        // the work is done
};

class Complaint
{
private:
    std::string complaintId;
    std::string studentId;
    std::string category;           // also the required worker skill, e.g. "Plumbing"
    std::string description;
    std::string location;           // a campus location name, e.g. "Sai Hostel 204"
    Priority priority;
    ComplaintStatus status;
    int arrivalOrder;               // 1, 2, 3... in order of registration
    std::string assignedWorkerId;   // empty until assigned

public:
    Complaint();
    Complaint(const std::string& complaintId, const std::string& studentId,
              const std::string& category, const std::string& description,
              const std::string& location, Priority priority, int arrivalOrder);

    std::string getId() const;
    std::string getStudentId() const;
    std::string getCategory() const;
    std::string getDescription() const;
    std::string getLocation() const;
    Priority getPriority() const;
    ComplaintStatus getStatus() const;
    int getArrivalOrder() const;
    std::string getAssignedWorkerId() const;

    bool markAssigned(const std::string& workerId);   // only Pending -> Assigned
    bool markResolved();                               // only Assigned -> Resolved
};

std::string priorityToString(Priority priority);
std::string statusToString(ComplaintStatus status);

#endif
