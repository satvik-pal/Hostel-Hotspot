#include "Complaint.h"

Complaint::Complaint()
    : priority(Priority::Low),
      status(ComplaintStatus::Pending),
      arrivalOrder(0)
{
}

Complaint::Complaint(const std::string& complaintId, const std::string& studentId,
                     const std::string& category, const std::string& description,
                     const std::string& location, Priority priority, int arrivalOrder)
    : complaintId(complaintId),
      studentId(studentId),
      category(category),
      description(description),
      location(location),
      priority(priority),
      status(ComplaintStatus::Pending),
      arrivalOrder(arrivalOrder)
{
}

std::string Complaint::getId() const { return complaintId; }
std::string Complaint::getStudentId() const { return studentId; }
std::string Complaint::getCategory() const { return category; }
std::string Complaint::getDescription() const { return description; }
std::string Complaint::getLocation() const { return location; }
Priority Complaint::getPriority() const { return priority; }
ComplaintStatus Complaint::getStatus() const { return status; }
int Complaint::getArrivalOrder() const { return arrivalOrder; }
std::string Complaint::getAssignedWorkerId() const { return assignedWorkerId; }

bool Complaint::markAssigned(const std::string& workerId)
{
    if (status != ComplaintStatus::Pending || workerId.empty())
    {
        return false;
    }
    status = ComplaintStatus::Assigned;
    assignedWorkerId = workerId;
    return true;
}

bool Complaint::markResolved()
{
    if (status != ComplaintStatus::Assigned)
    {
        return false;
    }
    status = ComplaintStatus::Resolved;
    return true;
}

std::string priorityToString(Priority priority)
{
    switch (priority)
    {
        case Priority::High:   return "High";
        case Priority::Medium: return "Medium";
        default:               return "Low";
    }
}

std::string statusToString(ComplaintStatus status)
{
    switch (status)
    {
        case ComplaintStatus::Assigned: return "Assigned";
        case ComplaintStatus::Resolved: return "Resolved";
        default:                        return "Pending";
    }
}
