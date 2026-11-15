#include "ComplaintManager.h"
#include "PriorityRules.h"

std::string ComplaintManager::generateComplaintId()
{
    std::string number = std::to_string(nextId);
    while (number.length() < 3)
    {
        number = "0" + number;
    }
    return "C" + number;
}

std::optional<std::string> ComplaintManager::addComplaint(const std::string& studentId,
                                                          const std::string& category,
                                                          const std::string& description,
                                                          const std::string& location)
{
    if (studentId.empty() || description.empty() || location.empty() ||
        !PriorityRules::isValidCategory(category))
    {
        return std::nullopt;                // invalid input: no ID is used up
    }

    int order = nextId;
    std::string id = generateComplaintId();
    nextId++;

    Priority priority = PriorityRules::assignPriority(category, description);

    complaints.insert(id, Complaint(id, studentId, category, description, location, priority, order));
    pendingQueue.push({static_cast<int>(priority), order, id});
    return id;
}

const Complaint* ComplaintManager::findComplaint(const std::string& complaintId) const
{
    return complaints.find(complaintId);
}

// Returns the most urgent complaint that is still Pending, without removing it.
// Entries for complaints that are no longer Pending are discarded here (lazy deletion).
std::optional<std::string> ComplaintManager::nextPending()
{
    QueuedComplaint top;
    while (pendingQueue.peek(top))
    {
        const Complaint* complaint = complaints.find(top.complaintId);
        if (complaint != nullptr && complaint->getStatus() == ComplaintStatus::Pending)
        {
            return top.complaintId;
        }
        pendingQueue.pop(top);              // outdated entry: already assigned
    }
    return std::nullopt;
}

bool ComplaintManager::markAssigned(const std::string& complaintId, const std::string& workerId)
{
    Complaint* complaint = complaints.find(complaintId);
    if (complaint == nullptr)
    {
        return false;
    }
    return complaint->markAssigned(workerId);
}

bool ComplaintManager::markResolved(const std::string& complaintId)
{
    Complaint* complaint = complaints.find(complaintId);
    if (complaint == nullptr)
    {
        return false;
    }
    return complaint->markResolved();
}

int ComplaintManager::getComplaintCount() const
{
    return complaints.size();
}

int ComplaintManager::getPendingCount() const
{
    int pending = 0;
    complaints.forEach([&](const std::string&, const Complaint& complaint)
    {
        if (complaint.getStatus() == ComplaintStatus::Pending)
        {
            pending++;
        }
    });
    return pending;
}
