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
class DeviceInfo {
 public:
  DeviceInfo();

  folly::dynamic GetDimensionsConstants() {
    return m_dimensions;
  }
  void update();

 private:
  folly::dynamic getDimensions(
      winrt::Windows::Graphics::Display::DisplayInformation displayInfo,
      winrt::Windows::UI::Core::CoreWindow window);

  folly::dynamic m_dimensions;
};

class DeviceInfoModule : public facebook::xplat::module::CxxModule,
                         std::enable_shared_from_this<DeviceInfoModule> {
 public:
  DeviceInfoModule(std::shared_ptr<DeviceInfo> deviceInfo);

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char *name;

 private:
  winrt::Windows::UI::Xaml::Application::LeavingBackground_revoker
      m_leavingBackgroundRevoker;
  void sendDimensionsChangedEvent();
  std::shared_ptr<DeviceInfo> m_deviceInfo;
};

} // namespace uwp
} // namespace react
