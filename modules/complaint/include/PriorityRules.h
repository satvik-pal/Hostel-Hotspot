#ifndef PRIORITY_RULES_H
#define PRIORITY_RULES_H

#include <string>
#include "Complaint.h"

namespace PriorityRules
{
    // Team-agreed categories. Each one is also a maintenance worker skill name.
    const int CATEGORY_COUNT = 6;
    const std::string CATEGORIES[CATEGORY_COUNT] = {
        "Electrical", "Plumbing", "Carpentry", "Cleaning", "AC Repair", "Internet"
    };

    bool isValidCategory(const std::string& category);     // exact, case-sensitive match
    std::string toLowerCopy(const std::string& text);
    Priority assignPriority(const std::string& category, const std::string& description);
}

#endif
