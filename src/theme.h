#pragma once
//
// theme.h — palette, layout constants and small GDI+ drawing helpers.
//
#include <gdiplus.h>

namespace theme {

// ----- window & layout ----------------------------------------------
inline const int kWindowWidth  = 1040;
inline const int kWindowHeight = 640;
inline const int kTitleBarH    = 44;
inline const int kSidebarW     = 224;

// ----- palette --------------------------------------------------------
struct Palette {
    Gdiplus::Color bg;
    Gdiplus::Color sidebar;
    Gdiplus::Color card;
    Gdiplus::Color cardHover;
    Gdiplus::Color border;
    Gdiplus::Color text;
    Gdiplus::Color textDim;
    Gdiplus::Color textFaint;
    Gdiplus::Color accent;
    Gdiplus::Color accentSoft;
    Gdiplus::Color danger;
    Gdiplus::Color success;
};

struct Accent {
    const wchar_t* name;
    Gdiplus::Color color;
};

inline const Accent kAccents[] = {
    { L"Blue",    Gdiplus::Color(255, 76, 139, 255) },
    { L"Violet",  Gdiplus::Color(255, 139, 92, 246) },
    { L"Emerald", Gdiplus::Color(255, 34, 197, 94)  },
    { L"Amber",   Gdiplus::Color(255, 245, 158, 11) },
    { L"Crimson", Gdiplus::Color(255, 239, 68, 68)  },
};
inline const int kAccentCount = 5;

Palette MakePalette(int accentIndex);

inline const wchar_t* kFontFamily = L"Segoe UI";

// ----- helpers ---------------------------------------------------------
// Returns a cached font for the given size/style (never deleted).
Gdiplus::Font* GetFont(float size, Gdiplus::FontStyle style = Gdiplus::FontStyleRegular);

// Multiply RGB channels by `factor` (0..1) — used for hover/press shades.
Gdiplus::Color Shade(Gdiplus::Color c, float factor);

// Rounded-rectangle path + convenience fill/stroke.
Gdiplus::GraphicsPath RoundedRectPath(const Gdiplus::RectF& r, float radius);
void FillRounded(Gdiplus::Graphics& g, const Gdiplus::RectF& r, float radius, Gdiplus::Color c);
void StrokeRounded(Gdiplus::Graphics& g, const Gdiplus::RectF& r, float radius,
                   Gdiplus::Color c, float penWidth = 1.0f);

} // namespace theme
