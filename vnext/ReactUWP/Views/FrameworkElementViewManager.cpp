// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/FrameworkElementViewManager.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <Utils/AccessibilityUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <WindowsNumerics.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
}

namespace react { namespace uwp {

FrameworkElementViewManager::FrameworkElementViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

void FrameworkElementViewManager::TransferDoubleProperty(XamlView oldView, XamlView newView, XD::XamlPropertyIndex prop)
{
  auto oldValue = XamlDirectInstance::GetXamlDirect().GetDoubleProperty(oldView, prop);
  if (oldValue != 0)
    XamlDirectInstance::GetXamlDirect().SetDoubleProperty(newView, prop, oldValue);
}

void FrameworkElementViewManager::TransferProperties(XamlView oldView, XamlView newView)
{
  // Render Properties
  TransferDoubleProperty(oldView, newView, XD::XamlPropertyIndex::UIElement_Opacity);

  if (oldView.try_as<winrt::IUIElement9>())
  {
    auto oldElement = oldView.as<winrt::UIElement>();
    auto newElement = newView.as<winrt::UIElement>();
    newElement.TransformMatrix(oldElement.TransformMatrix());
  }

  // Layout Properties
  TransferDoubleProperty(oldView, newView, XD::XamlPropertyIndex::FrameworkElement_Width);
  TransferDoubleProperty(oldView, newView, XD::XamlPropertyIndex::FrameworkElement_Height);
  TransferDoubleProperty(oldView, newView, XD::XamlPropertyIndex::FrameworkElement_MinWidth);
  TransferDoubleProperty(oldView, newView, XD::XamlPropertyIndex::FrameworkElement_MinHeight);
  TransferDoubleProperty(oldView, newView, XD::XamlPropertyIndex::FrameworkElement_MaxWidth);
  TransferDoubleProperty(oldView, newView, XD::XamlPropertyIndex::FrameworkElement_MaxHeight);

  // Accessibility Properties
  auto oldName = XamlDirectInstance::GetXamlDirect().GetStringProperty(oldView, XD::XamlPropertyIndex::AutomationProperties_Name);
  XamlDirectInstance::GetXamlDirect().SetStringProperty(newView, XD::XamlPropertyIndex::AutomationProperties_Name, oldName);

  auto oldLiveSetting = XamlDirectInstance::GetXamlDirect().GetEnumProperty(oldView, XD::XamlPropertyIndex::AutomationProperties_LiveSetting);
  XamlDirectInstance::GetXamlDirect().SetEnumProperty(newView, XD::XamlPropertyIndex::AutomationProperties_LiveSetting, oldLiveSetting);

  auto oldAccessibilityView = XamlDirectInstance::GetXamlDirect().GetEnumProperty(oldView, XD::XamlPropertyIndex::AutomationProperties_AccessibilityView);
  XamlDirectInstance::GetXamlDirect().SetEnumProperty(newView, XD::XamlPropertyIndex::AutomationProperties_AccessibilityView, oldAccessibilityView);

  auto toolTip = XamlDirectInstance::GetXamlDirect().GetObjectProperty(oldView, XD::XamlPropertyIndex::ToolTipService_ToolTip);
  XamlDirectInstance::GetXamlDirect().ClearProperty(oldView, XD::XamlPropertyIndex::ToolTipService_ToolTip);
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(newView, XD::XamlPropertyIndex::ToolTipService_ToolTip, toolTip);
}

folly::dynamic FrameworkElementViewManager::GetNativeProps() const
{
  folly::dynamic props = Super::GetNativeProps();
  props.update(folly::dynamic::object
    ("accessibilityHint", "string")
    ("accessibilityLabel", "string")
    ("testID", "string")
    ("tooltip", "string")
  );
  return props;
}


void FrameworkElementViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto element(nodeToUpdate->GetView());
  if (element != nullptr)
  {
    for (const auto& pair : reactDiffMap.items())
    {
      const std::string& propertyName = pair.first.getString();
      const folly::dynamic& propertyValue = pair.second;

      if (propertyName == "opacity")
      {
        if (propertyValue.isNumber())
        {
          double opacity = propertyValue.asDouble();
          if (opacity >= 0 && opacity <= 1)
            XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::UIElement_Opacity, opacity);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::UIElement_Opacity);
          continue;
        }
      }
      else if (propertyName == "transform")
      {
        if (element.try_as<winrt::IUIElement9>()) // Works on RS5+
        {
          if (propertyValue.isArray())
          {
            assert(propertyValue.size() == 16);
            winrt::Windows::Foundation::Numerics::float4x4 transformMatrix;
            transformMatrix.m11 = static_cast<float>(propertyValue[0].asDouble());
            transformMatrix.m12 = static_cast<float>(propertyValue[1].asDouble());
            transformMatrix.m13 = static_cast<float>(propertyValue[2].asDouble());
            transformMatrix.m14 = static_cast<float>(propertyValue[3].asDouble());
            transformMatrix.m21 = static_cast<float>(propertyValue[4].asDouble());
            transformMatrix.m22 = static_cast<float>(propertyValue[5].asDouble());
            transformMatrix.m23 = static_cast<float>(propertyValue[6].asDouble());
            transformMatrix.m24 = static_cast<float>(propertyValue[7].asDouble());
            transformMatrix.m31 = static_cast<float>(propertyValue[8].asDouble());
            transformMatrix.m32 = static_cast<float>(propertyValue[9].asDouble());
            transformMatrix.m33 = static_cast<float>(propertyValue[10].asDouble());
            transformMatrix.m34 = static_cast<float>(propertyValue[11].asDouble());
            transformMatrix.m41 = static_cast<float>(propertyValue[12].asDouble());
            transformMatrix.m42 = static_cast<float>(propertyValue[13].asDouble());
            transformMatrix.m43 = static_cast<float>(propertyValue[14].asDouble());
            transformMatrix.m44 = static_cast<float>(propertyValue[15].asDouble());
            XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::UIElement_TransformMatrix, winrt::box_value(transformMatrix));
          }
          else if (propertyValue.isNull())
          {
            XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::UIElement_TransformMatrix, winrt::box_value(winrt::Windows::Foundation::Numerics::float4x4::identity()));
          }
        }
      }
      else if (propertyName == "width")
      {
        if (propertyValue.isNumber())
        {
          double width = propertyValue.asDouble();
          if (width >= 0)
            XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::FrameworkElement_Width, width);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_Width);
          continue;
        }

      }
      else if (propertyName == "height")
      {
        if (propertyValue.isNumber())
        {
          double height = propertyValue.asDouble();
          if (height >= 0)
            XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::FrameworkElement_Height, height);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_Height);
          continue;
        }
      }
      else if (propertyName == "minWidth")
      {
        if (propertyValue.isNumber())
        {
          double minWidth = propertyValue.asDouble();
          if (minWidth >= 0)
            XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::FrameworkElement_MinWidth, minWidth);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_MinWidth);
          continue;
        }
      }
      else if (propertyName == "maxWidth")
      {
        if (propertyValue.isNumber())
        {
          double maxWidth = propertyValue.asDouble();
          if (maxWidth >= 0)
            XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::FrameworkElement_MaxWidth, maxWidth);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_MaxWidth);
          continue;
        }

      }
      else if (propertyName == "minHeight")
      {
        if (propertyValue.isNumber())
        {
          double minHeight = propertyValue.asDouble();
          if (minHeight >= 0)
            XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::FrameworkElement_MinHeight, minHeight);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_MinHeight);
          continue;
        }
      }
      else if (propertyName == "maxHeight")
      {
        if (propertyValue.isNumber())
        {
          double maxHeight = propertyValue.asDouble();
          if (maxHeight >= 0)
            XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::FrameworkElement_MaxHeight, maxHeight);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_MaxHeight);
          continue;
        }

      }
      else if (propertyName == "accessibilityHint")
      {
        if (propertyValue.isString())
        {
          auto value = react::uwp::asHstring(propertyValue);
          XamlDirectInstance::GetXamlDirect().SetStringProperty(element, XD::XamlPropertyIndex::AutomationProperties_HelpText, value);
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::AutomationProperties_HelpText);
        }
      }
      else if (propertyName == "accessibilityLabel")
      {
        if (propertyValue.isString())
        {
          auto value = react::uwp::asHstring(propertyValue);
          XamlDirectInstance::GetXamlDirect().SetStringProperty(element, XD::XamlPropertyIndex::AutomationProperties_Name, value);
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::AutomationProperties_Name);
        }
        AnnounceLiveRegionChangedIfNeeded(XamlDirectInstance::GetXamlDirect().GetObject(element).as<winrt::FrameworkElement>());
      }
      else if (propertyName == "accessible")
      {
        if (propertyValue.isBool())
        {
          if (!propertyValue.asBool())
            XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::AutomationProperties_AccessibilityView, static_cast<uint32_t>(winrt::Peers::AccessibilityView::Raw));
        }
      }
      else if (propertyName == "accessibilityLiveRegion")
      {
        if (propertyValue.isString())
        {
          auto value = propertyValue.getString();

          auto liveSetting = winrt::AutomationLiveSetting::Off;

          if (value == "polite")
          {
            liveSetting = winrt::AutomationLiveSetting::Polite;
          }
          else if (value == "assertive")
          {
            liveSetting = winrt::AutomationLiveSetting::Assertive;
          }

          XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::AutomationProperties_LiveSetting, static_cast<uint32_t>(liveSetting));
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::AutomationProperties_LiveSetting);
        }
        AnnounceLiveRegionChangedIfNeeded(XamlDirectInstance::GetXamlDirect().GetObject(element).as<winrt::FrameworkElement>());
      }
      else if (propertyName == "testID")
      {
        if (propertyValue.isString())
        {
          auto value = react::uwp::asHstring(propertyValue);
          XamlDirectInstance::GetXamlDirect().SetStringProperty(element, XD::XamlPropertyIndex::AutomationProperties_AutomationId, value);
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::AutomationProperties_AutomationId);

        }
      }
      else if (propertyName == "tooltip")
      {
        if (propertyValue.isString())
        {
          winrt::TextBlock tooltip = winrt::TextBlock();
          tooltip.Text(asHstring(propertyValue));
          XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::ToolTipService_ToolTip, winrt::box_value(tooltip));
        }
      }
      else if (propertyName == "zIndex")
      {
        if (propertyValue.isNumber())
        {
          auto value = static_cast<int>(propertyValue.asDouble());
          XamlDirectInstance::GetXamlDirect().SetInt32Property(element, XD::XamlPropertyIndex::Canvas_ZIndex, value);
        }
        else if (propertyValue.isNull())
        {
          XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Canvas_ZIndex);
        }
      }
      else if (TryUpdateFlowDirection(element, propertyName, propertyValue))
      {
        continue;
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} }
