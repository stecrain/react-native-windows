// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxReact/Instance.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <memory>
#include <vector>

namespace react {
namespace uwp {

// TODO: Emit event to react when dimensions change.
class DeviceInfoModule : public facebook::xplat::module::CxxModule {
 public:
  DeviceInfoModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static void uiThreadAvailable();

  static const char *name;

 private:
  folly::dynamic getDimensions(
      winrt::Windows::Graphics::Display::DisplayInformation displayInfo,
      winrt::Windows::UI::Core::CoreWindow window);
  static DeviceInfoModule *s_currentInstance;

  void sendDimensionsChangedEvent();
};

} // namespace uwp
} // namespace react
