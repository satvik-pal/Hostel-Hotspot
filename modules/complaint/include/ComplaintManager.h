#ifndef COMPLAINT_MANAGER_H
#define COMPLAINT_MANAGER_H

#include <optional>
#include <string>
#include "Complaint.h"
#include "HashMap.h"
#include "MinHeap.h"

// One entry in the scheduling queue. "Smaller" = should be handled first.
struct QueuedComplaint
{
    int priority = 0;           // 3 = High, 2 = Medium, 1 = Low
    int arrivalOrder = 0;
    std::string complaintId;

    bool operator<(const QueuedComplaint& other) const
    {
        if (priority != other.priority)
        {
            return priority > other.priority;        // higher priority first
        }
        return arrivalOrder < other.arrivalOrder;    // same priority: first come, first served
    }
};

class ComplaintManager
{
private:
    HashMap<Complaint> complaints;          // complaint ID -> Complaint
    MinHeap<QueuedComplaint> pendingQueue;  // scheduling order
    int nextId = 1;

    std::string generateComplaintId();

public:
    std::optional<std::string> addComplaint(const std::string& studentId,
                                            const std::string& category,
                                            const std::string& description,
                                            const std::string& location);
    const Complaint* findComplaint(const std::string& complaintId) const;
    std::optional<std::string> nextPending();
    bool markAssigned(const std::string& complaintId, const std::string& workerId);
    bool markResolved(const std::string& complaintId);
    int getComplaintCount() const;
    int getPendingCount() const;

    template <typename Action>
    void forEachComplaint(Action action) const
    {
        complaints.forEach([&](const std::string&, const Complaint& complaint) { action(complaint); });
    }
};

#endif
