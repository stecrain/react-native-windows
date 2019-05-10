# Extending React Native Windows (vnext)

React Native comes with a number of built-in components to access platform functionality and use native UI in your app. It also supports extending your app to access additional native functionality whether that is to interop between JavaScript and native code from a 3rd party library or to incorporate native UI controls into your application.

A **native module** lets you expose the methods and variables of native code for use in the JavaScript of your application. For example, if you needed access to a third-party native image processing library, you can implement a custom native module as a wrapper to expose that library's functionality to your React Native app.

A **view manager** is a specialized native module with additional support to interop with a native UI framework and expose a native UI control for use as a React Native component. For example, if you wanted to use a third-party XAML control in your React Native app, you would create a custom view manager to wrap that control as a component and then use it in your React Native app.

## Current Support

**Tl:dr;** If you'll need to wrap native UI with a custom view manager, then for now you'll need to use C++/winrt as the language for your React Native host app.

**_App Authors_**

You can set up a native Windows app (UWP) that is based on C#, C++/CX, or C++/winrt.  However, we're still working on API support for each language to author custom view managers.

> | Host Application <br/>Language | Custom <br/>Native Modules | Custom <br/>View Managers |
> |:--------------------------|:---------------------:|:--------------------:|
> | C#                        | Yes                   | No                   |
> | C++/CX                    | Yes                   | No                   |
> | C++/winrt                 | Yes                   | Yes                  |

**_Library Authors_**

We don't yet have a project template for creating a shareable library that others can incorporate into their project. If you need to create one then please reach out and let us know on the [#rn-windows](https://discordapp.com/channels/514829729862516747/559809806949679122) Discord channel.  

## High-Level Overview

If your UWP host application is...
### C++/WinRT 

#### Creating a Custom Native Module

1. Subclass `facebook::xplat::module::CxxModule`
   1. Override `getName()` with your module's name
   2. Override `getMethods()` with the list of methods your module will provide
   3. Override `getConstants()` with the constants that your module will provide
2. Subclass `facebook::react::NativeModuleProvider` 
   1. Override `GetModules()` to return a collection that contains an instance of your subclassed `CxxModule`
3. When you call `react::uwp::CreateReactInstance()`, pass in your subclassed `NativeModuleProvider` to make your module available in addition to those that are built-in

#### Creating a Custom View Manager

1. Subclass `react::uwp::FrameworkElementViewManager`
2. Subclass `react::uwp::ShadowNodeBase`
3. Subclass `react::uwp::ViewManagerProvider`
   1. Override `GetViewManagers()` to return a collection that contains an instance of your subclassed `FrameworkElementViewManager`
4. When you call `react::uwp::CreateReactInstance()`, pass in your subclassed `ViewManagerProvider`

### C# _or_ <br/>C++/CX

#### Creating a Custom Native Module

1. Create a class which implements `react::uwp::IModule`
2. Call `RegisterModule`, on your ReactInstance with your `IModule` (*Note:* this will be changed with issue [#2392](https://github.com/microsoft/react-native-windows/issues/2392))

#### Creating a Custom View Manager

Not yet implemented. See issue [#2298](https://github.com/microsoft/react-native-windows/issues/2298).

## FAQ

### How do I use a custom C++/WinRT native module in my C# (or C++/CX) UWP Host Application (or vice-versa)?

Not yet implemented. See issue [#2393](https://github.com/microsoft/react-native-windows/issues/2393).
