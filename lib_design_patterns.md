# DuiLib 设计模式概览

## 观察者模式（Observer）
- `CPaintManagerUI` 维护了一组实现 `INotifyUI` 接口的观察者，并在 `SendNotify` 中向所有监听者广播控件事件，实现控件事件与窗口逻辑的解耦。代码展示了如何向观察者集合添加和移除监听者，以及在事件发生时循环通知每一个观察者。【F:duilib_static/Core/UIManager.h†L152-L169】【F:duilib_static/Core/UIManager.cpp†L1964-L1999】【F:duilib_static/Core/UIManager.cpp†L2122-L2176】

## 构建者模式（Builder）
- `CDialogBuilder` 负责从 XML/资源中解析界面描述，逐步创建控件、加载资源并设置属性，最终生成完整的控件树。它将复杂的界面构建过程封装起来，使调用者只需提供资源入口即可得到构建好的界面结构。【F:duilib_static/Core/UIDlgBuilder.cpp†L5-L178】【F:duilib_static/Core/UIDlgBuilder.cpp†L382-L408】

## 工厂方法模式（Factory Method）
- 在 `CDialogBuilder::_Parse` 中，根据控件类型字符串选择并实例化不同的控件类；同时支持插件回调与 `IDialogBuilderCallback::CreateControl` 的扩展点，让调用者可通过覆写工厂方法创建自定义控件。【F:duilib_static/Core/UIDlgBuilder.cpp†L300-L368】
- `WindowImplBase::CreateControl` 作为虚函数，供业务窗口类按需覆写以插入自定义控件创建逻辑，配合 `CDialogBuilder` 的控件解析流程形成可扩展的工厂方法结构。【F:duilib_static/Utils/WinImplBase.h†L33-L53】【F:HelloWorldDemo/MainFrame.cpp†L24-L27】

## 模板方法模式（Template Method）
- `WindowImplBase::HandleMessage` 定义了窗口消息处理的总体流程：先分派到一系列虚函数（如 `OnCreate`、`OnSize` 等），再尝试自定义消息、最后回退到默认处理。派生类通过覆写这些钩子方法在不改变整体流程的前提下定制行为。【F:duilib_static/Utils/WinImplBase.cpp†L361-L415】【F:duilib_static/Utils/WinImplBase.h†L17-L63】
- `MainFrame` 等业务窗口覆盖 `HandleMessage`/`InitWindow` 等方法，通过调用 `__super` 保留模板流程，再在钩子内添加自身逻辑，体现了模板方法的使用方式。【F:HelloWorldDemo/MainFrame.cpp†L35-L52】【F:HelloWorldDemo/MainFrame.cpp†L59-L67】

