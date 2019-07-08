// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ControlViewManager.h>
#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <XamlDirectInstance.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
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
    auto controlXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(control);

    for (const auto& pair : reactDiffMap.items())
    {
      const std::string& propertyName = pair.first.getString();
      const folly::dynamic& propertyValue = pair.second;
      auto borderProperty = propertyName == "borderColor" ? XD::XamlPropertyIndex::Control_BorderBrush : XD::XamlPropertyIndex::Control_BorderThickness;

      if (TryUpdateBackgroundBrush(controlXD, propertyName, propertyValue, XD::XamlPropertyIndex::Control_Background))
      {
        continue;
      }
      else if (TryUpdateBorderProperties(nodeToUpdate, controlXD, propertyName, propertyValue, borderProperty))
      {
        continue;
      }
      else if (TryUpdateForeground(controlXD, propertyName, propertyValue, XD::XamlPropertyIndex::Control_Foreground))
      {
        continue;
      }
      else if (implementsPadding && TryUpdatePadding(nodeToUpdate, controlXD, propertyName, propertyValue, XD::XamlPropertyIndex::Control_Padding))
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
