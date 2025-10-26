#include "StdAfx.h"
#include "WndShadowEx.h"

#include <algorithm>   // 已加
#include <vector>      // 新增：stackBlur 用到了 std::vector
#include <cmath>       // 新增：sqrtf / powf

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif


#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "msimg32.lib")

namespace DuiLib {

    WndShadowEx::WndShadowEx() = default;
    WndShadowEx::~WndShadowEx() { Detach(); }

    void WndShadowEx::Attach(HWND hHost, const ShadowOptions& opt) {
        if (!::IsWindow(hHost)) return;
        host_ = hHost;
        opt_ = opt;
        attached_ = true;
        ensureDpi_();
        createShadowWindow_();
        Update();
        Show(true);
    }

    void WndShadowEx::Detach() {
        if (!attached_) return;
        destroyShadowWindow_();
        host_ = nullptr;
        attached_ = false;
    }

    void WndShadowEx::Update() {
        if (!attached_) return;
        ensureDpi_();
        reposition_();
        repaint_();
    }

    void WndShadowEx::Show(bool vis) {
        visible_ = vis;
        if (!shadow_) return;
        ::ShowWindow(shadow_, vis ? SW_SHOWNOACTIVATE : SW_HIDE);
    }

    void WndShadowEx::createShadowWindow_() {
        if (shadow_) return;

        // 半透明顶层、无激活、不进 Alt-Tab
        DWORD ex = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
        DWORD st = WS_POPUP;

        // 无父创建，随后设置 owner（避免某些主题下 owner-clip 导致外圈被裁）
        shadow_ = ::CreateWindowExW(ex, L"STATIC", L"", st,
            0, 0, 0, 0, nullptr, nullptr,
            (HINSTANCE)::GetWindowLongPtr(host_, GWLP_HINSTANCE), nullptr);
        if (!shadow_) return;

        ::SetWindowLongPtr(shadow_, GWLP_HWNDPARENT, (LONG_PTR)host_);
    }

    void WndShadowEx::destroyShadowWindow_() {
        if (shadow_)
        {
            ::DestroyWindow(shadow_);
            shadow_ = nullptr;
        }
    }

    void WndShadowEx::reposition_() {
        if (!shadow_ || !host_) return;

        RECT rc{}; ::GetWindowRect(host_, &rc);

        WINDOWPLACEMENT wp{ sizeof(wp) };
        if (::GetWindowPlacement(host_, &wp) && wp.showCmd == SW_SHOWMAXIMIZED)
        {
            Show(false); return;
        }
        else
        {
            Show(true);
        }

        // 关键：计算 blur 的 DPI 值，并给出“最小需要的厚度”
        const int blur = scale_(opt_.shadowBlurRadius);
        const int need = blur * 2 + 4;                         // 容纳光晕的最小厚度
        const int thick = std::max(scale_(opt_.shadowThickness), need);

        const int offx = scale_(opt_.shadowOffsetX);
        const int offy = scale_(opt_.shadowOffsetY);

        const int x = rc.left - thick + offx;
        const int y = rc.top - thick + offy;
        const int w = (rc.right - rc.left) + thick * 2;
        const int h = (rc.bottom - rc.top) + thick * 2;

        // 抢位到顶再贴在主窗上方（不改变 owner 链）
        ::SetWindowPos(shadow_, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        ::SetWindowPos(shadow_, host_, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }



    void WndShadowEx::repaint_() {
        if (!shadow_ || !host_) return;

        RECT wr{}, hr{};
        ::GetWindowRect(shadow_, &wr);
        ::GetWindowRect(host_, &hr);

        int W = wr.right - wr.left;
        int H = wr.bottom - wr.top;
        if (W <= 2 || H <= 2) return;

        // —— 关键修正：直接用“主窗在阴影窗内的内矩形” —— //
        RECT inner{};
        inner.left = hr.left - wr.left;
        inner.top = hr.top - wr.top;
        inner.right = inner.left + (hr.right - hr.left);
        inner.bottom = inner.top + (hr.bottom - hr.top);

        // 位图
        HBITMAP hbmp = buildShadowBitmap_(W, H, inner);
        if (!hbmp) return;

        HDC hdc = ::GetDC(nullptr);
        HDC mem = ::CreateCompatibleDC(hdc);
        HGDIOBJ old = ::SelectObject(mem, hbmp);

        POINT src{ 0,0 };
        SIZE  size{ W,H };
        POINT dst{ wr.left, wr.top };

        BLENDFUNCTION bf{};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = AC_SRC_ALPHA;

        ::UpdateLayeredWindow(shadow_, hdc, &dst, &size, mem, &src, 0, &bf, ULW_ALPHA);

        wchar_t buf[256];
        swprintf(buf, 256, L"shadow=(%d,%d,%d,%d) host=(%d,%d,%d,%d)\n",
            wr.left, wr.top, wr.right, wr.bottom, hr.left, hr.top, hr.right, hr.bottom);
        OutputDebugStringW(buf);

        ::SelectObject(mem, old);
        ::DeleteDC(mem);
        ::ReleaseDC(nullptr, hdc);
        ::DeleteObject(hbmp);
    }

    void WndShadowEx::ensureDpi_() {
        dpi_ = 96;
        if (host_)
        {
            HMODULE hUser32 = ::GetModuleHandleW(L"user32.dll");
            if (hUser32)
            {
                using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
                if (auto p = (GetDpiForWindow_t)::GetProcAddress(hUser32, "GetDpiForWindow"))
                    dpi_ = p(host_);
            }
        }
    }

    int WndShadowEx::scale_(int v) const {
        return (int)((long long)v * dpi_ / 96);
    }

    HBITMAP WndShadowEx::buildShadowBitmap_(int W, int H, const RECT& inner) {
        int blur = scale_(opt_.shadowBlurRadius);
        int radius = scale_(opt_.cornerRadius);
        if (radius < 0) radius = 0;

        // BGRA top-down DIB
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = W;
        bi.bmiHeader.biHeight = -H;            // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        HBITMAP hbmp = ::CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!hbmp || !bits) return nullptr;

        auto* p = static_cast<unsigned char*>(bits);
        const int stride = W * 4;
        memset(p, 0, W * H * 4);

        // 判定：点是否在“内圆角矩形”内
        auto inside = [&](int x, int y)->bool {
            if (x < inner.left || x >= inner.right || y < inner.top || y >= inner.bottom) return false;
            if (radius == 0) return true;
            // 四角圆角
            if (x < inner.left + radius && y < inner.top + radius)
            {
                int dx = inner.left + radius - x, dy = inner.top + radius - y;
                return (dx * dx + dy * dy) <= radius * radius;
            }
            if (x >= inner.right - radius && y < inner.top + radius)
            {
                int dx = x - (inner.right - radius - 1), dy = inner.top + radius - y;
                return (dx * dx + dy * dy) <= radius * radius;
            }
            if (x < inner.left + radius && y >= inner.bottom - radius)
            {
                int dx = inner.left + radius - x, dy = y - (inner.bottom - radius - 1);
                return (dx * dx + dy * dy) <= radius * radius;
            }
            if (x >= inner.right - radius && y >= inner.bottom - radius)
            {
                int dx = x - (inner.right - radius - 1), dy = y - (inner.bottom - radius - 1);
                return (dx * dx + dy * dy) <= radius * radius;
            }
            return true;
            };

        // ---- 路径 A：blur >= 1 → 用“模糊+打洞+可见宽度限制+软衰减”（推荐，最像 Qt） ----
        if (blur >= 1)
        {
            // 1) 画“内矩形” alpha=255（模糊种子）
            for (int y = inner.top; y < inner.bottom; ++y)
            {
                unsigned char* row = p + y * stride;
                for (int x = inner.left; x < inner.right; ++x)
                {
                    if (inside(x, y)) row[x * 4 + 3] = 255;
                }
            }

            // 2) 模糊 alpha（高斯近似）
            stackBlurAlpha_(p, W, H, stride, blur);

            // 3) 对 inner 打洞（alpha=0），仅保留外圈光晕
            for (int y = inner.top; y < inner.bottom; ++y)
            {
                unsigned char* row = p + y * stride;
                for (int x = inner.left; x < inner.right; ++x)
                {
                    if (inside(x, y)) row[x * 4 + 3] = 0;
                }
            }

            // 3.5) 只保留“外扩 spread 像素”的光晕带 + 外侧软渐隐
            const int visibleSpreadPx = 4; // 96DPI 下可见宽度（你想 3/5px 就改这里）
            const int spread = scale_(visibleSpreadPx);

            const int L = inner.left - spread;
            const int T = inner.top - spread;
            const int Rr = inner.right + spread;
            const int Bb = inner.bottom + spread;
            const int R = radius + spread;

            auto outer_inside = [&](int x, int y)->bool {
                if (x < L || x >= Rr || y < T || y >= Bb) return false;
                if (R <= 0) return true;
                if (x < L + R && y < T + R)
                {
                    int dx = L + R - x, dy = T + R - y;
                    return (dx * dx + dy * dy) <= R * R;
                }
                if (x >= Rr - R && y < T + R)
                {
                    int dx = x - (Rr - R - 1), dy = T + R - y;
                    return (dx * dx + dy * dy) <= R * R;
                }
                if (x < L + R && y >= Bb - R)
                {
                    int dx = L + R - x, dy = y - (Bb - R - 1);
                    return (dx * dx + dy * dy) <= R * R;
                }
                if (x >= Rr - R && y >= Bb - R)
                {
                    int dx = x - (Rr - R - 1), dy = y - (Bb - R - 1);
                    return (dx * dx + dy * dy) <= R * R;
                }
                return true;
                };

            auto dist_to_outer = [&](int x, int y)->float {
                if (!outer_inside(x, y)) return 0.0f;
                int dxL = x - L, dxR = Rr - 1 - x, dyT = y - T, dyB = Bb - 1 - y;
                int dEdge = (std::min)((std::min)(dxL, dxR), (std::min)(dyT, dyB));
                if (radius == 0) return (float)dEdge;

                if (x < L + R && y < T + R)
                {
                    float dx = float(L + R - x - 0.5f), dy = float(T + R - y - 0.5f);
                    return (float)R - sqrtf(dx * dx + dy * dy);
                }
                if (x >= Rr - R && y < T + R)
                {
                    float dx = float(x - (Rr - R - 1) - 0.5f), dy = float(T + R - y - 0.5f);
                    return (float)R - sqrtf(dx * dx + dy * dy);
                }
                if (x < L + R && y >= Bb - R)
                {
                    float dx = float(L + R - x - 0.5f), dy = float(y - (Bb - R - 1) - 0.5f);
                    return (float)R - sqrtf(dx * dx + dy * dy);
                }
                if (x >= Rr - R && y >= Bb - R)
                {
                    float dx = float(x - (Rr - R - 1) - 0.5f), dy = float(y - (Bb - R - 1) - 0.5f);
                    return (float)R - sqrtf(dx * dx + dy * dy);
                }
                return (float)dEdge;
                };

            for (int y = 0; y < H; ++y)
            {
                unsigned char* row = p + y * stride;
                for (int x = 0; x < W; ++x)
                {
                    if (!outer_inside(x, y)) { row[x * 4 + 3] = 0; continue; }
                    float w = dist_to_outer(x, y) / (float)spread;
                    if (w < 0.f) w = 0.f; else if (w > 1.f) w = 1.f;
                    if (opt_.shadowFalloffGamma > 1.001f) w = powf(w, opt_.shadowFalloffGamma);
                    unsigned char& a = row[x * 4 + 3];
                    a = (unsigned char)(a * w + 0.5f);
                }
            }
        }
        // ---- 路径 B：blur == 0 → 不做模糊，直接“构造一条线性渐隐环带”（直角同样支持） ----
        else
        {
            const int visibleSpreadPx = 4;                 // 96DPI 可见宽度
            const int spread = scale_(visibleSpreadPx);

            // 外/内边界（外边界=inner 外扩 spread；内边界=inner）
            const int L = inner.left - spread;
            const int T = inner.top - spread;
            const int Rr = inner.right + spread;
            const int Bb = inner.bottom + spread;
            const int R = radius + spread;

            auto outer_inside = [&](int x, int y)->bool {
                if (x < L || x >= Rr || y < T || y >= Bb) return false;
                if (R <= 0) return true;
                if (x < L + R && y < T + R)
                {
                    int dx = L + R - x, dy = T + R - y;
                    return (dx * dx + dy * dy) <= R * R;
                }
                if (x >= Rr - R && y < T + R)
                {
                    int dx = x - (Rr - R - 1), dy = T + R - y;
                    return (dx * dx + dy * dy) <= R * R;
                }
                if (x < L + R && y >= Bb - R)
                {
                    int dx = L + R - x, dy = y - (Bb - R - 1);
                    return (dx * dx + dy * dy) <= R * R;
                }
                if (x >= Rr - R && y >= Bb - R)
                {
                    int dx = x - (Rr - R - 1), dy = y - (Bb - R - 1);
                    return (dx * dx + dy * dy) <= R * R;
                }
                return true;
                };

            auto dist_to_inner = [&](int x, int y)->float {
                // 到“内边界”（inner）的内向距离，用于构造线性衰减
                if (inside(x, y)) return 0.0f;
                // 直边：到 inner 的最小距离
                int dxL = inner.left - x;
                int dxR = x - (inner.right - 1);
                int dyT = inner.top - y;
                int dyB = y - (inner.bottom - 1);
                int dEdge = (std::max)(0, (std::min)((std::min)(dxL, dxR), (std::min)(dyT, dyB)));

                
                if (radius == 0) return (float)dEdge;

                // 四角（映射到内圆角的圆弧距离）
                if (x < inner.left && y < inner.top)
                {
                    float dx = float(inner.left - x - 0.5f);
                    float dy = float(inner.top - y - 0.5f);
                    return std::max(0.0f, sqrtf(dx * dx + dy * dy) - radius);
                }
                if (x >= inner.right && y < inner.top)
                {
                    float dx = float(x - (inner.right - 1) - 0.5f);
                    float dy = float(inner.top - y - 0.5f);
                    return std::max(0.0f, sqrtf(dx * dx + dy * dy) - radius);
                }
                if (x < inner.left && y >= inner.bottom)
                {
                    float dx = float(inner.left - x - 0.5f);
                    float dy = float(y - (inner.bottom - 1) - 0.5f);
                    return std::max(0.0f, sqrtf(dx * dx + dy * dy) - radius);
                }
                if (x >= inner.right && y >= inner.bottom)
                {
                    float dx = float(x - (inner.right - 1) - 0.5f);
                    float dy = float(y - (inner.bottom - 1) - 0.5f);
                    return std::max(0.0f, sqrtf(dx * dx + dy * dy) - radius);
                }
                return (float)dEdge;
                };

            for (int y = 0; y < H; ++y)
            {
                unsigned char* row = p + y * stride;
                for (int x = 0; x < W; ++x)
                {
                    if (!outer_inside(x, y)) { row[x * 4 + 3] = 0; continue; }
                    float d = dist_to_inner(x, y);     // 0..spread
                    if (d <= 0.f || d > (float)spread) { row[x * 4 + 3] = 0; continue; }
                    float w = 1.0f - (d / (float)spread);    // 线性 1→0
                    if (opt_.shadowFalloffGamma > 1.001f) w = powf(w, opt_.shadowFalloffGamma);
                    row[x * 4 + 3] = (unsigned char)(255.0f * w + 0.5f);
                }
            }
        }

        // 4) 预乘着色
        const BYTE ca = opt_.shadowAlpha;
        const BYTE cr = GetRValue(opt_.shadowColor);
        const BYTE cg = GetGValue(opt_.shadowColor);
        const BYTE cb = GetBValue(opt_.shadowColor);

        for (int y = 0; y < H; ++y)
        {
            unsigned char* row = p + y * stride;
            for (int x = 0; x < W; ++x)
            {
                unsigned char* px = row + x * 4;
                BYTE a = (BYTE)((px[3] * ca) / 255);
                px[0] = (BYTE)((cb * a) / 255);
                px[1] = (BYTE)((cg * a) / 255);
                px[2] = (BYTE)((cr * a) / 255);
                px[3] = a;
            }
        }
        return hbmp;
    }


    void WndShadowEx::stackBlurAlpha_(unsigned char* bits, int w, int h, int stride, int radius) {
        if (radius < 1) return;
        std::vector<unsigned char> tmp(h * w);

        // 水平
        for (int y = 0; y < h; ++y)
        {
            unsigned char* row = bits + y * stride;
            int sum = 0, limit = radius * 2 + 1;
            for (int x = -radius; x <= radius; ++x)
            {
                int ix = x < 0 ? 0 : (x >= w ? w - 1 : x);
                sum += row[ix * 4 + 3];
            }
            for (int x = 0; x < w; ++x)
            {
                tmp[y * w + x] = (unsigned char)(sum / limit);
                int x_add = x + radius + 1; if (x_add >= w) x_add = w - 1;
                int x_sub = x - radius;     if (x_sub < 0)   x_sub = 0;
                sum += row[x_add * 4 + 3] - row[x_sub * 4 + 3];
            }
        }

        // 垂直
        for (int x = 0; x < w; ++x)
        {
            int sum = 0, limit = radius * 2 + 1;
            for (int y = -radius; y <= radius; ++y)
            {
                int iy = y < 0 ? 0 : (y >= h ? h - 1 : y);
                sum += tmp[iy * w + x];
            }
            for (int y = 0; y < h; ++y)
            {
                unsigned char a = (unsigned char)(sum / limit);
                bits[y * stride + x * 4 + 3] = a;
                int y_add = y + radius + 1; if (y_add >= h) y_add = h - 1;
                int y_sub = y - radius;     if (y_sub < 0)  y_sub = 0;
                sum += tmp[y_add * w + x] - tmp[y_sub * w + x];
            }
        }
    }

} // namespace DuiLib
