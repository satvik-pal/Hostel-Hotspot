// HostelPulse - Complaint Management module tests.
// Build: CMake target complaint_tests; run with ctest or ./build/complaint_tests

#include <iostream>
#include <optional>
#include <string>

#include "ComplaintManager.h"
#include "PriorityRules.h"

using namespace std;

int passed = 0;
int failed = 0;

void check(bool condition, const string& description)
{
    if (condition)
    {
        cout << "  [PASS] " << description << endl;
        passed++;
    }
    else
    {
        cout << "  [FAIL] " << description << endl;
        failed++;
    }
}

void section(const string& title)
{
    cout << endl << "== " << title << " ==" << endl;
}

void testPriorityRules()
{
    section("1. Priority rules and categories");
    using namespace PriorityRules;

    check(assignPriority("Electrical", "Sparking from socket") == Priority::High,   "sparking -> High");
    check(assignPriority("Electrical", "Light not working") == Priority::Medium,   "electrical category -> Medium");
    check(assignPriority("Carpentry", "Broken chair") == Priority::Low,            "no keyword -> Low");
    check(assignPriority("Cleaning", "FIRE near bins") == Priority::High,          "uppercase keyword still detected");
    check(assignPriority("Electrical", "Short circuit in room") == Priority::High, "short circuit -> High");
    check(isValidCategory("Plumbing") && !isValidCategory("plumbing") && !isValidCategory("Painting"),
          "category check is exact and rejects unknown categories");
    check(toLowerCopy("AbC") == "abc", "toLowerCopy lowercases text");
}

void testRegistration(ComplaintManager& m)
{
    section("2. Registration");

    check(!m.nextPending(), "new manager has nothing pending");

    optional<string> a = m.addComplaint("S101", "Carpentry",  "Broken chair",  "Sai Hostel 204");
    optional<string> b = m.addComplaint("S102", "Plumbing",   "Tap leaking",   "Atlantis Hostel 110");
    optional<string> c = m.addComplaint("S103", "Electrical", "Sparking wire", "Mother Teresa 305");
    optional<string> d = m.addComplaint("S104", "Plumbing",   "No water",      "Sai Hostel 101");
    check(a && *a == "C001" && b && *b == "C002" && c && *c == "C003" && d && *d == "C004",
          "automatic IDs C001..C004");

    check(!m.addComplaint("",     "Plumbing", "x", "Sai Hostel 101") &&
          !m.addComplaint("S1",   "Painting", "x", "Sai Hostel 101") &&
          !m.addComplaint("S1",   "Plumbing", "",  "Sai Hostel 101") &&
          !m.addComplaint("S1",   "Plumbing", "x", ""),
          "empty fields and unknown category are rejected");
    check(m.getComplaintCount() == 4, "rejected complaints are not stored");

    optional<string> e = m.addComplaint("S105", "Cleaning", "Dusty corridor", "Mess");
    check(e && *e == "C005", "rejected calls do not use up IDs");

    const Complaint* x = m.findComplaint("C003");
    check(x != nullptr && x->getPriority() == Priority::High &&
          x->getStatus() == ComplaintStatus::Pending &&
          x->getLocation() == "Mother Teresa 305" &&
          x->getCategory() == "Electrical" && x->getArrivalOrder() == 3 &&
          x->getAssignedWorkerId().empty(),
          "stored complaint has all fields, Pending, no worker");
    check(m.findComplaint("C999") == nullptr, "missing complaint returns nullptr");
}

void testQueueAndStatus(ComplaintManager& m)
{
    section("3. Priority queue and status");

    optional<string> next = m.nextPending();
    check(next && *next == "C003", "High priority complaint comes first");
    next = m.nextPending();
    check(next && *next == "C003", "nextPending does not remove the complaint");

    check(m.markAssigned("C003", "W002"),  "Pending complaint can be assigned");
    check(!m.markAssigned("C003", "W001"), "assigned complaint cannot be assigned again");
    check(m.findComplaint("C003")->getAssignedWorkerId() == "W002", "assigned worker is recorded");

    next = m.nextPending();
    check(next && *next == "C002", "then Medium priority, oldest first (C002)");
    m.markAssigned("C002", "W003");
    next = m.nextPending();
    check(next && *next == "C004", "then the next Medium (C004)");
    m.markAssigned("C004", "W003");
    next = m.nextPending();
    check(next && *next == "C001", "then Low priority, oldest first (C001)");
    m.markAssigned("C001", "W001");
    next = m.nextPending();
    check(next && *next == "C005", "then the last Low (C005)");
    check(m.getPendingCount() == 1, "one complaint still pending");

    check(!m.markResolved("C005"), "a Pending complaint cannot be resolved");
    check(m.markResolved("C003"),  "an Assigned complaint can be resolved");
    check(!m.markResolved("C003"), "a Resolved complaint cannot be resolved again");
    check(m.findComplaint("C003")->getStatus() == ComplaintStatus::Resolved, "status is Resolved");

    check(!m.markAssigned("C999", "W1") && !m.markResolved("C999"), "unknown complaint IDs are rejected");
    check(!m.markAssigned("C005", ""), "empty worker ID is rejected");

    m.markAssigned("C005", "W009");
    check(!m.nextPending() && m.getPendingCount() == 0, "queue is empty when everything is assigned");

    int visited = 0;
    const ComplaintManager& readOnly = m;
    readOnly.forEachComplaint([&](const Complaint&) { visited++; });
    check(visited == 5, "forEachComplaint visits all complaints (read-only)");
}

void testScale()
{
    section("4. Scale and strings");

    ComplaintManager big;
    for (int i = 0; i < 1000; i++)
    {
        big.addComplaint("S1", "Cleaning", "Dust", "Mess");
    }
    optional<string> next = big.nextPending();
    check(big.findComplaint("C1000") != nullptr && next && *next == "C001",
          "1000 complaints: equal priority comes out oldest first");

    check(priorityToString(Priority::High) == "High" &&
          statusToString(ComplaintStatus::Assigned) == "Assigned",
          "priority and status names");
}

int main()
{
    cout << "HostelPulse - Complaint module tests" << endl;

    testPriorityRules();

    ComplaintManager manager;
    testRegistration(manager);
    testQueueAndStatus(manager);
    testScale();

    cout << endl << "Result: " << passed << " passed, " << failed << " failed" << endl;
    return failed == 0 ? 0 : 1;
}
