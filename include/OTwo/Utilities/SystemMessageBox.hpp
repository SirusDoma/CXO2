#pragma once

#include <string>

#include <tinyfiledialogs/tinyfiledialogs.h>

class SystemMessageBox
{
public:
    static void ShowInformation(const std::string& message, const std::string& title)
    {
        tinyfd_messageBox(title.c_str(), message.c_str(), "ok", "info", 1);
    }

    static void ShowError(const std::string& message, const std::string& title)
    {
        tinyfd_messageBox(title.c_str(), message.c_str(), "ok", "error", 1);
    }
};