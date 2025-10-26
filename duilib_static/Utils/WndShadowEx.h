#pragma once
#include <windows.h>
#include <vector>

namespace DuiLib {

    struct ShadowOptions {
        int      cornerRadius = 8;               // 圆角（96DPI 基准像素）
        int      shadowBlurRadius = 14;              // 模糊半径
        COLORREF shadowColor = RGB(0, 0, 0);      // 阴影颜色（不含 alpha）
        BYTE     shadowAlpha = 100;             // 阴影不透明度（0~255）
        int      shadowOffsetX = 0;               // 阴影偏移（右/下为正）
        int      shadowOffsetY = 0;
        int      shadowThickness = 16;              // 主窗四周留白（容纳光晕）
        int      titleBarHeight = 36;              // 仅供上层自绘用；算法不使用
        float   shadowFalloffGamma = 1.0f;
    
        int   shadowVisibleSpread = 4;  // 阴影可见宽度，锁死为 4px（Qt 风）
    };

    class WndShadowEx {
    public:
        WndShadowEx();
        ~WndShadowEx();

        void Attach(HWND hHost, const ShadowOptions& opt);
        void Detach();
        void Update();
        void Show(bool vis);

    private:
        void createShadowWindow_();
        void destroyShadowWindow_();
        void reposition_();
        void repaint_();
        void ensureDpi_();
        int  scale_(int v) const;

        // 按“主窗在阴影窗里的实际内矩形”生成位图
        HBITMAP buildShadowBitmap_(int bmpW, int bmpH, const RECT& innerRect);

        // 仅对 alpha 通道做 stack blur（水平+垂直两 pass）
        static void stackBlurAlpha_(unsigned char* bits, int w, int h, int stride, int radius);

    private:
        HWND host_{};
        HWND shadow_{};
        ShadowOptions opt_{};
        bool attached_{ false };
        bool visible_{ true };
        UINT dpi_{ 96 };
    };

} // namespace DuiLib
