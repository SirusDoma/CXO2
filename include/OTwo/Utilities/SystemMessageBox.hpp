#pragma once

#include <string>

#ifdef USE_BOXER
#include <boxer/boxer.h>
#else
#include <messagebox-x11/messagebox.h>
#endif

class SystemMessageBox
{
public:
    static void ShowInformation(const std::string& message, const std::string& title)
    {
#ifdef USE_BOXER
        boxer::show(message.c_str(), title.c_str());
#else
        wchar_t text[] = L"OK";
        const std::vector<Button> buttons = { { text } };

        Messagebox(std::wstring(title.begin(), title.end()).c_str(), std::wstring(message.begin(), message.end()).c_str(), buttons.data(), 1);
#endif
    }

    static void ShowError(const std::string& message, const std::string& title)
    {
#ifdef USE_BOXER
        boxer::show(message.c_str(), title.c_str(), boxer::Style::Error);
#else
        wchar_t text[] = L"OK";
        const std::vector<Button> buttons = { { text } };

        Messagebox(std::wstring(title.begin(), title.end()).c_str(), std::wstring(message.begin(), message.end()).c_str(), buttons.data(), 1);
#endif
    }
};