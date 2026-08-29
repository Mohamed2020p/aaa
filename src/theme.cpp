//
// theme.cpp — palette construction + GDI+ drawing helpers.
//
#include "theme.h"

#include <map>
#include <utility>

namespace theme {

Palette MakePalette(int accentIndex) {
    if (accentIndex < 0) accentIndex = 0;
    if (accentIndex >= kAccentCount) accentIndex = kAccentCount - 1;
    Gdiplus::Color accent = kAccents[accentIndex].color;

    Palette p;
    p.bg         = Gdiplus::Color(255, 13, 15, 19);
    p.sidebar    = Gdiplus::Color(255, 17, 20, 26);
    p.card       = Gdiplus::Color(255, 23, 27, 34);
    p.cardHover  = Gdiplus::Color(255, 30, 36, 46);
    p.border     = Gdiplus::Color(255, 38, 45, 57);
    p.text       = Gdiplus::Color(255, 232, 235, 241);
    p.textDim    = Gdiplus::Color(255, 146, 156, 171);
    p.textFaint  = Gdiplus::Color(255, 97, 105, 119);
    p.accent     = accent;
    p.accentSoft = Gdiplus::Color(44, accent.GetR(), accent.GetG(), accent.GetB());
    p.danger     = Gdiplus::Color(255, 239, 68, 68);
    p.success    = Gdiplus::Color(255, 52, 211, 153);
    return p;
}

Gdiplus::Font* GetFont(float size, Gdiplus::FontStyle style) {
    static std::map<std::pair<int, int>, Gdiplus::Font*> cache;
    int key1 = (int)(size * 100);
    int key2 = (int)style;
    std::pair<int, int> key(key1, key2);
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;

    Gdiplus::FontFamily family(kFontFamily);
    Gdiplus::Font* f = new Gdiplus::Font(&family, size, key2, Gdiplus::UnitPixel);
    cache[key] = f;
    return f;
}

Gdiplus::Color Shade(Gdiplus::Color c, float factor) {
    if (factor > 1.0f) factor = 1.0f;
    if (factor < 0.0f) factor = 0.0f;
    return Gdiplus::Color(c.GetA(),
                          (BYTE)(c.GetR() * factor),
                          (BYTE)(c.GetG() * factor),
                          (BYTE)(c.GetB() * factor));
}

Gdiplus::GraphicsPath RoundedRectPath(const Gdiplus::RectF& r, float radius) {
    Gdiplus::GraphicsPath path;
    float d = radius * 2.0f;
    if (d > r.Width)  d = r.Width;
    if (d > r.Height) d = r.Height;
    path.StartFigure();
    path.AddArc(r.X, r.Y, d, d, 180.0f, 90.0f);
    path.AddArc(r.GetRight() - d, r.Y, d, d, 270.0f, 90.0f);
    path.AddArc(r.GetRight() - d, r.GetBottom() - d, d, d, 0.0f, 90.0f);
    path.AddArc(r.X, r.GetBottom() - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
    return path;
}

void FillRounded(Gdiplus::Graphics& g, const Gdiplus::RectF& r, float radius, Gdiplus::Color c) {
    Gdiplus::GraphicsPath path = RoundedRectPath(r, radius);
    Gdiplus::SolidBrush brush(c);
    g.FillPath(&brush, &path);
}

void StrokeRounded(Gdiplus::Graphics& g, const Gdiplus::RectF& r, float radius,
                   Gdiplus::Color c, float penWidth) {
    Gdiplus::GraphicsPath path = RoundedRectPath(r, radius);
    Gdiplus::Pen pen(c, penWidth);
    g.DrawPath(&pen, &path);
}

} // namespace theme
