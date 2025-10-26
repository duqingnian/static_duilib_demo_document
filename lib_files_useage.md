# duilib_static 项目文件用途概览

以下按照目录列出 duilib_static 工程中的 80 个核心源码文件及其主要作用，帮助理解各模块在 UI 框架中的职责分工。

## Control 目录

- **duilib_static/Control/UIActiveX.cpp**：实现 ActiveX 宿主窗口过程、消息分发以及与 CActiveXUI 控件的交互逻辑。
- **duilib_static/Control/UIActiveX.h**：声明 CActiveXUI 控件及其嵌入 ActiveX 组件所需的接口。
- **duilib_static/Control/UIButton.cpp**：定义按钮控件的事件响应、状态切换、热态渐隐等行为实现。
- **duilib_static/Control/UIButton.h**：声明通用按钮控件 CButtonUI 的属性配置与接口。
- **duilib_static/Control/UICheckBox.cpp**：实现复选框控件的选中状态管理、组切换和通知发送。
- **duilib_static/Control/UICheckBox.h**：声明复选框 CCheckBoxUI 派生自选项控件的接口。
- **duilib_static/Control/UICombo.cpp**：实现下拉组合框的弹出窗口、列表项选择及样式控制。
- **duilib_static/Control/UICombo.h**：声明组合框 CComboUI 对列表容器与编辑区的封装接口。
- **duilib_static/Control/UIDateTime.cpp**：封装原生 DATETIMEPICK 控件窗口并处理时间选择交互。
- **duilib_static/Control/UIDateTime.h**：声明日期时间控件 CDateTimeUI 的属性与消息接口。
- **duilib_static/Control/UIEdit.cpp**：实现文本编辑控件的原生窗口托管、输入限制与状态同步。
- **duilib_static/Control/UIEdit.h**：声明编辑框 CEditUI 的属性、事件及辅助方法。
- **duilib_static/Control/UIFlash.cpp**：定义 Flash 播放控件的 ActiveX 宿主逻辑与播放控制。
- **duilib_static/Control/UIFlash.h**：声明 CFlashUI 控件以及访问 Flash COM 接口的包装。
- **duilib_static/Control/UIGifAnim.cpp**：实现 GIF 动画控件的帧加载、播放计时与重绘。
- **duilib_static/Control/UIGifAnim.h**：声明 CGifAnimUI 并暴露帧控制、循环次数等属性。
- **duilib_static/Control/UILabel.cpp**：实现通用文本标签的绘制、字体与颜色处理。
- **duilib_static/Control/UILabel.h**：声明 CLabelUI 作为多数控件的文本基类接口。
- **duilib_static/Control/UIList.cpp**：实现列表控件的数据项、滚动、拖拽及子元素管理。
- **duilib_static/Control/UIList.h**：声明 CListUI 及其项、元素接口与回调定义。
- **duilib_static/Control/UIOption.cpp**：实现单选项控件的选中状态切换和通知机制。
- **duilib_static/Control/UIOption.h**：声明 COptionUI 以及单选分组相关接口。
- **duilib_static/Control/UIProgress.cpp**：实现进度条的数值更新、渐变绘制与动画支持。
- **duilib_static/Control/UIProgress.h**：声明 CProgressUI 的数据成员和配置接口。
- **duilib_static/Control/UIRichEdit.cpp**：封装原生 RichEdit 控件的窗口过程、格式化与文本操作。
- **duilib_static/Control/UIRichEdit.h**：声明 CRichEditUI 对富文本特性及通知的接口。
- **duilib_static/Control/UIScrollBar.cpp**：实现滚动条的滑块拖动、按钮响应和滚动范围控制。
- **duilib_static/Control/UIScrollBar.h**：声明 CScrollBarUI 的属性访问与事件接口。
- **duilib_static/Control/UISlider.cpp**：实现滑块控件的拖动、取值换算与样式绘制。
- **duilib_static/Control/UISlider.h**：声明 CSliderUI 支持垂直/水平滑动和步进配置。
- **duilib_static/Control/UIText.cpp**：实现超链接文本控件的富文本绘制与交互事件。
- **duilib_static/Control/UIText.h**：声明 CTextUI 派生自 CLabelUI 的扩展文本接口。
- **duilib_static/Control/UITreeView.cpp**：实现树形控件的节点管理、展开折叠和复选支持。
- **duilib_static/Control/UITreeView.h**：声明 CTreeViewUI 及树节点类的结构与接口。
- **duilib_static/Control/UIWebBrowser.cpp**：封装 WebBrowser ActiveX 控件的宿主窗口和事件处理。
- **duilib_static/Control/UIWebBrowser.h**：声明 CWebBrowserUI 控件对网页渲染及脚本交互的接口。

## Core 目录

- **duilib_static/Core/UIBase.cpp**：提供框架基础设施，如字符串、颜色、矩形等辅助类型实现。
- **duilib_static/Core/UIBase.h**：声明通用基础类型、宏定义及轻量工具类。
- **duilib_static/Core/UIContainer.cpp**：实现容器控件的子控件布局、循环与焦点处理。
- **duilib_static/Core/UIContainer.h**：声明 CContainerUI 及其容器接口和迭代器。
- **duilib_static/Core/UIControl.cpp**：实现所有控件共有的属性存取、绘制和消息分发逻辑。
- **duilib_static/Core/UIControl.h**：声明控件基类 CControlUI 的完整接口集合。
- **duilib_static/Core/UIDefine.h**：集中定义常量、消息类型和控件注册字符串。
- **duilib_static/Core/UIDlgBuilder.cpp**：实现 XML 布局解析器和运行时控件构建流程。
- **duilib_static/Core/UIDlgBuilder.h**：声明对话框构建器 CDuiStringPtrMap 与解析接口。
- **duilib_static/Core/UIManager.cpp**：实现 CPaintManagerUI 的消息循环、资源管理及全局状态。
- **duilib_static/Core/UIManager.h**：声明管理器 CPaintManagerUI 的接口、回调和辅助结构。
- **duilib_static/Core/UIMarkup.cpp**：实现 XML 标记解析、节点遍历与属性读取。
- **duilib_static/Core/UIMarkup.h**：声明 CMarkupParser、CMarkupNode 等 XML 解析类。
- **duilib_static/Core/UIRender.cpp**：实现 GDI 绘制封装、缓冲渲染与位图操作。
- **duilib_static/Core/UIRender.h**：声明 CRenderEngine 等渲染工具类的接口。

## Layout 目录

- **duilib_static/Layout/UIChildLayout.cpp**：实现子窗口布局加载及按需延迟创建。
- **duilib_static/Layout/UIChildLayout.h**：声明 CChildLayoutUI 控件的接口与子布局管理。
- **duilib_static/Layout/UIHorizontalLayout.cpp**：实现水平布局容器对子项的排列与拉伸策略。
- **duilib_static/Layout/UIHorizontalLayout.h**：声明 CHorizontalLayoutUI 的布局属性接口。
- **duilib_static/Layout/UITabLayout.cpp**：实现选项卡布局的页切换与激活状态维护。
- **duilib_static/Layout/UITabLayout.h**：声明 CTabLayoutUI 提供页面管理接口。
- **duilib_static/Layout/UITileLayout.cpp**：实现平铺布局的网格计算与项换行规则。
- **duilib_static/Layout/UITileLayout.h**：声明 CTileLayoutUI 的网格尺寸与排列接口。
- **duilib_static/Layout/UIVerticalLayout.cpp**：实现垂直布局容器的子项尺寸与对齐策略。
- **duilib_static/Layout/UIVerticalLayout.h**：声明 CVerticalLayoutUI 的属性与方法。

## 根目录公共文件

- **duilib_static/StdAfx.cpp**：预编译头源文件，集中编译常用系统与库头文件。
- **duilib_static/StdAfx.h**：预编译头声明，定义全局包含、版本宏及平台配置。
- **duilib_static/UIlib.cpp**：导出库入口，聚合命名空间实现和模块初始化。
- **duilib_static/UIlib.h**：库对外唯一包含头，导出所有公开接口和命名空间引用。

## Utils 目录

- **duilib_static/Utils/FlashEventHandler.h**：声明处理 Flash ActiveX 事件的 COM 事件接收器。
- **duilib_static/Utils/UIDelegate.cpp**：实现回调委托机制，支持事件绑定与函数封装。
- **duilib_static/Utils/UIDelegate.h**：声明模板化委托类及回调绑定辅助工具。
- **duilib_static/Utils/Utils.cpp**：提供通用工具函数，如字符串、路径与系统辅助操作实现。
- **duilib_static/Utils/Utils.h**：声明工具函数、常量以及环境检测接口。
- **duilib_static/Utils/WebBrowserEventHandler.h**：声明 WebBrowser ActiveX 的事件处理 COM 接收器。
- **duilib_static/Utils/WinImplBase.cpp**：实现窗口封装基类 CWindowWnd、CNotifyPump 等消息循环逻辑。
- **duilib_static/Utils/WinImplBase.h**：声明窗口实现基类及通知泵接口，为应用提供窗口封装。
- **duilib_static/Utils/WndShadow.cpp**：实现窗口阴影绘制、尺寸计算与透明度控制。
- **duilib_static/Utils/WndShadow.h**：声明简单阴影辅助类 CWndShadow 的接口。
- **duilib_static/Utils/WndShadowEx.cpp**：实现扩展阴影效果，支持多边形区域与缓存渲染。
- **duilib_static/Utils/WndShadowEx.h**：声明增强型阴影管理类 CShadowWnd/CShadowUI。
- **duilib_static/Utils/XUnzip.cpp**：提供 ZIP 解压实现，支持资源包读取。
- **duilib_static/Utils/downloadmgr.h**：声明简单下载管理器接口，用于资源更新。
- **duilib_static/Utils/stb_image.h**：第三方头文件，提供多格式图片解码功能。

