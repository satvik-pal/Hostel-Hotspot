#ifndef APP_CORE_H
#define APP_CORE_H

// Shared application layer for the GUI.
// Holds all four modules together and exposes the actions the windows need.
// No Qt code here, so this logic can be reused by any interface.

#include <string>

#include "AuthManager.h"
#include "Campus.h"
#include "ComplaintManager.h"
#include "MaintenanceManager.h"
#include "PriorityRules.h"
#include "StudentManager.h"

struct HostelSystem
{
    HostelGraph graph;
    RoomRegistry rooms;
    StudentManager students;
    AuthManager auth;
    ComplaintManager complaints;
    MaintenanceManager maintenance;
};

struct ActionResult
{
    bool success = false;
    std::string message;
};

struct HotspotEntry
{
    std::string name;
    int count = 0;
};

struct HotspotReport
{
    int total = 0;
    DynamicArray<HotspotEntry> locations;
    DynamicArray<HotspotEntry> hostels;
    DynamicArray<HotspotEntry> categories;
};

bool setupSystem(HostelSystem& sys);

ActionResult registerStudent(HostelSystem& sys,
                             const std::string& id, const std::string& name,
                             const std::string& phone, const std::string& hostel,
                             const std::string& room, const std::string& password);

ActionResult raiseComplaint(HostelSystem& sys, const std::string& studentId,
                            const std::string& category, const std::string& description,
                            const std::string& location);

ActionResult assignComplaint(HostelSystem& sys, const std::string& complaintId);
ActionResult assignNextPending(HostelSystem& sys);
ActionResult resolveComplaint(HostelSystem& sys, const std::string& complaintId);

ActionResult addWorker(HostelSystem& sys, const std::string& name,
                       const std::string& skill, const std::string& location);
ActionResult toggleWorkerDuty(HostelSystem& sys, const std::string& workerId);

std::string routeToString(const HostelGraph& graph, const RouteResult& route);
HotspotReport buildHotspotReport(const HostelSystem& sys);

#endif

