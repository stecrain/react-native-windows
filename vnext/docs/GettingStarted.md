# Getting Started Guide for React Native Windows (vnext)

This is a summary of setup steps needed to install and work with React Native for Windows (vnext). See the [React Native Getting Started Guide](http://facebook.github.io/react-native/docs/getting-started.html) for React Native details and see [Getting Started Guide - current](https://github.com/microsoft/react-native-windows/blob/master/current/docs/GettingStarted.md) for working with the `current` React Native for Windows implementation.

## System requirements
* You can run React-Native for Windows10 apps only on Windows 10 devices and Windows version: 10.0.15063.0 or higher.
* [Visual Studio 2019](https://www.visualstudio.com/downloads) with the following options:
  * Workloads
    * Universal Windows Platform development
      * Enable the optional `C++ (v141) Universal Windows Platform tools`
    * Desktop development with C++
  * Individual Components
    * Development activities
      * Node.js development support (optional)
    * SDKs, libraries, and frameworks per your versioning needs
      * Windows 10 SDK (10.0.18362.0)

## Dependencies
* Install the dependencies [specified by React Native](http://facebook.github.io/react-native/docs/getting-started.html#node-python2-jdk). Specifically, make sure a recent version of [Node.js](https://nodejs.org) is installed. [Chocolatey](https://chocolatey.org/) is the React Native recommended installation method. But you can also install Node directly from [NodeJs](https://nodejs.org/en/download/).  To use chocolately, from an elevated Command Prompt, run:
  ```
  choco install nodejs
  ```
* Install [Chrome](https://www.google.com/chrome/) (*optional*, but needed for JS debugging)
* Install [Yarn](https://yarnpkg.com/en/docs/install) (*optional* if consuming react-native-windows, but required to work in the react-native-windows repo)

## Installing React Native for Windows (vnext)
There are two ways to install and work with React Native for Windows (vnext):
1. [Consuming react-native-windows](./ConsumingRNW.md) : Using react-native-windows in your package
1. [Building react-native-windows repo](./BuildingRNW.md) : Manually by cloning the repo and get things running.

## E2E Test
Please refer to [Author and Run E2E Test for React Native Windows](E2ETest.md)

# Troubleshooting
* If after running the app the packager does not update (or) app does not show React Native content - close the packager command prompt window and the app, run `yarn start` and run the app again.  Issue [#2311](https://github.com/microsoft/react-native-windows/issues/2311) is tracking a known issue on this.
* If you get a red error box in your UWP app window with the error message : `ERROR: Instance failed to start. A connection with the server cannot be established`, make sure you have the packager running using `yarn start` and run the app again.
* If you are trying to run your `react-native` app on iOS/Android while using this `vnext` implementation for developing/running on Windows, you will encounter errors while running the app for other platforms. This will be fixed once we address Issues [#2264](https://github.com/microsoft/react-native-windows/issues/2264) and [#2535](https://github.com/microsoft/react-native-windows/issues/2535). Until this is fixed, please refer to [this comment](https://github.com/microsoft/react-native-windows/issues/2515#issuecomment-497375198) which describes the workaround for running on other platforms while developing for windows using `vnext`.
