# 程序执行流程梳理

以下按照 `wWinMain` 的执行顺序，从入口初始化到程序退出，梳理主要调用链路与关键逻辑。

## 1. 入口函数 `wWinMain`
1. `SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);` 启用每显示器 DPI 感知，使窗口在高分辨率环境下正常缩放。【F:HelloWorldDemo/main.cpp†L17-L18】
2. `CPaintManagerUI::SetInstance(hInst);` 将当前实例句柄注册到全局 UI 管理器，后续创建窗口/加载资源时要用到该实例。【F:HelloWorldDemo/main.cpp†L19-L20】
3. 构造 `ShadowOptions` 并配置阴影参数，供窗口创建后套用自定义阴影效果。【F:HelloWorldDemo/main.cpp†L22-L31】
4. `new MainFrame()`：实例化主窗口类。`MainFrame` 继承自 `DuiLib::WindowImplBase`，后者又派生自 `CWindowWnd`，因此具备窗口创建和 UI 框架逻辑。【F:HelloWorldDemo/main.cpp†L33-L34】【F:HelloWorldDemo/MainFrame.h†L14-L28】
5. `pFrame->Create(...)` 调用 `CWindowWnd::Create`，完成窗口类注册和原生窗口句柄创建。该函数在创建时把 `this` 作为 `lpParam` 传入 Win32 的 `CreateWindowEx`，配合静态窗口过程把后续消息分派到对象实例。【F:HelloWorldDemo/main.cpp†L35-L36】【F:duilib_static/Core/UIBase.cpp†L241-L270】
6. 阴影参数通过 `SetShadowOptions` 传递给 `MainFrame`，随后调用 `CenterWindow`、`ShowWindow` 展示主窗体。【F:HelloWorldDemo/main.cpp†L37-L39】
7. 进入 `CPaintManagerUI::MessageLoop()` 消息循环，持续处理窗口消息直到收到 `WM_QUIT`。【F:HelloWorldDemo/main.cpp†L41】【F:duilib_static/Core/UIManager.cpp†L1735-L1754】

## 2. 窗口创建阶段（`Create` → `WM_CREATE`）
1. Win32 创建窗口后，会发送 `WM_CREATE`。该消息由 `WindowImplBase::HandleMessage` 捕获，进而调用 `OnCreate` 完成 UI 框架初始化。【F:duilib_static/Utils/WinImplBase.cpp†L379-L417】
2. `OnCreate` 做了以下工作：
   - 去掉原生标题栏样式，重新设置窗口客户区；
   - 调用 `m_PaintManager.Init(m_hWnd)` 并把当前对象注册为前置消息过滤器、通知接收者；
   - 依据 `GetSkinFolder()/GetSkinFile()` 和 `GetResourceType()` 加载皮肤资源（本例返回 `res/skin.xml`，类型为磁盘文件）；
   - 使用 `CDialogBuilder` 解析 XML，生成根控件树并挂载到 `CPaintManagerUI`；
   - 最后调用派生类覆写的 `InitWindow()`。【F:duilib_static/Utils/WinImplBase.cpp†L300-L373】

## 3. `MainFrame::InitWindow` 与控件准备
1. `InitWindow` 设置窗口图标，并调用 `shadow_.Attach(GetHWND(), shadowOpt_)` 将 `wWinMain` 中准备好的阴影参数应用到真实窗口。【F:HelloWorldDemo/MainFrame.cpp†L48-L62】
2. 当界面加载完成时，DuiLib 会发送 `windowinit` 通知，`MainFrame::Notify` 捕获后调用 `OnPrepare`，在其中通过 `m_PaintManager.FindControl` 获取 UI 控件指针（按钮、输入框、列表等）以便后续逻辑使用。【F:HelloWorldDemo/MainFrame.cpp†L70-L113】

## 4. 消息与事件处理
1. `MainFrame::HandleMessage` 在窗口移动/大小变化时调用阴影对象的 `Update`、在显示/隐藏时调用 `Show`、在销毁时调用 `Detach`，然后将消息转交给 `WindowImplBase::HandleMessage` 继续处理默认逻辑和 UI 派发。【F:HelloWorldDemo/MainFrame.cpp†L26-L46】
2. `MainFrame::Notify` 处理来自控件的业务事件：
   - `click` + `closebtn`：调用 `OnExit` → `Close()`，触发窗口销毁流程；
   - `click` + `btnPopup`：弹出当前时间；
   - `click` + `btnApply`：把输入框文本复制到标签；
   - `click` + `btnClear`：清空文本；
   - 其他通知交由基类 `WindowImplBase::Notify` 继续分发。【F:HelloWorldDemo/MainFrame.cpp†L72-L109】
3. `WindowImplBase::HandleMessage` 负责大部分系统消息（非客户区命中测试、最小/最大化、鼠标键盘等），并把未处理的消息交给 `CPaintManagerUI` 或底层的 `CWindowWnd::HandleMessage`。这确保 UI 控件可以响应输入并渲染。【F:duilib_static/Utils/WinImplBase.cpp†L239-L418】

## 5. 程序退出
1. 当用户触发关闭（例如点击关闭按钮或发送 `WM_CLOSE`），`WindowImplBase::HandleMessage` 调用 `OnClose`/`OnDestroy` 并最终触发 `OnFinalMessage`。
2. `MainFrame::OnFinalMessage` 先让基类回收 UI 资源，再执行 `delete this`。此时会进入 `MainFrame` 析构函数，执行 `PostQuitMessage(0)`，导致消息循环收到 `WM_QUIT` 并退出。【F:HelloWorldDemo/MainFrame.cpp†L20-L34】【F:HelloWorldDemo/MainFrame.cpp†L12-L16】
3. `CPaintManagerUI::MessageLoop()` 返回后，`wWinMain` 跳出循环并返回 0，程序结束。【F:HelloWorldDemo/main.cpp†L41-L45】

以上流程展示了从入口到退出的完整调用链，涵盖窗口创建、资源加载、消息分发以及退出清理等关键步骤。
