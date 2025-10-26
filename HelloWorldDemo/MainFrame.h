#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#define NOMINMAX
#define _HAS_STD_BYTE 0

#include <windows.h>
#include "UIlib.h"
#include "Utils/WndShadowEx.h"

using namespace DuiLib;

class MainFrame : public WindowImplBase
{
public:
	MainFrame();
	~MainFrame();



public:
	LPCTSTR GetWindowClassName() const;
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();

	void SetShadowOptions(const DuiLib::ShadowOptions& opt);
	
	virtual DuiLib::UILIB_RESOURCETYPE GetResourceType() const override;
	
	virtual DuiLib::CDuiString GetSkinFile() override {
		return _T("skin.xml");
	}
	virtual DuiLib::CDuiString GetSkinFolder() override {
		return _T("res"); 
	}

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void OnExit(TNotifyUI& msg);
	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);

protected:
	CButtonUI* m_btnPopup; //按钮 - 弹窗
	CButtonUI* m_btnApply; //按钮 - 应用到文本
	CButtonUI* m_btnClear; //按钮 - 清空输入和文本

	CEditUI* m_editInput;
	CLabelUI* m_lblOutput;

	CContainerUI* m_pMainWndContent = nullptr;

	CListUI* m_pMainWndList = nullptr;

	DuiLib::WndShadowEx shadow_;
	DuiLib::ShadowOptions shadowOpt_;
};

#endif // MAINFRAME_HPP