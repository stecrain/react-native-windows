// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <Utils/XamlDirectInstance.h>

namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::Foundation;
}

namespace react { namespace uwp {

typedef winrt::DependencyObject XamlView;

inline int64_t GetTag(XamlView view)
{
  auto direct = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(winrt::box_value(view));
  return XamlDirectInstance::GetXamlDirect().GetObjectProperty(direct, XD::XamlPropertyIndex::FrameworkElement_Tag).as<winrt::IPropertyValue>().GetInt64();
}

inline void SetTag(XamlView view, int64_t tag)
{
  auto direct = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(winrt::box_value(view));
  
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(direct, XD::XamlPropertyIndex::FrameworkElement_Tag, winrt::PropertyValue::CreateInt64(tag));
}

inline void SetTag(XamlView view, winrt::IInspectable tag)
{
  /*auto xamlDirect = winrt::XamlDirect::GetDefault();
  auto direct = xamlDirect.GetXamlDirectObject(winrt::box_value(view));
  xamlDirect.SetObjectProperty(direct, winrt::XamlPropertyIndex::FrameworkElement_Tag, tag);*/

  SetTag(view, tag.as<winrt::IPropertyValue>().GetInt64());
}

} }
