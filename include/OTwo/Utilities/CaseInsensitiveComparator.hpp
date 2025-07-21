#pragma once

#include <string>

struct CaseInsensitiveComparator
{
    bool operator()(const std::string& a, const std::string& b) const noexcept
    {
        return std::lexicographical_compare(
            a.begin(), a.end(),
            b.begin(), b.end(),
            [](unsigned char c1, unsigned char c2) {
                return std::tolower(c1) < std::tolower(c2);
            }
        );
    }
};
