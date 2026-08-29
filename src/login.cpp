//
// login.cpp — the sign-in screen.
//
#include "app.h"

void App::RenderLogin(ui::Ui& u) {
    const Gdiplus::RectF client = u.Client();
    const theme::Palette& pal = u.Pal();

    const float cardW = 384.0f;
    const float cardH = 452.0f;
    Gdiplus::RectF card((client.Width - cardW) / 2.0f,
                        theme::kTitleBarH + (client.Height - theme::kTitleBarH - cardH) / 2.0f,
                        cardW, cardH);
    u.Card(card, nullptr, nullptr);

    const float inset = 32.0f;
    float x = card.X + inset;
    float w = cardW - inset * 2.0f;
    float y = card.Y + 34.0f;

    // Logo.
    Gdiplus::RectF logo(card.X + cardW / 2.0f - 26.0f, y, 52.0f, 52.0f);
    theme::FillRounded(u.G(), logo, 14.0f, pal.accent);
    u.TextCenter(logo, L"A", 24.0f, Gdiplus::Color(255, 255, 255, 255), Gdiplus::FontStyleBold);
    y = logo.GetBottom() + 20.0f;

    // Title + subtitle.
    Gdiplus::RectF titleBox(x, y, w, 26.0f);
    u.TextCenter(titleBox, account::kAppTitle, 18.0f, pal.text, Gdiplus::FontStyleBold);
    y += 28.0f;
    Gdiplus::RectF subBox(x, y, w, 18.0f);
    u.TextCenter(subBox, (std::wstring(L"Sign in to continue  •  v") + account::kVersion).c_str(),
                 11.5f, pal.textDim);
    y += 36.0f;

    // Username.
    u.Text(x, y, L"USERNAME", 10.5f, pal.textDim, Gdiplus::FontStyleBold);
    y += 18.0f;
    Gdiplus::RectF userField(x, y, w, 40.0f);
    u.TextField(userField, loginUser_, L"Enter username", focusedField_ == 0, false);
    if (u.Clicked(userField)) focusedField_ = 0;
    y = userField.GetBottom() + 16.0f;

    // Password.
    u.Text(x, y, L"PASSWORD", 10.5f, pal.textDim, Gdiplus::FontStyleBold);
    y += 18.0f;
    Gdiplus::RectF passField(x, y, w, 40.0f);
    u.TextField(passField, loginPass_, L"Enter password", focusedField_ == 1, true);
    if (u.Clicked(passField)) focusedField_ = 1;
    y = passField.GetBottom() + 12.0f;

    // Error.
    if (!loginError_.empty()) {
        Gdiplus::RectF errBox(x, y, w, 16.0f);
        u.TextCenter(errBox, loginError_.c_str(), 11.5f, pal.danger);
        y += 22.0f;
    }

    // Submit.
    Gdiplus::RectF btn(x, y, w, 42.0f);
    if (u.Button(btn, L"Sign in", pal.accent, Gdiplus::Color(255, 255, 255, 255), 13.5f, true))
        (void)TryLogin();

    // Demo hint.
    Gdiplus::RectF hint(x, btn.GetBottom() + 14.0f, w, 16.0f);
    u.TextCenter(hint, L"Demo credentials — user: 2   •   password: 2", 10.5f, pal.textFaint);
}
