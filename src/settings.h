#pragma once
//
// settings.h — application state + account constants.
//
#include <string>

struct Settings {
    // ----- General -----
    bool notifications = true;
    bool startOnBoot   = false;
    bool autoUpdate    = true;
    int  volume        = 70;   // 0..100 (%)
    int  quality       = 2;    // index into kQualityLevels

    // ----- Interface -----
    int  accent        = 0;    // index into theme::kAccents
    int  fontScale     = 100;  // 80..140 (%)
    bool animations    = true;
    bool compactMode   = false;

    // ----- Network -----
    bool p2pMode       = false;
    bool showLatency   = true;
    int  maxPeers      = 4;    // 1..16

    // ----- Security -----
    bool twoFactor     = false;
    int  sessionTimeout = 30;  // 5..120 (minutes)
    std::wstring password = L"2"; // demo password, kept in memory only
};

namespace account {
    // Demo credentials — replace with real authentication in any real app.
    inline const std::wstring kDefaultUser     = L"2";
    inline const std::wstring kDefaultPassword = L"2";

    // Branding.
    inline const wchar_t* kAppTitle  = L"ALIENTX";
    inline const wchar_t* kVersion   = L"1.0.0";
    inline const wchar_t* kDeveloper = L"AlienX";
}

inline const wchar_t* kQualityLevels[] = { L"Low", L"Medium", L"High", L"Ultra" };
inline const int kQualityCount = 4;

// Trim leading/trailing whitespace.
inline std::wstring Trim(const std::wstring& s) {
    size_t b = s.find_first_not_of(L" \t");
    if (b == std::wstring::npos) return L"";
    size_t e = s.find_last_not_of(L" \t");
    return s.substr(b, e - b + 1);
}
