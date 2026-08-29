//
// main.cpp — Win32 entry point: window class, message loop, double-buffered paint.
//
#include <windows.h>
#include <gdiplus.h>

#include "app.h"
#include "theme.h"

static App g_app;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // Defer destruction to a later message: the close button is clicked
        // from inside WM_PAINT, so DestroyWindow must not run mid-paint.
        g_app.onClose = [hwnd]() { PostMessage(hwnd, WM_CLOSE, 0, 0); };
        SetTimer(hwnd, 1, 120, nullptr); // caret blink + live values
        return 0;

    case WM_TIMER:
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_ERASEBKGND:
        return 1; // we paint everything ourselves (double buffered)

    case WM_NCHITTEST: {
        // Make the title bar draggable, except over the close button.
        POINT pt = { (SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam) };
        ScreenToClient(hwnd, &pt);
        RECT cr;
        GetClientRect(hwnd, &cr);
        Gdiplus::RectF closeBtn((cr.right - cr.left) - 46.0f, 0.0f, 46.0f, theme::kTitleBarH);
        if (pt.y < theme::kTitleBarH) {
            if (closeBtn.Contains((float)pt.x, (float)pt.y)) return HTCLIENT;
            return HTCAPTION;
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT cr;
        GetClientRect(hwnd, &cr);
        int w = cr.right - cr.left, h = cr.bottom - cr.top;

        // Double buffer: draw to a memory DC, then blit once (no flicker).
        HDC mem = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
        HGDIOBJ old = SelectObject(mem, bmp);
        {
            Gdiplus::Graphics g(mem);
            g_app.RenderFrame(g, w, h);
        }
        BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;

    case WM_MOUSEMOVE:
        // Redraw on every move so slider/toggle dragging stays smooth.
        g_app.HandleMessage(msg, wParam, lParam);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    default:
        if ((msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) ||
            msg == WM_CHAR || msg == WM_KEYDOWN) {
            g_app.HandleMessage(msg, wParam, lParam);
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
    Gdiplus::GdiplusStartupInput gdiplusInput;
    ULONG_PTR gdiplusToken = 0;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, nullptr);

    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"AlienXMenuClass";
    RegisterClassEx(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    HWND hwnd = CreateWindowEx(
        0,
        L"AlienXMenuClass",
        (std::wstring(account::kAppTitle) + L" - by " + account::kDeveloper).c_str(),
        WS_POPUP,
        (screenW - theme::kWindowWidth) / 2,
        (screenH - theme::kWindowHeight) / 2,
        theme::kWindowWidth,
        theme::kWindowHeight,
        nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 1;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}
