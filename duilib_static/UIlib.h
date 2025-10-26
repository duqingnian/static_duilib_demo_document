typedef unsigned char byte;

#ifdef UILIB_STATIC
#	define DUILIB_API 
#else
#	if defined(UILIB_EXPORTS)
#		if	defined(_MSC_VER)
#			define DUILIB_API __declspec(dllexport)
#		else
#			define DUILIB_API 
#		endif
#	else
#		if defined(_MSC_VER)
#			define DUILIB_API __declspec(dllimport)
#		else
#			define DUILIB_API 
#		endif
#	endif
#endif

#define UILIB_COMDAT __declspec(selectany)

#if defined _M_IX86
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>      // 确保在其他头文件之前包含 windows.h
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>        // stddef.h 中有可能定义了 byte
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <stdio.h>
#include <gdiplus.h>       // 将 gdiplus.h 放在最后


#include "Utils/Utils.h"
#include "Utils/UIDelegate.h"
#include "Core/UIDefine.h"
#include "Core/UIManager.h"
#include "Core/UIBase.h"
#include "Core/UIControl.h"
#include "Core/UIContainer.h"
#include "Core/UIMarkup.h"
#include "Core/UIDlgBuilder.h"
#include "Core/UIRender.h"
#include "Utils/WinImplBase.h"
#include "Utils/WndShadow.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"

#include "Control/UILabel.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"

#include "Control/UIButton.h"
#include "Control/UIOption.h"
#include "Control/UICheckBox.h"

#include "Control/UIProgress.h"
#include "Control/UISlider.h"

#include "Control/UIRichEdit.h"
#include "Control/UIDateTime.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIGifAnim.h"
//#include "Control/UIFlash.h"
