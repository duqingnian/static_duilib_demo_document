#include "MainFrame.h"
#include "resource.h"
#include <time.h>



MainFrame::MainFrame()
{
}

MainFrame::~MainFrame()
{
    PostQuitMessage(0);
}



// 窗体类名
LPCTSTR MainFrame::GetWindowClassName() const
{
    return _T("DuilibDevWindowClass");
}

CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass)
{
    return NULL;
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
    WindowImplBase::OnFinalMessage(hWnd);
    delete this;
}

LRESULT MainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
    case WM_MOVE:
    case WM_WINDOWPOSCHANGED:
        shadow_.Update();
        break;
    case WM_SHOWWINDOW:
        shadow_.Show(wParam != 0);
        break;
    case WM_DESTROY:
        shadow_.Detach();
        break;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

void MainFrame::OnExit(TNotifyUI& msg)
{
    Close();
}

void MainFrame::InitWindow()
{
    //设置程序图标
    HICON hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    ::SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    ::SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    shadow_.Attach(GetHWND(), shadowOpt_);
}

void MainFrame::SetShadowOptions(const DuiLib::ShadowOptions& opt)
{
    shadowOpt_ = opt; 
}

DuiLib::UILIB_RESOURCETYPE MainFrame::GetResourceType() const
{
    return DuiLib::UILIB_FILE;
}


void MainFrame::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
    {
        OnPrepare(msg);
    }
    else if (_tcsicmp(msg.sType, _T("click")) == 0)
    {
        if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
        {
            OnExit(msg);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("btnPopup")) == 0)
        {
            SYSTEMTIME st{};
            ::GetLocalTime(&st);
            TCHAR buf[64];
            _stprintf_s(buf, _T("%04d年%02d月%02d日 %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            ::MessageBox(NULL, buf, _T("当前时间"), MB_OK | MB_ICONINFORMATION);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("btnApply")) == 0)
        {
            if (!m_editInput || !m_lblOutput)
            {
                ::MessageBox(m_hWnd, _T("控件未初始化：检查 OnPrepare 和控件 name"), _T("提示"), MB_OK | MB_ICONWARNING);
                return;
            }
            m_lblOutput->SetText(m_editInput->GetText());
        }
        else if (msg.pSender == m_btnClear)
        {
            if (m_editInput) m_editInput->SetText(_T(""));
            if (m_lblOutput) m_lblOutput->SetText(_T(""));
        }
        else
        {
        }
    }

    WindowImplBase::Notify(msg);
}

void MainFrame::OnPrepare(TNotifyUI& msg)
{
    m_btnApply = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnApply")));
    m_btnClear = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClear")));
    m_editInput = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editInput")));
    m_lblOutput = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblOutput")));

    m_pMainWndContent = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("main_wnd_content")));
    m_pMainWndList = dynamic_cast<CListUI*>(m_PaintManager.FindControl(_T("main_wnd_list")));
}
