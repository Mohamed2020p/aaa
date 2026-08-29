//
// ui.cpp — widget implementations (GDI+ rendering + hit testing).
//
#include "ui.h"

#include <cmath>
#include <cwchar>

namespace ui {

Ui::Ui(Gdiplus::Graphics& g, const Gdiplus::RectF& client, Input& input,
       const theme::Palette& palette, float fontScale)
    : g_(g), client_(client), input_(input), pal_(palette), scale_(fontScale) {}

// ----- hit tests ---------------------------------------------------------

bool Ui::InRect(const Gdiplus::RectF& r, float pad) const {
    return input_.mouseX >= r.X - pad && input_.mouseX <= r.GetRight() + pad &&
           input_.mouseY >= r.Y - pad && input_.mouseY <= r.GetBottom() + pad;
}

bool Ui::Hovered(const Gdiplus::RectF& r, float pad) const {
    return InRect(r, pad);
}

bool Ui::Clicked(const Gdiplus::RectF& r, float pad) {
    return input_.mousePressed && InRect(r, pad);
}

// ----- primitives ----------------------------------------------------------

void Ui::Text(float x, float y, const wchar_t* s, float size, Gdiplus::Color c,
              Gdiplus::FontStyle style) const {
    Gdiplus::Font* f = theme::GetFont(S(size), style);
    Gdiplus::SolidBrush brush(c);
    Gdiplus::PointF pt(x, y);
    g_.DrawString(s, -1, f, pt, &brush);
}

void Ui::TextCenterV(const Gdiplus::RectF& box, const wchar_t* s, float size, Gdiplus::Color c,
                     Gdiplus::FontStyle style) const {
    Gdiplus::Font* f = theme::GetFont(S(size), style);
    Gdiplus::RectF m(0.0f, 0.0f, 0.0f, 0.0f);
    Gdiplus::PointF origin(0.0f, 0.0f);
    g_.MeasureString(s, -1, f, origin, &m);
    Gdiplus::SolidBrush brush(c);
    Gdiplus::PointF pt(box.X, box.Y + (box.Height - m.Height) / 2.0f);
    g_.DrawString(s, -1, f, pt, &brush);
}

void Ui::TextCenter(const Gdiplus::RectF& box, const wchar_t* s, float size, Gdiplus::Color c,
                    Gdiplus::FontStyle style) const {
    Gdiplus::Font* f = theme::GetFont(S(size), style);
    Gdiplus::SolidBrush brush(c);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::AlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::AlignmentCenter);
    g_.DrawString(s, -1, f, box, &brush, &fmt);
}

Gdiplus::RectF Ui::Measure(const wchar_t* s, float size, Gdiplus::FontStyle style) const {
    Gdiplus::Font* f = theme::GetFont(S(size), style);
    Gdiplus::RectF out(0.0f, 0.0f, 0.0f, 0.0f);
    Gdiplus::PointF origin(0.0f, 0.0f);
    g_.MeasureString(s, -1, f, origin, &out);
    return out;
}

void Ui::Divider(float x1, float x2, float y, Gdiplus::Color c) const {
    Gdiplus::Pen pen(c, 1.0f);
    g_.DrawLine(&pen, x1, y, x2, y);
}

// ----- widgets --------------------------------------------------------------

bool Ui::Button(const Gdiplus::RectF& r, const wchar_t* label, Gdiplus::Color bg, Gdiplus::Color fg,
                float fontSize, bool bold) {
    Gdiplus::FontStyle style = bold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular;
    Gdiplus::Color fill = bg;
    if (InRect(r) && input_.mouseDown) fill = theme::Shade(bg, 0.72f);
    else if (Hovered(r)) fill = theme::Shade(bg, 0.88f);
    theme::FillRounded(g_, r, 10.0f, fill);
    TextCenter(r, label, fontSize, fg, style);
    return Clicked(r);
}

void Ui::OptionLabel(const Gdiplus::RectF& row, const wchar_t* label, const wchar_t* sub) const {
    if (sub) {
        Text(row.X, row.Y + row.Height / 2.0f - 14.0f, label, 13.0f, pal_.text);
        Text(row.X, row.Y + row.Height / 2.0f + 4.0f, sub, 10.5f, pal_.textDim);
    } else {
        TextCenterV(row, label, 13.0f, pal_.text);
    }
}

void Ui::ToggleSwitch(float x, float y, bool* value) {
    Gdiplus::RectF track(x, y, 44.0f, 24.0f);
    Gdiplus::Color bgc = *value ? pal_.accent : Gdiplus::Color(255, 40, 46, 57);
    if (InRect(track) && input_.mouseDown) bgc = theme::Shade(bgc, 0.85f);
    theme::FillRounded(g_, track, 12.0f, bgc);

    float kx = *value ? track.GetRight() - 20.0f : track.X + 4.0f;
    Gdiplus::SolidBrush knobBrush(Gdiplus::Color(255, 255, 255, 255));
    g_.FillEllipse(&knobBrush, kx, track.Y + 4.0f, 16.0f, 16.0f);

    if (Clicked(track, 4.0f)) *value = !*value;
}

void Ui::SliderRow(const Gdiplus::RectF& row, const wchar_t* label, int minV, int maxV,
                   int* value, const wchar_t* suffix) {
    Text(row.X, row.Y + 4.0f, label, 13.0f, pal_.text);

    wchar_t buf[48] = L"";
    if (suffix) swprintf(buf, 48, L"%d %s", *value, suffix);
    else swprintf(buf, 48, L"%d", *value);
    Gdiplus::RectF vm = Measure(buf, 12.5f, Gdiplus::FontStyleBold);
    Text(row.GetRight() - vm.Width, row.Y + 4.5f, buf, 12.5f, pal_.accent, Gdiplus::FontStyleBold);

    Gdiplus::RectF track(row.X, row.Y + 32.0f, row.Width, 6.0f);
    theme::FillRounded(g_, track, 3.0f, Gdiplus::Color(255, 37, 43, 54));

    float t = 0.0f;
    if (maxV > minV) t = (float)(*value - minV) / (float)(maxV - minV);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float fillW = track.Width * t;
    if (fillW > 6.0f)
        theme::FillRounded(g_, Gdiplus::RectF(track.X, track.Y, fillW, track.Height), 3.0f, pal_.accent);

    float kx = track.X + fillW - 8.0f;
    Gdiplus::RectF knob(kx, track.Y - 5.0f, 16.0f, 16.0f);

    Gdiplus::RectF hitArea(track.X, track.Y - 12.0f, track.Width, 28.0f);
    if (InRect(hitArea) && input_.mouseDown) {
        int v = minV + (int)std::lround(((input_.mouseX - track.X) / track.Width) * (float)(maxV - minV));
        if (v < minV) v = minV;
        if (v > maxV) v = maxV;
        *value = v;
    }

    Gdiplus::SolidBrush knobBrush(Gdiplus::Color(255, 244, 246, 250));
    g_.FillEllipse(&knobBrush, knob.X, knob.Y, knob.Width, knob.Height);
    Gdiplus::Pen knobPen(pal_.accent, 1.8f);
    g_.DrawEllipse(&knobPen, knob.X, knob.Y, knob.Width, knob.Height);
}

void Ui::Combo(const Gdiplus::RectF& box, const wchar_t* label, const wchar_t** items, int count,
               int* selected, int* openCombo, int comboId) {
    (void)label;
    bool open = (*openCombo == comboId);

    theme::FillRounded(g_, box, 9.0f, open ? pal_.cardHover : pal_.card);
    theme::StrokeRounded(g_, box, 9.0f, open ? pal_.accent : pal_.border, open ? 1.5f : 1.0f);

    const wchar_t* cur = (selected && *selected >= 0 && *selected < count) ? items[*selected] : L"";
    Gdiplus::RectF textRect(box.X + 12.0f, box.Y, box.Width - 32.0f, box.Height);
    TextCenterV(textRect, cur, 12.5f, pal_.text);

    // Chevron (points down when closed, up when open).
    float cx = box.GetRight() - 18.0f;
    float cy = box.Y + box.Height / 2.0f;
    float d = open ? -1.0f : 1.0f;
    Gdiplus::Pen chev(open ? pal_.accent : pal_.textDim, 1.6f);
    chev.SetStartCap(Gdiplus::LineCapRound);
    chev.SetEndCap(Gdiplus::LineCapRound);
    g_.DrawLine(&chev, cx - 4.0f, cy - 2.5f * d, cx, cy + 2.0f * d);
    g_.DrawLine(&chev, cx, cy + 2.0f * d, cx + 4.0f, cy - 2.5f * d);

    if (Clicked(box)) {
        input_.comboClick = true;
        *openCombo = open ? -1 : comboId;
        return;
    }
    if (!open) return;

    // Dropdown list (opens upward if there is not enough room below).
    const float itemH = 30.0f;
    const float listPad = 6.0f;
    const float listH = itemH * count + listPad * 2.0f;
    float listY = box.GetBottom() + 6.0f;
    if (listY + listH > client_.GetBottom() - 8.0f) listY = box.Y - 6.0f - listH;

    Gdiplus::RectF list(box.X, listY, box.Width, listH);
    theme::FillRounded(g_, list, 10.0f, pal_.card);
    theme::StrokeRounded(g_, list, 10.0f, pal_.border, 1.0f);

    for (int i = 0; i < count; i++) {
        Gdiplus::RectF item(box.X + 6.0f, list.Y + listPad + i * itemH, box.Width - 12.0f, itemH - 2.0f);
        bool isSel = (*selected == i);
        if (isSel) theme::FillRounded(g_, item, 7.0f, pal_.accentSoft);
        else if (Hovered(item)) theme::FillRounded(g_, item, 7.0f, pal_.cardHover);
        TextCenterV(item, items[i], 12.5f, isSel ? pal_.accent : pal_.text,
                    isSel ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular);
        if (Clicked(item)) {
            input_.comboClick = true;
            *selected = i;
            *openCombo = -1;
        }
    }
}

void Ui::Swatches(float x, float y, int* selected) {
    for (int i = 0; i < theme::kAccentCount; i++) {
        Gdiplus::RectF sw(x + i * 36.0f, y, 24.0f, 24.0f);
        if (i == *selected) {
            Gdiplus::RectF ring(sw.X - 4.0f, sw.Y - 4.0f, sw.Width + 8.0f, sw.Height + 8.0f);
            theme::StrokeRounded(g_, ring, 12.0f, theme::kAccents[i].color, 2.0f);
        }
        Gdiplus::SolidBrush b(theme::kAccents[i].color);
        g_.FillEllipse(&b, sw.X, sw.Y, sw.Width, sw.Height);
        if (Clicked(sw, 5.0f)) *selected = i;
    }
}

void Ui::TextField(const Gdiplus::RectF& r, const std::wstring& value, const wchar_t* placeholder,
                   bool focused, bool password) {
    theme::FillRounded(g_, r, 9.0f, Gdiplus::Color(255, 16, 19, 25));
    theme::StrokeRounded(g_, r, 9.0f, focused ? pal_.accent : pal_.border, focused ? 1.5f : 1.0f);

    std::wstring shown = password ? std::wstring(value.size(), L'\u2022') : value;
    Gdiplus::RectF textRow(r.X + 12.0f, r.Y, r.Width - 24.0f, r.Height);
    if (shown.empty()) TextCenterV(textRow, placeholder, 13.0f, pal_.textFaint);
    else TextCenterV(textRow, shown.c_str(), 13.0f, pal_.text);

    if (focused) {
        float cx = r.X + 12.0f;
        if (!shown.empty()) {
            Gdiplus::RectF m = Measure(shown.c_str(), 13.0f);
            cx += m.Width - 1.0f;
        }
        float top = r.Y + r.Height / 2.0f - 9.0f;
        float bot = r.Y + r.Height / 2.0f + 9.0f;
        if (((GetTickCount() / 500) % 2) == 0) {
            Gdiplus::Pen caret(pal_.accent, 1.5f);
            g_.DrawLine(&caret, cx, top, cx, bot);
        }
    }

    if (Clicked(r)) input_.fieldClick = true;
}

void Ui::Card(const Gdiplus::RectF& r, const wchar_t* title, const wchar_t* sub) {
    theme::FillRounded(g_, r, 14.0f, pal_.card);
    theme::StrokeRounded(g_, r, 14.0f, pal_.border, 1.0f);
    if (!title) return;

    Gdiplus::RectF titleBox(r.X + 18.0f, r.Y + 14.0f, r.Width - 36.0f, 24.0f);
    TextCenterV(titleBox, title, 14.0f, pal_.text, Gdiplus::FontStyleBold);
    if (sub) {
        Gdiplus::RectF tm = Measure(title, 14.0f, Gdiplus::FontStyleBold);
        Gdiplus::RectF subBox(r.X + 30.0f + tm.Width, r.Y + 18.0f, r.Width - 60.0f - tm.Width, 18.0f);
        TextCenterV(subBox, sub, 10.5f, pal_.textFaint);
    }
}

} // namespace ui
