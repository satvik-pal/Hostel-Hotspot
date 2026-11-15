#include "PriorityRules.h"

#include <cctype>

namespace
{
    const int HIGH_COUNT = 6;
    const std::string HIGH_KEYWORDS[HIGH_COUNT] = {
        "fire", "sparking", "exposed wire", "gas leak", "flood", "short circuit"
    };

    const int MEDIUM_COUNT = 4;
    const std::string MEDIUM_KEYWORDS[MEDIUM_COUNT] = {
        "electrical", "plumbing", "water", "internet"
    };

    bool containsAny(const std::string& text, const std::string keywords[], int count)
    {
        for (int i = 0; i < count; i++)
        {
            if (text.find(keywords[i]) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }
}

namespace PriorityRules
{
    bool isValidCategory(const std::string& category)
    {
        for (int i = 0; i < CATEGORY_COUNT; i++)
        {
            if (CATEGORIES[i] == category)
            {
                return true;
            }
        }
        return false;
    }

    std::string toLowerCopy(const std::string& text)
    {
        std::string result = text;
        for (int i = 0; i < static_cast<int>(result.size()); i++)
        {
            result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
        }
        return result;
    }

    // Prototype rules: improve later using category, urgency and the affected area.
    Priority assignPriority(const std::string& category, const std::string& description)
    {
        std::string text = toLowerCopy(category + " " + description);

        if (containsAny(text, HIGH_KEYWORDS, HIGH_COUNT))
        {
            return Priority::High;
        }
        if (containsAny(text, MEDIUM_KEYWORDS, MEDIUM_COUNT))
        {
            return Priority::Medium;
        }
        return Priority::Low;
    }
}
