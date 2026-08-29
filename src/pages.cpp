//
// pages.cpp — dashboard: sidebar, top bar and the six option pages.
//
#include "app.h"

#include <cwchar>

namespace {

const wchar_t* kTabs[] = {
    L"Overview", L"General", L"Interface", L"Network", L"Security", L"About",
};
const int kTabCount = 6;

// Small 16x16 line icons drawn with GDI+ primitives.
void DrawIcon(Gdiplus::Graphics& g, int type, Gdiplus::RectF box, Gdiplus::Color c) {
    Gdiplus::Pen pen(c, 1.6f);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    Gdiplus::SolidBrush fill(c);
    float x = box.X, y = box.Y;
    switch (type) {
    case 0: // overview — 2x2 grid
        theme::FillRounded(g, Gdiplus::RectF(x, y, 6.5f, 6.5f), 2.0f, c);
        theme::FillRounded(g, Gdiplus::RectF(x + 8.0f, y, 6.5f, 6.5f), 2.0f, c);
        theme::FillRounded(g, Gdiplus::RectF(x, y + 8.0f, 6.5f, 6.5f), 2.0f, c);
        theme::FillRounded(g, Gdiplus::RectF(x + 8.0f, y + 8.0f, 6.5f, 6.5f), 2.0f, c);
        break;
    case 1: // general — slider lines
        g.DrawLine(&pen, x, y + 3.0f, x + 16.0f, y + 3.0f);
        g.DrawLine(&pen, x, y + 8.0f, x + 16.0f, y + 8.0f);
        g.DrawLine(&pen, x, y + 13.0f, x + 16.0f, y + 13.0f);
        g.FillEllipse(&fill, x + 9.5f, y + 1.0f, 4.0f, 4.0f);
        g.FillEllipse(&fill, x + 3.5f, y + 6.0f, 4.0f, 4.0f);
        g.FillEllipse(&fill, x + 11.5f, y + 11.0f, 4.0f, 4.0f);
        break;
    case 2: // interface — window
        theme::StrokeRounded(g, Gdiplus::RectF(x, y + 1.5f, 15.0f, 13.0f), 3.0f, c, 1.5f);
        g.DrawLine(&pen, x + 1.5f, y + 5.5f, x + 13.5f, y + 5.5f);
        break;
    case 3: // network — target
        g.DrawEllipse(&pen, x + 1.0f, y + 1.0f, 14.0f, 14.0f);
        g.FillEllipse(&fill, x + 6.0f, y + 6.0f, 4.0f, 4.0f);
        break;
    case 4: // security — lock
        g.DrawArc(&pen, x + 3.5f, y + 1.0f, 9.0f, 9.0f, 180.0f, 180.0f);
        theme::FillRounded(g, Gdiplus::RectF(x + 2.0f, y + 5.5f, 12.0f, 9.5f), 2.5f, c);
        break;
    case 5: // about — info
        g.DrawEllipse(&pen, x + 0.5f, y + 0.5f, 15.0f, 15.0f);
        g.FillEllipse(&fill, x + 6.75f, y + 4.0f, 2.5f, 2.5f);
        g.DrawLine(&pen, x + 8.0f, y + 8.0f, x + 8.0f, y + 12.0f);
        break;
    default:
        break;
    }
}

void FormatUptime(ULONGLONG ms, wchar_t* out, int n) {
    int s = (int)(ms / 1000);
    swprintf(out, n, L"%02d:%02d:%02d", s / 3600, (s / 60) % 60, s % 60);
}

std::wstring ToWide(const char* s) {
    std::wstring out;
    for (const char* p = s; *p; ++p) out += (wchar_t)(unsigned char)*p;
    return out;
}

// Label on the left, control reserved on the right (caller draws it).
void ToggleRow(ui::Ui& u, float x, float y, float w, float rowH,
               const wchar_t* label, const wchar_t* sub, bool* value) {
    u.OptionLabel(Gdiplus::RectF(x, y, w, rowH), label, sub);
    u.ToggleSwitch(x + w - 44.0f, y + rowH / 2.0f - 12.0f, value);
}

// Label on the left, colored value right-aligned.
void ValueRow(ui::Ui& u, float x, float y, float w, float rowH,
              const wchar_t* label, const wchar_t* value, Gdiplus::Color valueColor,
              bool dot = false) {
    u.OptionLabel(Gdiplus::RectF(x, y, w - 140.0f, rowH), label, nullptr);
    Gdiplus::RectF vm = u.Measure(value, 12.5f, Gdiplus::FontStyleBold);
    float vx = x + w - vm.Width;
    if (dot) {
        Gdiplus::SolidBrush b(valueColor);
        u.G().FillEllipse(&b, vx - 14.0f, y + rowH / 2.0f - 3.5f, 7.0f, 7.0f);
    }
    u.Text(vx, y + rowH / 2.0f - 9.0f, value, 12.5f, valueColor, Gdiplus::FontStyleBold);
}

} // namespace

void App::RenderDashboard(ui::Ui& u) {
    const Gdiplus::RectF client = u.Client();
    const theme::Palette& pal = u.Pal();
    const float rowH = settings_.compactMode ? 48.0f : 58.0f;

    // ----- sidebar -------------------------------------------------------
    Gdiplus::RectF side(0.0f, theme::kTitleBarH, theme::kSidebarW, client.Height - theme::kTitleBarH);
    Gdiplus::SolidBrush sideBg(pal.sidebar);
    u.G().FillRectangle(&sideBg, side);
    Gdiplus::Pen sidePen(pal.border, 1.0f);
    u.G().DrawLine(&sidePen, theme::kSidebarW - 0.5f, side.Y, theme::kSidebarW - 0.5f, side.GetBottom());

    // Sidebar brand.
    Gdiplus::RectF logo(18.0f, side.Y + 18.0f, 34.0f, 34.0f);
    theme::FillRounded(u.G(), logo, 10.0f, pal.accent);
    u.TextCenter(logo, L"A", 17.0f, Gdiplus::Color(255, 255, 255, 255), Gdiplus::FontStyleBold);
    Gdiplus::RectF nameBox(logo.GetRight() + 10.0f, side.Y + 16.0f, 120.0f, 20.0f);
    u.TextCenterV(nameBox, account::kAppTitle, 14.0f, pal.text, Gdiplus::FontStyleBold);
    Gdiplus::RectF nameSub(logo.GetRight() + 10.0f, side.Y + 34.0f, 120.0f, 14.0f);
    u.TextCenterV(nameSub, L"Control Panel", 9.5f, pal.textFaint);

    // Navigation.
    float navY = side.Y + 76.0f;
    Gdiplus::RectF navRect(12.0f, navY, theme::kSidebarW - 24.0f, 42.0f);
    for (int i = 0; i < kTabCount; i++) {
        Gdiplus::RectF item(navRect.X, navY + i * 48.0f, navRect.Width, 42.0f);
        bool active = (activeTab_ == i);
        bool hover = u.Hovered(item);

        if (active) {
            theme::FillRounded(u.G(), item, 10.0f, pal.accentSoft);
            Gdiplus::RectF bar(8.0f, item.Y + 9.0f, 3.5f, item.Height - 18.0f);
            theme::FillRounded(u.G(), bar, 2.0f, pal.accent);
        } else if (hover) {
            theme::FillRounded(u.G(), item, 10.0f, pal.card);
        }

        Gdiplus::Color iconC = active ? pal.accent : pal.textDim;
        DrawIcon(u.G(), i, Gdiplus::RectF(item.X + 14.0f, item.Y + item.Height / 2.0f - 8.0f, 16.0f, 16.0f), iconC);

        Gdiplus::RectF labelBox(item.X + 42.0f, item.Y, item.Width - 50.0f, item.Height);
        u.TextCenterV(labelBox, kTabs[i], 13.0f,
                      active ? pal.accent : (hover ? pal.text : pal.textDim),
                      active ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular);

        if (u.Clicked(item)) {
            activeTab_ = i;
            openCombo_ = -1;
            focusedField_ = -1;
        }
    }

    // Sidebar footer.
    u.Text(side.X + 18.0f, side.GetBottom() - 42.0f,
           (std::wstring(L"v") + account::kVersion).c_str(), 10.5f, pal.textFaint);
    u.Text(side.X + 18.0f, side.GetBottom() - 26.0f,
           (std::wstring(L"by ") + account::kDeveloper).c_str(), 10.5f, pal.textFaint);

    // ----- top bar ---------------------------------------------------------
    float contentX = theme::kSidebarW + 24.0f;
    float contentW = client.Width - contentX - 24.0f;
    float topBarY = theme::kTitleBarH;

    Gdiplus::RectF titleBox(contentX, topBarY + 8.0f, 320.0f, 26.0f);
    u.TextCenterV(titleBox, kTabs[activeTab_], 17.0f, pal.text, Gdiplus::FontStyleBold);

    Gdiplus::RectF logoutBtn(client.Width - 24.0f - 92.0f, topBarY + 12.0f, 92.0f, 34.0f);
    if (u.Button(logoutBtn, L"Log out", pal.card, pal.textDim, 12.0f)) Logout();

    Gdiplus::RectF avatar(logoutBtn.X - 46.0f, topBarY + 13.0f, 30.0f, 30.0f);
    Gdiplus::SolidBrush av(pal.accentSoft);
    u.G().FillEllipse(&av, avatar.X, avatar.Y, avatar.Width, avatar.Height);
    Gdiplus::Pen avPen(pal.accent, 1.2f);
    u.G().DrawEllipse(&avPen, avatar.X, avatar.Y, avatar.Width, avatar.Height);
    std::wstring who = Trim(loginUser_);
    std::wstring initial = who.empty() ? L"2" : who.substr(0, 1);
    u.TextCenter(avatar, initial.c_str(), 12.5f, pal.accent, Gdiplus::FontStyleBold);
    Gdiplus::RectF userLabel(avatar.X - 84.0f, avatar.Y, 72.0f, avatar.Height);
    u.TextCenterV(userLabel, (std::wstring(L"User ") + Trim(loginUser_)).c_str(), 12.0f, pal.text);

    // ----- content -----------------------------------------------------------
    float contentY = topBarY + 56.0f;
    float footerH = 30.0f;
    float contentH = client.Height - contentY - footerH - 14.0f;
    float colW = (contentW - 16.0f) / 2.0f;

    switch (activeTab_) {
    case 0:  RenderOverview(u, contentX, contentY, contentW, colW, contentH, rowH); break;
    case 1:  RenderGeneral(u, contentX, contentY, contentW, colW, contentH, rowH); break;
    case 2:  RenderInterface(u, contentX, contentY, contentW, colW, contentH, rowH); break;
    case 3:  RenderNetwork(u, contentX, contentY, contentW, colW, contentH, rowH); break;
    case 4:  RenderSecurity(u, contentX, contentY, contentW, colW, contentH, rowH); break;
    default: RenderAbout(u, contentX, contentY, contentW, contentH); break;
    }

    RenderFooter(u, contentX, client.Height - footerH - 8.0f, contentW);
}

void App::RenderFooter(ui::Ui& u, float x, float y, float w) {
    Gdiplus::RectF r(x, y, w, 18.0f);
    u.TextCenter(r, (std::wstring(account::kAppTitle) + L" v" + account::kVersion +
                     L"  •  developed by " + account::kDeveloper +
                     L"  •  for educational purposes only").c_str(),
                 10.5f, u.Pal().textFaint);
}

// ----- Overview ------------------------------------------------------------

void App::RenderOverview(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH) {
    const theme::Palette& pal = u.Pal();
    (void)colW;

    // Welcome card.
    Gdiplus::RectF welcome(x, y, w, 108.0f);
    u.Card(welcome, L"Welcome back", L"all systems normal");
    u.Text(welcome.X + 18.0f, welcome.Y + 52.0f,
           (std::wstring(L"Signed in as ") + Trim(loginUser_) + L". You are fully up to date.").c_str(),
           12.5f, pal.textDim);

    Gdiplus::RectF pill(welcome.GetRight() - 96.0f, welcome.Y + 18.0f, 78.0f, 24.0f);
    theme::FillRounded(u.G(), pill, 12.0f,
                       Gdiplus::Color(40, pal.success.GetR(), pal.success.GetG(), pal.success.GetB()));
    Gdiplus::SolidBrush dot(pal.success);
    u.G().FillEllipse(&dot, pill.X + 10.0f, pill.Y + 8.5f, 7.0f, 7.0f);
    u.Text(pill.X + 24.0f, pill.Y + 5.5f, L"Online", 11.0f, pal.success);

    // Stat row.
    float row2 = y + 108.0f + 16.0f;
    float statW = (w - 2 * 16.0f) / 3.0f;

    wchar_t up[16] = L"00:00:00";
    FormatUptime(GetTickCount64() - loginTick_, up, 16);
    Gdiplus::RectF s1(x, row2, statW, 92.0f);
    u.Card(s1, L"Uptime");
    u.Text(s1.X + 18.0f, s1.Y + 42.0f, up, 18.0f, pal.text, Gdiplus::FontStyleBold);

    Gdiplus::RectF s2(x + statW + 16.0f, row2, statW, 92.0f);
    u.Card(s2, L"Version");
    u.Text(s2.X + 18.0f, s2.Y + 42.0f, account::kVersion, 18.0f, pal.text, Gdiplus::FontStyleBold);

    Gdiplus::RectF s3(x + 2 * (statW + 16.0f), row2, statW, 92.0f);
    u.Card(s3, L"Accent");
    Gdiplus::RectF swatch(s3.X + 18.0f, s3.Y + 49.0f, 14.0f, 14.0f);
    theme::FillRounded(u.G(), swatch, 4.0f, pal.accent);
    u.Text(s3.X + 42.0f, s3.Y + 44.0f, theme::kAccents[settings_.accent].name, 15.0f,
           pal.text, Gdiplus::FontStyleBold);

    // Bottom row.
    float row3 = row2 + 92.0f + 16.0f;
    float h3 = y + h - row3;
    if (h3 < 120.0f) h3 = 120.0f;

    Gdiplus::RectF left(x, row3, colW, h3);
    u.Card(left, L"Quick toggles", L"mirrors the General tab");
    float ry = left.Y + 52.0f;
    float rw = left.Width - 36.0f;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Notifications", L"Important-event toasts", &settings_.notifications);
    ry += rowH;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Auto-update", L"Check automatically", &settings_.autoUpdate);
    ry += rowH;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Two-factor auth", L"Extra sign-in step", &settings_.twoFactor);

    ULONGLONG tick = GetTickCount64();
    Gdiplus::RectF right(x + colW + 16.0f, row3, colW, h3);
    u.Card(right, L"System status", L"live • simulated for demo");
    ry = right.Y + 52.0f;
    int lat = 9 + (int)((tick / 900) % 17);
    wchar_t latBuf[24];
    swprintf(latBuf, 24, L"%d ms", lat);
    ValueRow(u, right.X + 18.0f, ry, rw, rowH, L"Latency", latBuf, pal.accent);
    ry += rowH;
    ValueRow(u, right.X + 18.0f, ry, rw, rowH, L"State", L"Connected", pal.success, true);
    ry += rowH;
    ValueRow(u, right.X + 18.0f, ry, rw, rowH, L"Mode", settings_.p2pMode ? L"Peer-to-peer" : L"Classic", pal.text);
}

// ----- General ---------------------------------------------------------------

void App::RenderGeneral(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH) {
    const theme::Palette& pal = u.Pal();
    (void)w;

    Gdiplus::RectF left(x, y, colW, h);
    u.Card(left, L"Behavior", L"daily drivers");
    float ry = left.Y + 52.0f;
    float rw = left.Width - 36.0f;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Notifications", L"Show a toast for important events", &settings_.notifications);
    ry += rowH;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Start on boot", L"Launch with Windows", &settings_.startOnBoot);
    ry += rowH;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Auto-update", L"Check for updates automatically", &settings_.autoUpdate);
    ry += rowH;
    u.Divider(left.X + 18.0f, left.X + left.Width - 18.0f, ry + 6.0f, pal.border);
    u.Text(left.X + 18.0f, ry + 18.0f, L"Changes apply instantly and are kept for this session.",
           11.0f, pal.textFaint);

    Gdiplus::RectF right(x + colW + 16.0f, y, colW, h);
    u.Card(right, L"Performance", L"rendering & audio");
    ry = right.Y + 52.0f;
    u.OptionLabel(Gdiplus::RectF(right.X + 18.0f, ry, rw - 160.0f, rowH), L"Quality preset", L"Low, medium, high or ultra");
    Gdiplus::RectF combo(right.GetRight() - 18.0f - 150.0f, ry + rowH / 2.0f - 19.0f, 150.0f, 38.0f);
    u.Combo(combo, L"Quality", kQualityLevels, kQualityCount, &settings_.quality, &openCombo_, 1);
    ry += rowH;
    u.SliderRow(Gdiplus::RectF(right.X + 18.0f, ry, rw, rowH + 8.0f), L"Master volume", 0, 100, &settings_.volume, L"%");
}

// ----- Interface ----------------------------------------------------------------

void App::RenderInterface(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH) {
    const theme::Palette& pal = u.Pal();
    (void)w;

    Gdiplus::RectF left(x, y, colW, h);
    u.Card(left, L"Theme", L"live preview");
    float ry = left.Y + 52.0f;
    float rw = left.Width - 36.0f;
    u.OptionLabel(Gdiplus::RectF(left.X + 18.0f, ry, rw, rowH), L"Accent color", L"Recolors the whole UI instantly");
    u.Swatches(left.X + 18.0f, ry + rowH - 14.0f, &settings_.accent);
    ry += rowH + 34.0f;
    u.Divider(left.X + 18.0f, left.X + left.Width - 18.0f, ry, pal.border);
    ry += 14.0f;
    u.SliderRow(Gdiplus::RectF(left.X + 18.0f, ry, rw, rowH + 8.0f), L"Font scale", 80, 140, &settings_.fontScale, L"%");

    Gdiplus::RectF right(x + colW + 16.0f, y, colW, h);
    u.Card(right, L"Display", L"motion & density");
    ry = right.Y + 52.0f;
    rw = right.Width - 36.0f;
    ToggleRow(u, right.X + 18.0f, ry, rw, rowH, L"Smooth animations", L"Eased toggles and transitions", &settings_.animations);
    ry += rowH;
    ToggleRow(u, right.X + 18.0f, ry, rw, rowH, L"Compact mode", L"Tighter option rows (try it)", &settings_.compactMode);
    ry += rowH + 8.0f;
    u.Divider(right.X + 18.0f, right.X + right.Width - 18.0f, ry, pal.border);
    u.Text(right.X + 18.0f, ry + 14.0f, L"Tip: move the font scale slider to see the UI rescale live.",
           11.0f, pal.textFaint);
}

// ----- Network --------------------------------------------------------------------

void App::RenderNetwork(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH) {
    (void)w;

    Gdiplus::RectF left(x, y, colW, h);
    u.Card(left, L"Connection", L"transport settings");
    float ry = left.Y + 52.0f;
    float rw = left.Width - 36.0f;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Peer-to-peer mode", L"Route traffic directly between peers", &settings_.p2pMode);
    ry += rowH;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Show latency", L"Display ping in the status panel", &settings_.showLatency);
    ry += rowH;
    u.SliderRow(Gdiplus::RectF(left.X + 18.0f, ry, rw, rowH + 8.0f), L"Max peers", 1, 16, &settings_.maxPeers, nullptr);

    ULONGLONG tick = GetTickCount64();
    Gdiplus::RectF right(x + colW + 16.0f, y, colW, h);
    u.Card(right, L"Status", L"live • simulated for demo");
    ry = right.Y + 52.0f;
    int lat = 9 + (int)((tick / 900) % 17);
    wchar_t latBuf[24];
    swprintf(latBuf, 24, L"%d ms", lat);
    ValueRow(u, right.X + 18.0f, ry, rw, rowH, L"State", L"Connected", u.Pal().success, true);
    ry += rowH;
    ValueRow(u, right.X + 18.0f, ry, rw, rowH, L"Latency", settings_.showLatency ? latBuf : L"hidden", u.Pal().accent);
    ry += rowH;
    ValueRow(u, right.X + 18.0f, ry, rw, rowH, L"Mode", settings_.p2pMode ? L"Peer-to-peer" : L"Classic", u.Pal().text);
    ry += rowH;
    wchar_t addr[32];
    swprintf(addr, 32, L"192.168.1.%d", 10 + (int)((tick / 4000) % 180));
    ValueRow(u, right.X + 18.0f, ry, rw, rowH, L"Address", addr, u.Pal().textDim);
}

// ----- Security -----------------------------------------------------------------------

void App::RenderSecurity(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH) {
    const theme::Palette& pal = u.Pal();
    (void)w;

    Gdiplus::RectF left(x, y, colW, h);
    u.Card(left, L"Protection", L"account safety");
    float ry = left.Y + 52.0f;
    float rw = left.Width - 36.0f;
    ToggleRow(u, left.X + 18.0f, ry, rw, rowH, L"Two-factor authentication", L"Require a second factor at sign-in", &settings_.twoFactor);
    ry += rowH;
    u.SliderRow(Gdiplus::RectF(left.X + 18.0f, ry, rw, rowH + 8.0f), L"Session timeout", 5, 120, &settings_.sessionTimeout, L"min");

    Gdiplus::RectF right(x + colW + 16.0f, y, colW, h);
    u.Card(right, L"Change password", L"demo — session only");
    ry = right.Y + 52.0f;
    rw = right.Width - 36.0f;

    u.Text(right.X + 18.0f, ry, L"CURRENT PASSWORD", 10.5f, pal.textDim, Gdiplus::FontStyleBold);
    ry += 18.0f;
    Gdiplus::RectF f1(right.X + 18.0f, ry, rw, 38.0f);
    u.TextField(f1, passCurrent_, L"Current password", focusedField_ == 10, true);
    if (u.Clicked(f1)) focusedField_ = 10;
    ry = f1.GetBottom() + 14.0f;

    u.Text(right.X + 18.0f, ry, L"NEW PASSWORD", 10.5f, pal.textDim, Gdiplus::FontStyleBold);
    ry += 18.0f;
    Gdiplus::RectF f2(right.X + 18.0f, ry, rw, 38.0f);
    u.TextField(f2, passNew_, L"New password", focusedField_ == 11, true);
    if (u.Clicked(f2)) focusedField_ = 11;
    ry = f2.GetBottom() + 16.0f;

    Gdiplus::RectF btn(right.X + 18.0f, ry, 150.0f, 38.0f);
    if (u.Button(btn, L"Update password", pal.accent, Gdiplus::Color(255, 255, 255, 255), 12.5f, true)) {
        if (Trim(passCurrent_) != settings_.password) {
            statusMsg_ = L"Current password is incorrect.";
            statusOk_ = false;
        } else if (Trim(passNew_).empty()) {
            statusMsg_ = L"Enter a new password.";
            statusOk_ = false;
        } else {
            settings_.password = Trim(passNew_);
            passCurrent_.clear();
            passNew_.clear();
            statusMsg_ = L"Password updated for this session.";
            statusOk_ = true;
        }
    }
    ry = btn.GetBottom() + 12.0f;
    if (!statusMsg_.empty())
        u.Text(right.X + 18.0f, ry, statusMsg_.c_str(), 11.5f, statusOk_ ? pal.success : pal.danger);
    u.Text(right.X + 18.0f, ry + 20.0f, L"Note: stored in memory only — nothing is written to disk.",
           10.5f, pal.textFaint);
}

// ----- About -----------------------------------------------------------------------------

void App::RenderAbout(ui::Ui& u, float x, float y, float w, float h) {
    const theme::Palette& pal = u.Pal();

    const float cardW = 430.0f;
    const float cardH = 340.0f;
    Gdiplus::RectF card(x + (w - cardW) / 2.0f, y + (h - cardH) / 2.0f, cardW, cardH);
    u.Card(card);

    Gdiplus::RectF logo(card.X + cardW / 2.0f - 28.0f, card.Y + 30.0f, 56.0f, 56.0f);
    theme::FillRounded(u.G(), logo, 15.0f, pal.accent);
    u.TextCenter(logo, L"A", 26.0f, Gdiplus::Color(255, 255, 255, 255), Gdiplus::FontStyleBold);

    Gdiplus::RectF nameBox(card.X, logo.GetBottom() + 16.0f, cardW, 28.0f);
    u.TextCenter(nameBox, account::kAppTitle, 20.0f, pal.text, Gdiplus::FontStyleBold);
    Gdiplus::RectF descBox(card.X, nameBox.GetBottom() + 2.0f, cardW, 18.0f);
    u.TextCenter(descBox, L"Clean C++ control-panel UI — GDI+ edition", 11.5f, pal.textDim);

    float ry = descBox.GetBottom() + 16.0f;
    u.Divider(card.X + 36.0f, card.GetRight() - 36.0f, ry, pal.border);
    ry += 14.0f;

    struct InfoRow { const wchar_t* label; std::wstring value; };
    InfoRow rows[] = {
        { L"Version",   std::wstring(account::kVersion) },
        { L"Developer", std::wstring(account::kDeveloper) },
        { L"Build",     ToWide(__DATE__) },
        { L"License",   L"MIT" },
    };
    const int rowCount = (int)(sizeof(rows) / sizeof(rows[0]));
    for (int i = 0; i < rowCount; i++) {
        u.Text(card.X + 36.0f, ry + 2.0f, rows[i].label, 12.0f, pal.textDim);
        Gdiplus::RectF vm = u.Measure(rows[i].value.c_str(), 12.0f, Gdiplus::FontStyleBold);
        u.Text(card.GetRight() - 36.0f - vm.Width, ry + 2.0f, rows[i].value.c_str(),
               12.0f, pal.text, Gdiplus::FontStyleBold);
        ry += 28.0f;
    }

    Gdiplus::RectF noteBox(card.X, card.GetBottom() - 34.0f, cardW, 16.0f);
    u.TextCenter(noteBox, L"Educational UI demo — no game integration of any kind.", 10.5f, pal.textFaint);
}
