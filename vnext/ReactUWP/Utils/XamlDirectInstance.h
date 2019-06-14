// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Xaml.Core.Direct.h>

namespace XD {
  using namespace winrt::Windows::UI::Xaml::Core::Direct;
}

class XamlDirectInstance
{
public:
  static inline XD::IXamlDirect GetXamlDirect() {
    if (!m_xamlDirectInstance) {
      m_xamlDirectInstance = XD::XamlDirect::GetDefault();
    }
    return m_xamlDirectInstance;
  }

private:
  XamlDirectInstance() {}
  static XD::IXamlDirect m_xamlDirectInstance;
};
