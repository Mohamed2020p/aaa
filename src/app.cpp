//
// app.cpp — input routing, frame pipeline, login/logout logic.
//
#include "app.h"

App::App() = default;

void App::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEMOVE:
        input_.mouseX = (float)GET_X_LPARAM(lParam);
        input_.mouseY = (float)GET_Y_LPARAM(lParam);
        break;
    case WM_LBUTTONDOWN:
        input_.mouseX = (float)GET_X_LPARAM(lParam);
        input_.mouseY = (float)GET_Y_LPARAM(lParam);
        input_.mouseDown = true;
        input_.mousePressed = true;
        break;
    case WM_LBUTTONUP:
        input_.mouseDown = false;
        input_.mouseReleased = true;
        break;
    case WM_CHAR:
        if (wParam >= 32 && input_.chars.size() < 64)
            input_.chars.push_back((wchar_t)wParam);
        break;
    case WM_KEYDOWN:
        input_.lastKey = (int)wParam;
        break;
    default:
        break;
    }
}

void App::RenderFrame(Gdiplus::Graphics& g, int width, int height) {
    input_.BeginFrame();

    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    Gdiplus::RectF client(0.0f, 0.0f, (float)width, (float)height);
    theme::Palette pal = theme::MakePalette(settings_.accent);

    Gdiplus::SolidBrush bg(pal.bg);
    g.FillRectangle(&bg, client);

    ui::Ui u(g, client, input_, pal, settings_.fontScale / 100.0f);

    RenderTitleBar(u);
    if (page_ == Page::Login) RenderLogin(u);
    else RenderDashboard(u);

    // Close any open combo when the click lands outside it.
    if (openCombo_ != -1 && input_.mousePressed && !input_.comboClick) openCombo_ = -1;
    // Unfocus any text field when the click lands outside it.
    if (focusedField_ != -1 && input_.mousePressed && !input_.fieldClick) focusedField_ = -1;

    ConsumeTypedInput();

    // Enter submits the login form.
    if (input_.lastKey == VK_RETURN && page_ == Page::Login) (void)TryLogin();
}

void App::ConsumeTypedInput() {
    bool typed = !input_.chars.empty();
    bool back  = input_.lastKey == VK_BACK;
    if (!typed && !back) return;

    std::wstring* target = nullptr;
    if (page_ == Page::Login) {
        if (focusedField_ == 0) target = &loginUser_;
        else if (focusedField_ == 1) target = &loginPass_;
    } else if (activeTab_ == 4) { // Security tab
        if (focusedField_ == 10) target = &passCurrent_;
        else if (focusedField_ == 11) target = &passNew_;
    }
    if (!target) return;

    for (wchar_t c : input_.chars)
        if (target->size() < 24) target->push_back(c);
    if (back && !target->empty()) target->pop_back();
}

bool App::TryLogin() {
    std::wstring u = Trim(loginUser_);
    std::wstring p = Trim(loginPass_);
    if (u == account::kDefaultUser && p == account::kDefaultPassword) {
        loginError_.clear();
        focusedField_ = -1;
        openCombo_ = -1;
        loginTick_ = GetTickCount64();
        page_ = Page::Dashboard;
        activeTab_ = 0;
        statusMsg_.clear();
        statusOk_ = false;
        return true;
    }
    loginError_ = L"Invalid username or password.";
    return false;
}

void App::Logout() {
    page_ = Page::Login;
    activeTab_ = 0;
    focusedField_ = -1;
    openCombo_ = -1;
    loginPass_.clear();
    loginError_.clear();
    passCurrent_.clear();
    passNew_.clear();
    statusMsg_.clear();
    statusOk_ = false;
}

// ----- title bar (shared by both pages) ----------------------------------

void App::RenderTitleBar(ui::Ui& u) {
    const Gdiplus::RectF client = u.Client();
    const theme::Palette& pal = u.Pal();

    Gdiplus::RectF bar(0.0f, 0.0f, client.Width, theme::kTitleBarH);
    Gdiplus::SolidBrush bg(pal.sidebar);
    u.G().FillRectangle(&bg, bar);

    // Brand mark + title.
    Gdiplus::RectF mark(16.0f, bar.Y + 15.0f, 14.0f, 14.0f);
    theme::FillRounded(u.G(), mark, 4.0f, pal.accent);

    Gdiplus::RectF titleBox(mark.GetRight() + 10.0f, bar.Y, 200.0f, bar.Height);
    u.TextCenterV(titleBox, account::kAppTitle, 12.5f, pal.text, Gdiplus::FontStyleBold);
    Gdiplus::RectF tm = u.Measure(account::kAppTitle, 12.5f, Gdiplus::FontStyleBold);
    Gdiplus::RectF tagBox(mark.GetRight() + 22.0f + tm.Width, bar.Y, 220.0f, bar.Height);
    u.TextCenterV(tagBox, L"• Control Panel", 11.5f, pal.textFaint);

    // Close button — its hit area matches the WM_NCHITTEST region in main.cpp.
    Gdiplus::RectF closeBtn(client.Width - 46.0f, 0.0f, 46.0f, bar.Height);
    bool hover = u.Hovered(closeBtn);
    if (hover)
        theme::FillRounded(u.G(),
                           Gdiplus::RectF(closeBtn.X + 5.0f, closeBtn.Y + 8.0f,
                                          closeBtn.Width - 10.0f, bar.Height - 16.0f),
                           9.0f, Gdiplus::Color(255, 239, 68, 68));

    Gdiplus::Pen xpen(hover ? Gdiplus::Color(255, 255, 255, 255) : pal.textDim, 1.6f);
    xpen.SetStartCap(Gdiplus::LineCapRound);
    xpen.SetEndCap(Gdiplus::LineCapRound);
    float cx = closeBtn.X + closeBtn.Width / 2.0f;
    float cy = bar.Height / 2.0f;
    u.G().DrawLine(&xpen, cx - 5.0f, cy - 5.0f, cx + 5.0f, cy + 5.0f);
    u.G().DrawLine(&xpen, cx + 5.0f, cy - 5.0f, cx - 5.0f, cy + 5.0f);

    if (u.Clicked(closeBtn) && onClose) onClose();
}
