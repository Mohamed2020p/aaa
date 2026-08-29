#pragma once
//
// ui.h — a small immediate-mode UI toolkit drawn with GDI+.
//
// One Ui instance is created per painted frame. Widgets read a shared
// Input snapshot (mouse/keyboard state) and fire when the mouse was
// pressed inside their rect. There is no hidden widget state — all
// persistent state lives in the app (Settings / page flags).
//
#include <gdiplus.h>
#include <string>
#include <vector>

#include "theme.h"

namespace ui {

// Input snapshot shared between the window procedure and the UI frame.
struct Input {
    float mouseX = 0.0f, mouseY = 0.0f;
    bool  mouseDown = false;    // left button currently held
    bool  mousePressed = false; // went down since the last frame
    bool  mouseReleased = false;// went up since the last frame
    int   lastKey = 0;          // virtual key of the last WM_KEYDOWN
    bool  comboClick = false;   // a combo widget handled this frame's click
    bool  fieldClick = false;   // a text field handled this frame's click
    std::vector<wchar_t> chars; // characters typed since the last frame

    void BeginFrame() {
        mousePressed = false;
        mouseReleased = false;
        lastKey = 0;
        comboClick = false;
        fieldClick = false;
        chars.clear();
    }
};

class Ui {
public:
    Ui(Gdiplus::Graphics& g, const Gdiplus::RectF& client, Input& input,
       const theme::Palette& palette, float fontScale);

    Gdiplus::Graphics&   G()      { return g_; }
    const Gdiplus::RectF& Client() const { return client_; }
    Input&               In()     { return input_; }
    const theme::Palette& Pal() const { return pal_; }

    // Font size scaled by the user's font scale setting.
    float S(float size) const { return size * scale_; }

    // ----- hit tests ----------------------------------------------------
    bool InRect(const Gdiplus::RectF& r, float pad = 0.0f) const;
    bool Hovered(const Gdiplus::RectF& r, float pad = 0.0f) const;
    bool Clicked(const Gdiplus::RectF& r, float pad = 0.0f);

    // ----- primitives -----------------------------------------------------
    void Text(float x, float y, const wchar_t* s, float size, Gdiplus::Color c,
              Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) const;
    // Left-aligned at box.X, vertically centered in box.
    void TextCenterV(const Gdiplus::RectF& box, const wchar_t* s, float size, Gdiplus::Color c,
                     Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) const;
    // Centered horizontally and vertically in box.
    void TextCenter(const Gdiplus::RectF& box, const wchar_t* s, float size, Gdiplus::Color c,
                    Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) const;
    Gdiplus::RectF Measure(const wchar_t* s, float size,
                           Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) const;
    void Divider(float x1, float x2, float y, Gdiplus::Color c) const;

    // ----- widgets ----------------------------------------------------------
    // Returns true when clicked this frame.
    bool Button(const Gdiplus::RectF& r, const wchar_t* label, Gdiplus::Color bg, Gdiplus::Color fg,
                float fontSize = 13.0f, bool bold = false);
    // Row label (+ optional subtitle), left-aligned.
    void OptionLabel(const Gdiplus::RectF& row, const wchar_t* label, const wchar_t* sub) const;
    // 44x24 pill toggle drawn with its top-left corner at (x, y).
    void ToggleSwitch(float x, float y, bool* value);
    // Label + value on top, draggable track below.
    void SliderRow(const Gdiplus::RectF& row, const wchar_t* label, int minV, int maxV,
                   int* value, const wchar_t* suffix);
    // Dropdown. `openCombo` stores the id of the currently open combo (-1 = none).
    void Combo(const Gdiplus::RectF& box, const wchar_t* label, const wchar_t** items, int count,
               int* selected, int* openCombo, int comboId);
    // Row of accent-color swatches; top-left of the first at (x, y).
    void Swatches(float x, float y, int* selected);
    // Single-line input (password mode masks characters). Clicking sets fieldClick.
    void TextField(const Gdiplus::RectF& r, const std::wstring& value, const wchar_t* placeholder,
                   bool focused, bool password);
    // Card with optional title/subtitle.
    void Card(const Gdiplus::RectF& r, const wchar_t* title = nullptr, const wchar_t* sub = nullptr);

private:
    Gdiplus::Graphics&    g_;
    Gdiplus::RectF        client_;
    Input&                input_;
    const theme::Palette& pal_;
    float                 scale_;
};

} // namespace ui
