// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ControlViewManager.h>
#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Core.Direct.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Core::Direct;
}

namespace react { namespace uwp {

ControlViewManager::ControlViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

folly::dynamic ControlViewManager::GetNativeProps() const
{
  folly::dynamic props = Super::GetNativeProps();
  props.update(folly::dynamic::object
    ("tabIndex", "number")
  );
  return props;
}

void ControlViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto control(nodeToUpdate->GetView().as<winrt::Control>());

  bool implementsPadding = nodeToUpdate->ImplementsPadding();

  if (control != nullptr)
  {
    for (const auto& pair : reactDiffMap.items())
    {
      const std::string& propertyName = pair.first.getString();
      const folly::dynamic& propertyValue = pair.second;
      auto borderPropXD = propertyName == "borderColor" ? winrt::XamlPropertyIndex::Control_BorderBrush : winrt::XamlPropertyIndex::Control_BorderThickness;

      if (TryUpdateBackgroundBrush(control, propertyName, propertyValue, winrt::XamlPropertyIndex::Control_Background))
      {
        continue;
      }
      else if (TryUpdateBorderProperties(nodeToUpdate, control, propertyName, propertyValue, borderPropXD))
      {
        continue;
      }
      else if (TryUpdateForeground(control, propertyName, propertyValue, winrt::XamlPropertyIndex::Control_Foreground))
      {
        continue;
      }
      else if (implementsPadding && TryUpdatePadding(nodeToUpdate, control, propertyName, propertyValue, winrt::XamlPropertyIndex::Control_Padding))
      {
        continue;
      }
      else if (propertyName == "tabIndex")
      {
        if (propertyValue.isNumber())
        {
          auto tabIndex = propertyValue.asDouble();
          if (tabIndex == static_cast<int32_t>(tabIndex))
            control.TabIndex(static_cast<int32_t>(tabIndex));
        }
        else if (propertyValue.isNull())
        {
          control.ClearValue(winrt::Control::TabIndexProperty());
        }
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} }
