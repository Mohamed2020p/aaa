#pragma once
//
// app.h — application state + per-frame pipeline.
//
#include <windows.h>
#include <gdiplus.h>
#include <functional>
#include <string>

#include "settings.h"
#include "theme.h"
#include "ui.h"

class App {
public:
    App();

    // Set by the window; called when the close button is clicked.
    std::function<void()> onClose;

    void HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void RenderFrame(Gdiplus::Graphics& g, int width, int height);

private:
    enum class Page { Login, Dashboard };

    // Frame sections.
    void RenderTitleBar(ui::Ui& u);
    void RenderLogin(ui::Ui& u);
    void RenderDashboard(ui::Ui& u);
    void RenderFooter(ui::Ui& u, float x, float y, float w);

    // Dashboard tabs.
    void RenderOverview(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH);
    void RenderGeneral(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH);
    void RenderInterface(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH);
    void RenderNetwork(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH);
    void RenderSecurity(ui::Ui& u, float x, float y, float w, float colW, float h, float rowH);
    void RenderAbout(ui::Ui& u, float x, float y, float w, float h);

    void ConsumeTypedInput();
    bool TryLogin();
    void Logout();

    Page   page_ = Page::Login;
    int    activeTab_ = 0;
    int    focusedField_ = -1;   // 0/1 = login fields, 10/11 = password fields
    int    openCombo_ = -1;      // id of the open combo (-1 = none)
    bool   statusOk_ = false;

    std::wstring loginUser_;
    std::wstring loginPass_;
    std::wstring loginError_;
    std::wstring statusMsg_;
    std::wstring passCurrent_;
    std::wstring passNew_;

    Settings  settings_;
    ui::Input input_;
    ULONGLONG loginTick_ = 0;
};
