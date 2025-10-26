#define NOMINMAX
#define _HAS_STD_BYTE 0

#include <windows.h>
#include <string>
#include <cassert>
#include "UIlib.h"
#include "resource.h"
#include "MainFrame.h"

#include <shellscalingapi.h>
#pragma comment(lib, "Shcore.lib")

using namespace DuiLib;

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CPaintManagerUI::SetInstance(hInst);

    ShadowOptions opt;
    

    opt.cornerRadius = 4; 
    opt.shadowBlurRadius = 8;
    opt.shadowVisibleSpread = 4;                 // 关键：锁死 4px 可见宽度
    opt.shadowAlpha = 88;                // ≈35% 不透明度（72~100 之间微调）
    opt.shadowOffsetX = 0;
    opt.shadowOffsetY = 3;                 // 轻微下坠
    opt.shadowThickness = 2 * opt.shadowBlurRadius + opt.shadowVisibleSpread + 6;
    // thickness 只要足够容纳 blur + spread，别太大以免显“肥边”
    opt.shadowFalloffGamma = 2.2f;  // 2.0~2.4，让边缘更陡更像 Qt


    MainFrame* pFrame = new MainFrame();
    if (!pFrame) return 0;
    pFrame->Create(NULL, _T("MenuTest"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 0, 0);
    pFrame->SetShadowOptions(opt);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOW);

    CPaintManagerUI::MessageLoop();

    return 0;
}